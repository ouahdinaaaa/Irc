/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:51:25 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/08 20:39:32 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"

void    serveur::CreateBot(int epollFd, epoll_event event)
{
    int clientSocket;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(1, (sockaddr*)&clientAddr, &clientAddrLen);
    setNonBlocking(clientSocket);
    epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event);
    Client _user("Bot", "ayael-ou", 1);
    this->_client.push_back(_user);
}

void serveur::Bot_cmd(int socket)
{
    Client _bot = getUser(1);
    Client _user = getUser(socket);

    std::string color[8] = {GREEN, BLUE, MAGENTA, RED, ROSE, ORANGE, CYAN, EMERAUDE};
    std::string msgs[6] = {"A GREAT TEAM", "WAOUUUW", "AMAAAAZIIINNG !!!", "TRooop hlouuuu ❤️", "FINISH THIS EVALUTAION", "GOO GOO GOO ITS TIME"};
    int num = rand() % 8;
    int num2 = rand() % 6;
    
    std::cout << "user : " << _user.get_name() << std::endl;
    std::string message = color[num] + msgs[num2] + RESET;
    std::string privmsg = ":" + _bot.get_user() + " PRIVMSG " + _user.get_user() + " :" + message + "\r\n";
    SendRPL(socket, privmsg);
}
