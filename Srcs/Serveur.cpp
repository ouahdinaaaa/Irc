/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:23:33 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/17 17:37:35 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"

int    serveur::FirstParam()
{
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

void    serveur::JoinCommand(const std::string &channelName, Client userName)
{
    std::string message;
    Channel Name(channelName);
    // std::cout << "Channel create in : [" << Name.getname() << "]" << std::endl;
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Name);
    if (it != this->_channel.end())
    {
        it->Add(userName);
        message = RPL_JOIN(userName.get_user(), channelName);
        Name = *it;
        SendRPL(userName.get_socket(), message);
        SendMsg(Name, message, userName.get_socket());
    }
    else
    {
        Channel NewChan(channelName);
        NewChan.Add(userName);
        this->_channel.push_back(NewChan);
        message = RPL_JOIN(userName.get_user(), channelName);
        Name = NewChan;
        SendRPL(userName.get_socket(), message);
        SendMsg(Name, message, userName.get_socket());
    }
    return ; 
}

void    SendRPL(int socket, std::string message)
{
    ssize_t size =  send(socket, message.c_str(), message.length(), 0);
    if(size == -1)
        std::cout << "You RPL in not good !!!" << std::endl;
}


void setNonBlocking(int sockfd) 
{
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
    while (true) 
    {
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
                    // free close break 
                    std::cout << "test" << std::endl;
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
                retrieve_cmd(ret, buffer, events, i);
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


void    serveur::SendMsg(Channel &Channel, std::string &msg, int socket)
{
    Client userName = getUser(socket);
    std::vector<Client>::iterator it;
    std::string message = RPL_PRIVMSG_CHANNEL(userName.get_user(), Channel.getname(), msg);
    for (it = (Channel.get_client()).begin(); it != (Channel.get_client()).end(); ++it) {
        if ((*it).get_socket() != socket)
            SendRPL((*it).get_socket(), message);
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

    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it)
    {
        if ((*it).get_socket() == socket)
            return *it;
    }
    it = this->_client.end();
    return *it;
}

void    serveur::Use(std::string command, int socket)
{
    std::string newCmd;
    std::string Chan;
    
    std::vector<std::string> spliit;
    int size = command.find(' ');
    newCmd = command.substr(0, size);
    if (newCmd == "CAP")
        this->_ret = 1;
    if (newCmd == "NICK" && this->_ret){
        Chan = command.substr(size + 1, command.length());
        std::string msg = RPL_WELCOME(Chan);
        Client newClient(Chan, socket);
        this->_client.push_back(newClient);
        this->_ret = 0;
        std::cout << "----- mdp : [" << _mdp << "]  |||| mdp recu : [" << _mdprecu << "] ----- " << std::endl;
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
        Chan = command.substr(size, command.length());
        JoinCommand(Chan, getUser(socket));
    }
    else if (newCmd == "PRIVMSG"){
        size = command.find(':');
        int len = size - command.find('#') - 2;
        std::string Channel = command.substr((command.find('#') + 1), len);
        Chan = command.substr(size, command.length());
        PrivMsg(Channel, Chan, socket);
    }
}

void    serveur::retrieve_cmd(int ret, char *buffer, epoll_event* events, int i)
{ 
    std::string string = buffer;
    std::string command = "";
    int j = 0;
    int size;
    if (ret > 0) {
        while (j < (int)string.length()){
            size = string.find('\n', j);
            if (size == -1)
                size = string.find('\r', j) - 1;
            command = string.substr(j, (size - j - 1));
            std::cout << "------ Command : [" << command << "] ------ " << std::endl;
            Use(command, events[i].data.fd);
            command = "";
            j = size + 1;
        }
    }
}