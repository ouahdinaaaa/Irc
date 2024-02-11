/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:23:33 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/10 19:50:06 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>
#include "../include/serveur.hpp"
#include "algorithm"

extern int ctrl_c;

int    serveur::FirstParam()
{
    signal(SIGINT, signal_ctrl_c);
    std::string header = create_header();
    std::cout << header << std::endl;
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket == -1)
        return -1;
    int option = 1;
    if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
        throw std::runtime_error("Setsockopt failed");
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
    close(epollFd);
    close(this->_socket);
    return 0;
}

serveur::serveur(char *port, char *mdp) : _mdp(mdp), _port(atoi(port)), _channel(), _client(), _ret(0), _NickName(0)
{
    FirstParam();
}


void setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

void    serveur::connexion(int epollFd)
{
    char buffer[1024];
    epoll_event event;
    int ret;
    std::vector<int> welcomeSocket;
    epoll_event* events = new epoll_event[MAX_EVENTS];
    event.events = EPOLLIN | EPOLLET; 
    int clientSocket;
    CreateBot(epollFd, event);
    while (true)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1){
            // EveryDelete(epoll)
            close(clientSocket);
            close(epollFd);
            close(this->_socket);
            return ;
        }
        if (ctrl_c == 1 && numEvents > 0)
            return (EveryDelete(epollFd, events, event));
        else if (ctrl_c == 1)
        {
            close(epollFd);
            close(this->_socket);
            return ;
        }
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
                if (ret == -1){
                    return ;
                }
                buffer[ret] = '\0';
                retrieve_cmd(ret, buffer, event, events, i, epollFd);
            }
        }
    }
    delete []events;
}

void    serveur::Use(std::string command, int socket, epoll_event event, int epollFd)
{
    std::string Chan; 
    std::string newCmd;

    std::vector<std::string> spliit;
    int size = command.find(' ');
    newCmd = command.substr(0, size);
    std::cout << "New cmd : [" << newCmd << "]" << std::endl;
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
        std::cout << "New cmd : [" << Chan << "]" << std::endl;
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
        int len = command.find(':', command.find('#') + 1) - command.find('#') - 2 ;
        std::string message = command.substr(command.find('#') + 1, len);
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
    else if (newCmd == "QUIT") {
        Delete(socket);
        epoll_ctl(epollFd,  EPOLL_CTL_DEL, socket, &event);
        close(socket);
    }
    //creer un bot               [FAIT DOIT ETRE AMELIORER POUR ENVOYER MSG EN PV]
    //Mocupper de la cmd quit    [FAIT]
    //Mocupper du signaux ctrl C [FAIT]
    //Mocupper du signaux ctrl D [FAIT]
}


void    serveur::retrieve_cmd(int ret, char *buffer, epoll_event event, epoll_event* events, int i, int epollFd)
{ 
    std::string string = buffer;
    std::string command = "";
    int j = 0;
    int size;
    int count = -1;
    if (((int)string.find('\n') == -1))
    {
        if ((int)string.find('\r') == -1)
            this->_commands[events[i].data.fd] += string + "\r\n";
    } 
    else
    {
        if ((int)string.find('\r') == -1)
        {
            std::cout << "open in here" << std::endl;
            this->_commands[events[i].data.fd] += string;
        }
        else
            this->_commands[events[i].data.fd] += string;
    }
    /*
        Good maintenant rajouter fonction qui va clear le buffer en mettant seuleument 1 seule espace entre chaque Mot verifie quan manque argument
    */
    std::cout << "Buffer : [" << buffer << "]" << std::endl;
    std::cout << "\n--------------- Buffer this->cmd : [" << this->_commands[events[i].data.fd] << "]   |||| socket : [" << events[i].data.fd << "]-----------------------\n" << std::endl;
    if ((int)string.find('\n') == -1)
        return ;
    if (ret > 0 && this->_commands[events[i].data.fd].find('\n')) {
        while (j < (int)this->_commands[events[i].data.fd].length()) {
            size = this->_commands[events[i].data.fd].find('\n', j);
            if (size == -1)
                size = this->_commands[events[i].data.fd].find('\r', j) - 1;
            count = size - j - 1;
            command = this->_commands[events[i].data.fd].substr(j, count);
            std::cout << "command : [" << command << "]" << std::endl;
            Use(command, events[i].data.fd, event, epollFd);
            command = "";
            j = size + 1;
        }
    }
    this->_commands[events[i].data.fd] = "";
}


/*
            COMMENT ENVOYER ET RECEVOIR FICHIER

            - /dcc send <nickname> <filename> filename : nom du fichier
            - /dcc accept connexion etablie grace au dcc maintenant le client doit laccepter puis recoit le fichier ||| sert a rien.
            - recoit le fichier

            verifier Valgrind
            Faire bot implemeter command bot.
            Envoie msg aleatoire plus couleur aleatoir
            Pour le Bot changer pour que le msg senvoie en pv
            Poour le Kick penser a bien verifier quil detiens bien les proprieter de moderator +o [FAIT]
            Rajouter si membre operator a quitter chann doit pouvoit donner les pouvoir au second qui a rejoint
*/