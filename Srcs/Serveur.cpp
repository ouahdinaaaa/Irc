/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:23:33 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/05 18:02:19 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>
#include "../include/serveur.hpp"
#include "algorithm"

extern int ctrl_c;

void    signal_ctrl_c(int signal)
{
    if (signal == SIGINT) {
        ctrl_c = 1;
    }
}

int    serveur::FirstParam()
{
    // std::string header = create_header();
    // std::cout << header << std::endl;
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket == -1)
        return -1;
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->_socket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        // Gestion de l'erreur
        close(this->_socket);
        return -1;
    }
    if (listen(this->_socket, SOMAXCONN) == -1) {
        // Gestion de l'erreur
        close(this->_socket);
        return -1;
    }
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        // Gestion de l'erreur
        close(this->_socket);
        return -1;
    }
    epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Événements à surveiller (lecture et mode Edge Triggered)
    event.data.fd = this->_socket;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, this->_socket, &event) == -1) {
        // Gestion de l'erreur
        close(this->_socket);
        close(epollFd);
        return -1;
    }
    connexion(epollFd);
    close(this->_socket);
    close(epollFd);
    return 0;
}

serveur::serveur(char *port, char *mdp) : _mdp(mdp), _port(atoi(port)), _channel(), _client(), _ret(0) 
{
    FirstParam();
}

void    serveur::JoinCommand(const std::string &channelName, Client userName, int socket, int key)
{
    std::string message;
    std::string Namely;
    std::string message2;
    Channel Name(channelName);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Name);
    if (it != this->_channel.end()) {
        // std::cout << "open in here !!" << std::endl;
        // std::cout << "\n\nget mode i : [" << (it)->Getmode_i() << "]\n\n" << std::endl;
        if (it->Getmode_i() || (it->Getmode_l() && it->Getmode_l() == (int)(it->get_client()).size()) || it->Getmode_k()) 
        {
            if (it->Getmode_i() && !userName.VerifInvitation(channelName) && !userName.GetOperator())
                return (SendRPL(socket, ERR_INVITEONLYCHAN(userName.get_user(), channelName)));
            if (it->Getmode_l() && it->Getmode_l() == (int)(it->get_client()).size() && !userName.GetOperator())
                return(SendRPL(socket, ERR_CHANNELISFULL(userName.get_user(), channelName)));
            if (it->Getmode_k() && it->Getmode_k() != key && !userName.GetOperator())
                return (SendRPL(socket, ERR_BADCHANNELKEY(userName.get_user(), channelName)));
            else  {
                std::vector<Client>::iterator its = std::find(this->_client.begin(), this->_client.end(), userName);
                its->Newlist(channelName);
            }
        }
        Namely = RPL_NAMREPLY(userName.get_user(), channelName, (*it).ListClient());
        std::string EndName = RPL_ENDOFNAMES(userName.get_user(), channelName);
        it->Add(userName);
        message = RPL_JOIN(userName.get_user(), userName.get_name(), channelName);
        Name = *it;
        SendRPL(userName.get_socket(), message);
        if ((*it).GetTopic().length() > 0) {
            SendRPL(socket, RPL_TOPIC(userName.get_user(), channelName, (*it).GetTopic()));
            SendMsg(Name, RPL_TOPIC(userName.get_user(), channelName, (*it).GetTopic()), socket, 1);
        }
        SendRPL(userName.get_socket(), Namely);
        SendRPL(userName.get_socket(), EndName);
        SendMsg(Name, message, userName.get_socket(), 1);
    }
    else {
        std::vector<Client>::iterator its = std::find(this->_client.begin(), this->_client.end(), userName);
        its->SetOperator(1);
        its->SetImunite();
        Channel NewChan(channelName);
        NewChan.Add(userName);
        this->_channel.push_back(NewChan);
        Name = NewChan;
        Namely = RPL_NAMREPLY(userName.get_user(), channelName, "");
        message = RPL_JOIN(userName.get_user(), userName.get_user(), channelName);
        std::string EndName = RPL_ENDOFNAMES(userName.get_user(), channelName);
        SendRPL(userName.get_socket(), message);
        SendRPL(userName.get_socket(), Namely);
        SendRPL(userName.get_socket(), EndName);
        SendMsg(Name, message, socket, 1);
    }
    return ; 
}

void    SendRPL(int socket, std::string message) {
    ssize_t size =  send(socket, message.c_str(), message.length(), 0);
    if(size == -1)
        std::cout << "You RPL in not good !!!" << std::endl;
}


void setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}


void    serveur::connexion(int epollFd)
{
    int ctrl_c = 0;
    char buffer[1024];
    epoll_event event;
    int ret;
    std::vector<int> welcomeSocket;
    epoll_event* events = new epoll_event[MAX_EVENTS];
    event.events = EPOLLIN | EPOLLET; 
    int clientSocket;
    signal(SIGINT, signal_ctrl_c);
    while (true) 
    {
        if (ctrl_c == 1)
        {
        }
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == this->_socket)
            {
                // Nouvelle connexion entrante
                sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                clientSocket = accept(this->_socket, (sockaddr*)&clientAddr, &clientAddrLen);
                setNonBlocking(clientSocket);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clientSocket;
                epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event);
            }
            else
            {
                ret = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (ret == -1)
                {
                    return ;
                }
                else if (ret == 0)
                {
                    epoll_ctl(epollFd,  EPOLL_CTL_DEL, events[i].data.fd, &event);
                    // std::cout << "Socket : " << events[i].data.fd << " disconnect !!!" << std::endl; // traitement des deconnexion 
                     close(events[i].data.fd);
                }
                buffer[ret] = '\0';
                // std::cout << ": cmd is [" << buffer << "]\n" << std::endl;
                retrieve_cmd(ret, buffer, events, i, epollFd);
                // std::cout << "buffer rec : [" << buffer << "]" << std::endl; 
                // Lecture ou traitement des données pour les clients connectés
                // Gérez la réception et l'envoi de données pour chaque socket client
                // Traitez les commandes IRC et la communication avec les clients
                // N'oubliez pas de gérer les déconnexions et de supprimer les sockets fermés de l'epoll
            }
        }
    }
    delete[] events;
}


void serveur::SendMsg(Channel &Channel, std::string msg, int socket, int boolean)
{
    Client userName = getUser(socket);
    std::vector<Client>::iterator it;
    std::vector<Client> list = Channel.get_client();
    std::string message;
    if (boolean)
        message = RPL_PRIVMSG_CHANNEL(userName.get_user(), Channel.getname(), msg);
    for (it = list.begin(); it != list.end(); ++it) {
        if ((*it).get_socket() != socket) {
            if (!boolean)
                message = RPL_TOPIC((*it).get_user(), Channel.getname(), msg);
            SendRPL((*it).get_socket(), message);
        }
    }
}

void    serveur::PrivChannel(Channel &Channel, int socket, std::string &msg)
{
    std::vector<Client> list = Channel.get_client();
    std::vector<Client>::iterator it = list.begin();
    Client userName = getUser(socket);
    std::string message = RPL_PRIVMSG_CHANNEL(userName.get_user(), Channel.getname(), msg);
    for (it = list.begin(); it != list.end(); ++it){
        if ((*it).get_socket() != socket){
            if ((*it).get_socket() != socket)
                SendRPL((*it).get_socket(), message);
        }
    }
}

void    serveur::PrivMsg(std::string &channel, std::string &msg, int socket)
{
    Channel name(channel);
    std::vector<Channel>::iterator  it = std::find(this->_channel.begin(), this->_channel.end(), name);
    if (it != this->_channel.end()) 
        PrivChannel(*it, socket, msg);
}

Client serveur::getUser(int socket)
{
    std::vector<Client>::iterator it;
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it) {
        if ((*it).get_socket() == socket)
            return *it;
    }
    it = this->_client.end();
    return *it;
}

void    serveur::PrivMsgClient(std::string &name, std::string &message, int socket)
{
    Client user = getUser(socket);
    int newsocket = RetrieveSocket(name);
    std::string msg = RPL_PRIVMSG_CLIENT(user.get_user(), user.get_name(), name, message);
    if (newsocket == -1) {
        std::string msg = ERR_NOSUCHNICK(name);
        return (SendRPL(socket, msg));
    }
    SendRPL(newsocket, msg);
}

void    serveur::PrintClient(Channel &chan)
{
    std::vector<Client> list = chan.get_client();
    for (std::vector<Client>::iterator it = list.begin(); it != list.end(); ++it) {
        std::cout << "name : [" << (*it).get_user() << "]  |||| socket : " << (*it).get_socket() << std::endl;
    }       
}


void    serveur::PartCommand(std::string &channel, int socket, std::string reason) 
{
    (void)reason;
    std::string nameChannel = channel;
    Channel name(channel);
    Client  user = getUser(socket);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), name);
    if (it != this->_channel.end()){
        PrintClient(*it);
        std::vector<Client> Chan = it->get_client();
        std::vector<Client>::iterator its = std::find(Chan.begin(), Chan.end(), user);
        if (its != Chan.end()) {
            std::string msg = PART_CHANEL((*its).get_user(), (*its).get_name(), "PART", nameChannel);
            std::string message = PART((*its).get_user(), (*its).get_name(), (*it).getname(), reason);
            SendRPL(socket, message);
            SendMsg(*it, msg, user.get_socket(), 1);
            it->NewList(*its);
        }
        else {
            std::string message = ERR_NOTONCHANNEL(user.get_user(), channel);
            SendRPL(socket, message);
        }
    }
    else  {
        std::string message = ERR_NOSUCHCHANNEL(user.get_name(), channel);
        SendRPL(socket, message);
    }
}

void    serveur::Topic(std::string &channel, std::string &topic, int socket)
{
    Client       _user = getUser(socket);
    std::string  name = _user.get_user();
    std::string  message;
    int newsocket = RetrieveSocketChan(channel, name);
    if (newsocket < 0) {
        if (newsocket == -1)
            message = ERR_NOSUCHCHANNEL(name, channel);
        else
            message = ERR_NOTONCHANNEL(name, channel);
        SendRPL(socket, message);
    } else {
        Channel chan(channel);
        std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), chan);
        if (it->Getmode_t() && !(_user.GetOperator())){
            message = ERR_NOPRIVILEGES(name, channel);
            return (SendRPL(socket, message));
        }
        it->AddOrChangeTopic(topic);
        message = RPL_TOPIC(name, channel, topic);
        SendRPL(socket, message);
        std::string message2 = SET_TOPIC(name, _user.get_name(), "TOPIC", channel, topic);
        SendMsg((*it), message2, socket, 1);
        SendMsg((*it), topic, socket, 0); // Reflechir a modifier le RPL pour dire a modifier to new topic is et voila
    }
}

void    serveur::ConfigMode(std::string &channel, std::string &mode, int socket)
{
    Channel Name(channel);
    Client _user = getUser(socket);
    std::string name = _user.get_user();
    std::string message;
    int newsocket = RetrieveSocketChan(channel, name);
    if (newsocket < 0) {
        // std::cout << "socket retreiev : " << newsocket << std::endl;
        if (newsocket == -1)
            message = ERR_NOSUCHCHANNEL(name, channel);
        else
            message = ERR_NOTONCHANNEL(name, channel);
        return (SendRPL(socket, message));
    }
    if (mode.length() < 2)
        return ;
    // std::cout << "open in mode : [" << mode << "] ||| channnel is : [" << channel << "]" << std::endl;
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Name);
    if (it != this->_channel.end()){
        if (!_user.GetOperator())
            return (SendRPL(socket, ERR_NOPRIVILEGES(name, channel)));
        it->choice_mode(mode, _user, channel, socket);
    }
}

int serveur::RetrieveSocketChan(std::string &name, std::string &user)
{
    Channel _name(name);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), _name);
    if (it != this->_channel.end()) {
        std::vector<Client> cliennt = it->get_client();
        for (std::vector<Client>::iterator its = cliennt.begin(); its != cliennt.end(); ++its) {
            if ((*its).get_user() == user)
                return ((*its).get_socket());
        }
        return -2; // user is not in chan
    }
    return -1; //channel dont exit
}

int     serveur::RetrieveSocket(std::string &name)
{
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it) {
        if ((*it).get_user() == name)
            return (*it).get_socket();
    }
    return -1;
}

void    serveur::KickUser(std::string &channel, std::string &reason, int socket)
{
    std::string Chan = channel.substr(1, channel.find(' ') - 1);
    Channel name(Chan);
    std::string user = channel.substr(channel.find(' ') + 1, channel.length());
    std::string message;
    int newsocket = RetrieveSocketChan(Chan, user);
    if (newsocket < 0){
        if (newsocket == -2)
            message = ERR_NOTONCHANNEL(user, Chan);
        else if (newsocket == -1)
            message = ERR_NOSUCHCHANNEL(user, Chan);
        SendRPL(socket, message);
    }
    else
    {
        Client userKick = getUser(newsocket);
        Client _user = getUser(socket);
        if (userKick.GetImunite()){
            message = ERR_NOPRIVILEGES(_user.get_user(), channel);
            return (SendRPL(socket, message));
        }
        message = RPL_KICK(_user.get_name(), Chan, user, reason);
        std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), name);
        std::vector<Client> list = (*it).get_client();
        std::vector<Client>::iterator its = std::find(list.begin(), list.end(), userKick);
        if (it != this->_channel.end() && its != list.end()) 
        {
            SendRPL(newsocket, message);
            it->NewList(*its);
        }
    }
}

void    serveur::Invite(std::string &user, std::string &channel, int socket)
{
    Client _user = getUser(socket);
    std::string name = _user.get_user();
    int newsocket = RetrieveSocketChan(channel, name); // verifier que lutilisateur est bien present dans le channel
    if (newsocket < 0) {
        std::string message = ERR_NOTONCHANNEL(name, channel);
        SendRPL(socket, message);
        return ;
    }
    int newsocket2 = RetrieveSocketChan(channel, user); // verifier que le user ne fais pas partie du chan
    if (newsocket2 >= 0) {
        std::string message = ERR_USERONCHANNEL(user, channel);
        SendRPL(socket, message);
        return ;
    }
    int targetsock = RetrieveSocket(user);
    if (targetsock < 0)
        return(SendRPL(socket, ERR_NOSUCHNICK(name))); //penser a envoyer msg derreur user dosent existed
    if (!_user.GetOperator())
        return (SendRPL(socket, ERR_NOPRIVILEGES(name, channel)));
    Client UserInvite = getUser(targetsock);
    std::vector<Client>::iterator it = std::find(this->_client.begin(), this->_client.end(), UserInvite);
    it->getInvitation(channel);
    std::string message = INVITE_CLIENT(name, _user.get_name(), "INVITE", user, channel);
    SendRPL(targetsock, message);
}

void    serveur::Use(std::string command, int socket, epoll_event *events, int i, int epollFd)
{
    (void)i;
    (void)events;
    (void)epollFd;
    std::string Chan; 
    std::string newCmd;

    std::vector<std::string> spliit;
    int size = command.find(' ');
    newCmd = command.substr(0, size);
    if (newCmd == "CAP")
        this->_ret = 1;
    if (newCmd == "NICK" && this->_ret){
        Chan = command.substr(size + 1, command.length());
        if (UserExist(Chan))
            return (SendRPL(socket, ERR_NICKNAMEINUSE(Chan)));
        std::string msg = RPL_WELCOME(Chan);
        Client newClient(Chan, socket);
        (*this)._client.push_back(newClient);
        this->_ret = 0;
        if (_mdp != _mdprecu)
            msg = ERR_PASSWDMISMATCH(Chan);
        SendRPL(socket, msg);
    }
    else if (newCmd == "PING")
        SendRPL(socket, RPL_PONG);
    else if (newCmd == "PASS"){
        Chan = command.substr(size + 1, command.length());
        this->_mdprecu = Chan;
    }
    else if (newCmd == "JOIN"){
        size = command.find('#') + 1;
        int len = command.find(' ', size);
        int key = 0;
        std::string keys;
        if (len == -1)
           Chan = command.substr(size, command.length());
        else {
            len = command.length() - size - 3;
            Chan = command.substr(size, len);
        }   
        if ((int)command.find(' ', size) != -1) {
            keys = command.substr(len + size, command.length());
            key = atoi(keys.c_str());
        }
        JoinCommand(Chan, getUser(socket), socket, key);
    }
    else if (newCmd == "USER") {
        if (GetNickName()) {
            this->_NickName = 0;
            return ; }
        Client user = getUser(socket);
        size_t pos = command.find(' ', size + 1);
        Chan = command.substr(size + 1, pos - (size + 1));
        user.set_user(Chan);
        std::vector<Client>::iterator it = std::find(this->_client.begin(), this->_client.end(), user);
        if (it != this->_client.end()){
            this->_client.erase(it);
            this->_client.push_back(user);
        }
    }
    else if (newCmd == "PART") 
    {
        size = command.find(':');
        if (size != -1) {
            int len = size - command.find('#') - 2;
            std::string channel = command.substr((command.find('#') + 1), len);
            Chan = command.substr(size, command.length());
            PartCommand(channel, socket, Chan);
        }
        else 
        {
            size = command.find('#') + 1;
            int len = command.length() - size;
            std::string channels = command.substr(size, len);
            PartCommand(channels, socket, "Good Bye");
        }
    }
    int len_size = command.find('#');
    if (newCmd == "PRIVMSG" && len_size != -1)
    {
        size = command.find(':');
        int len = size - command.find('#') - 2;
        std::string Channel = command.substr((command.find('#') + 1), len);
        Chan = command.substr(size, command.length());
        PrivMsg(Channel, Chan, socket);
    }
    else if (newCmd == "PRIVMSG" && len_size == -1)
    {
        size = command.find(':') + 1;
        int len = size - command.find(' ', 7) - 3;
        std::string message = command.substr((command.find(' ', 7) + 1) , len);
        Chan = command.substr(size, command.length());
        PrivMsgClient(message, Chan, socket);
    }
    else if (newCmd == "KICK") 
    {
        size = command.find(':') + 1;
        int len = size - command.find(' ', 4) - 3;
        std::string message = command.substr(command.find(' ', 4) + 1, len);
        Chan = command.substr(size, command.length());
        KickUser(message, Chan, socket);
    }
    else if (newCmd == "INVITE")
    {
        size = command.find('#') + 1;
        int len = size - command.find(' ', 6) - 3;
        std::string user = command.substr(command.find(' ', 6) + 1, len);
        Chan = command.substr(size, command.length());
        Invite(user, Chan, socket);
    }
    else if (newCmd == "TOPIC"){
        size = command.find('#') + 1;
        int len = command.find(':') - size - 1;
        std::string channel = command.substr(size, len);
        std::string topic = command.substr(command.find(':') + 1, command.length());
        Topic(channel, topic, socket);
    }
    else if (newCmd == "NAMES")
    {
        size = command.find('#') + 1;
        std::string channel = command.substr(size, command.length());
        Names(channel, socket);
    }
    if (newCmd == "MODE")
    {
        size = command.find('#') + 1;
        if (!size) {
            int len = command.find(' ', 5) - 5;
            std::string user = command.substr(5, len);
            std::string mode = command.substr(command.find(' ', 5) + 1, command.length());                    
            ConfigModeClient(user, mode, socket);
            int socketretriev = RetrieveSocket(user);
            if (socketretriev < 0)
                return (SendRPL(socket, ERR_NOSUCHNICK(user)));
            Client users = getUser(socketretriev);
        }
        else {
            int debut;
            int len = command.find(' ', size) - size;
            debut = len + 1 + size;
            if (len < 0)
                len = command.length() - size;
            std::string channel = command.substr(size, len);
            std::string mode = command.substr(debut, command.length());
            if (debut == 0)
                mode = "";
            ConfigMode(channel, mode, socket);
        }
    }
    //creer un bot
    //Mocupper de la cmd quit
    //Mocupper du signaux ctrl C
    //Mocupper du signaux ctrl D
}

void    serveur::Names(std::string channel, int socket)
{
    Channel Chan(channel);
    Client  user = getUser(socket);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Chan);
    if (it != this->_channel.end()) {
        std::string msg = RPL_NAMREPLY(user.get_user(), channel, it->ListClient());
        SendRPL(socket, msg);
    }
    else {
        std::string msg = ERR_NOSUCHCHANNEL(user.get_name(), channel);
        SendRPL(socket, msg);
    }
}

void    serveur::retrieve_cmd(int ret, char *buffer, epoll_event* events, int i, int epollFd)
{
    std::string string = buffer;
    std::string command = "";
    int j = 0;
    int size;
    if (ret > 0) {
        while (j < (int)string.length()) {
            size = string.find('\n', j);
            if (size == -1)
                size = string.find('\r', j) - 1;
            command = string.substr(j, (size - j - 1));
            std::cout << "command : [" << command << "]" << std::endl;
            Use(command, events[i].data.fd, events, i, epollFd);
            command = "";
            j = size + 1;
        }
    }
}

void    serveur::ConfigModeClient(std::string &user, std::string &mode, int socket)
{
    if (mode.size() != 2)
        return ;
    Client userName = getUser(socket);
    if (mode[1] != 'o' && mode[1] != 'i')
        return (SendRPL(socket, ERR_NEEDMOREPARAMS(userName.get_user(), "MODE")));
    int newsocket = RetrieveSocket(user);
    if (newsocket == -1)
        return ;
    Client newUser = getUser(newsocket);
    std::vector<Client>::iterator it = std::find(this->_client.begin(), this->_client.end(), newUser);
    if (it != this->_client.end()) {
        if (mode[0] == '-' && mode[1] == 'o')
            it->SetOperator(0);
        else if (mode[0] == '+' && mode[1] == 'o')
            it->SetOperator(1);
    }
}

int     serveur::UserExist(std::string &name)
{
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it)     {
        if (name == it->get_user()) {
            this->_NickName = 1;
            return 1;
        }
    }
    return 0;
}


/*
            COMMENT ENVOYER ET RECEVOIR FICHIER

            - /dcc send <nickname> <filename> filename : nom du fichier

            - /dcc accept connexion etablie grace au dcc maintenant le client doit laccepter puis recoit le fichier ||| sert a rien.
            
            - recoit le fichier


            Faire bot implemeter command bot.
            Envoie msg aleatoire plus couleur aleatoir
            verifier Valgrind 
             
*/

