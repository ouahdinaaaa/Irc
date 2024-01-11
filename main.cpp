/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 17:03:35 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/11 12:02:40 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

# define RPL_WELCOME(nickname) (":localhost 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "\r")

const int MAX_EVENTS = 10;

void setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        // Gestion de l'erreur
        return -1;
    }
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6667); // Port IRC standard
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        // Gestion de l'erreur
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
        // Gestion de l'erreur
        close(serverSocket);
        return -1;
    }
    
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        // Gestion de l'erreur
        close(serverSocket);
        return -1;
    }

    epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Événements à surveiller (lecture et mode Edge Triggered)
    event.data.fd = serverSocket;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event) == -1) {
        // Gestion de l'erreur
        close(serverSocket);
        close(epollFd);
        return -1;
    }

    epoll_event* events = new epoll_event[MAX_EVENTS];

    char buffer[1024];
    int ret;
    std::vector<int> welcomeSocket;
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
                if (ret == 0)
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &event);
                // std::cout << "phrase recup : [" << buffer << "]" << std::endl;
                std::vector<int>::iterator it = std::find(welcomeSocket.begin(), welcomeSocket.end(), clientSocket);
                if (it == welcomeSocket.end()) 
                {
                    std::string aya("aya");
                    std::string welcomeMessage = RPL_WELCOME(aya); 
                    send(events[i].data.fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
                    welcomeSocket.push_back(clientSocket); // Ajouter le clientSocket à welcomeSocket après l'envoi du message de bienvenue
                }
                if (ret == -1)
                {
                    // free close break 
                    std::cout << "test" << std::endl;
                    return 1;
                }
                else if (ret == 0)
                {
                //     epoll_ctl(epollFd,  EPOLL_CTL_DEL, clientSocket, &event);
                    std::cout << "Socket : " << events[i].data.fd << " disconnect !!!" << std::endl; // traitement des deconnexion 
                }
                // Lecture ou traitement des données pour les clients connectés
                // Gérez la réception et l'envoi de données pour chaque socket client
                // Traitez les commandes IRC et la communication avec les clients
                // N'oubliez pas de gérer les déconnexions et de supprimer les sockets fermés de l'epoll
            }
        }
    }
    delete[] events;
    close(serverSocket);
    close(epollFd);
    return 0;
}