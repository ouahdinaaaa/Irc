/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:23:33 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/13 20:11:09 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"

int    serveur::FirstParam()
{
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket == -1) {
        // Gestion de l'erreur
        return -1;
    }
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

serveur::serveur(char *port, char *mdp) : _mdp(mdp), _port(atoi(port))
{
    FirstParam();
}

void    serveur::JoinCommand(const std::string &channelName, const std::string &userName)
{
    std::string message = RPL_JOIN(userName, channelName);
    Channel Name(channelName);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Name);
    // std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), std::string(channelName.c_str()));
    if (it != this->_channel.end())
    {
        it->Add(userName);
        SendRPL(this->_socket, message);
        // send Message to cient RPL;
    }
    else
    {
        Channel NewChan(channelName);
        NewChan.Add(userName);
        this->_channel.push_back(NewChan);
        SendRPL(this->_socket, message);
        //Envoyer msg To client RPL;
    }
    return ; 
}

void    SendRPL(int socket, std::string message)
{
    send(socket, message.c_str(), message.length(), 0);
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
                // std::vector<int>::iterator it = std::find(welcomeSocket.begin(), welcomeSocket.end(), clientSocket);
                // if (it == welcomeSocket.end()) 
                // {
                    std::string aya("aya🤩");
                    std::string welcomeMessage = RPL_WELCOME(aya); 
                    welcomeSocket.push_back(clientSocket); // Ajouter le clientSocket à welcomeSocket après l'envoi du message de bienvenue
                    SendRPL(clientSocket, welcomeMessage);
                    // send(events[i].data.fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
                    
                // }
            }
            else
            {
                ret = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                // if (ret == 0)
                //     epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &event);
                // std::cout << "phrase recup : [" << buffer << "]" << std::endl;
                if (ret == -1)
                {
                    // free close break 
                    std::cout << "test" << std::endl;
                    return ;
                }
                else if (ret == 0)
                {
                    epoll_ctl(epollFd,  EPOLL_CTL_DEL, events[i].data.fd, &event);
                    std::cout << "Socket : " << events[i].data.fd << " disconnect !!!" << std::endl; // traitement des deconnexion 
                     close(events[i].data.fd);
                }
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

void    serveur::Use(std::string command)
{
    std::string newCmd;
    std::string Chan;
    
    newCmd = command.substr(0, 5);
    Chan = command.substr(5, command.length());
    if (newCmd == "join")
        JoinCommand(Chan, "aya");
}

void    serveur::retrieve_cmd(int ret, char *buffer, epoll_event* events, int i)
{ 
    if (ret > 0) {
    // Les données ont été correctement reçues
    for (int j = 0; j < ret; ++j) {
        if (buffer[j] == '\n') {
            // Une commande complète a été reçue
            std::string command(buffer, j -1);
            // Traitez la commande ici (par exemple, affichez-la)
            std::cout << "Command received from socket " << events[i].data.fd << ": cmd is [" << command << "] ||\n" << std::endl;
            Use(command);
            // Effacez le tampon pour la prochaine lecture
            // memset(buffer, 0, sizeof(buffer));
            }
        }
    }
}