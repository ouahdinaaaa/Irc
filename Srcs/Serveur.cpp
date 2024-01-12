/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:23:33 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/12 19:03:40 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"




const int MAX_EVENTS = 10;

void    serveur::JoinCommand(const std::string &channelName, const std::string &userName)
{
    Channel Name(channelName);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Name);
    // std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), std::string(channelName.c_str()));
    if (it != this->_channel.end())
    {
        it->Add(userName);
        SendRPL(this->_socket, "Bienvenue sur le canal par defaut : ");
        // send Message to cient RPL;
    }
    else
    {
        Channel NewChan(channelName);
        NewChan.Add(userName);
        this->_channel.push_back(NewChan);
        SendRPL(this->_socket, "Bienvenue sur le canal par defaut : ");
        //Envoyer msg To client RPL;
    }
    return ; 
}

void    SendRPL(int socket, const char *message)
{
    send(socket, message, strlen(message), 0);
}


void setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

void    serveur::connexion(int ret, int epollFd, int serverSocket)
{
    char buffer[1024];
    epoll_event event;
    std::vector<int> welcomeSocket;
    epoll_event* events = new epoll_event[MAX_EVENTS];
    event.events = EPOLLIN | EPOLLET; 
    int clientSocket;
    while (true) {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == serverSocket)
            {
                // Nouvelle connexion entrante
                sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
                setNonBlocking(clientSocket);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clientSocket;
                epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event);
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
                //     epoll_ctl(epollFd,  EPOLL_CTL_DEL, clientSocket, &event);
                    std::cout << "Socket : " << events[i].data.fd << " disconnect !!!" << std::endl; // traitement des deconnexion 
                }
                std::vector<int>::iterator it = std::find(welcomeSocket.begin(), welcomeSocket.end(), clientSocket);
                if (it == welcomeSocket.end()) 
                {
                    std::string aya("aya");
                    std::string welcomeMessage = RPL_WELCOME(aya); 
                    welcomeSocket.push_back(clientSocket); // Ajouter le clientSocket à welcomeSocket après l'envoi du message de bienvenue
                    SendRPL(events[i].data.fd, "Welcome in you server");
                    // send(events[i].data.fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
                    
                }
                std::cout << "buffer rec : [" << buffer << "]" << std::endl; 
                // Lecture ou traitement des données pour les clients connectés
                // Gérez la réception et l'envoi de données pour chaque socket client
                // Traitez les commandes IRC et la communication avec les clients
                // N'oubliez pas de gérer les déconnexions et de supprimer les sockets fermés de l'epoll
            }
        }
    }
}