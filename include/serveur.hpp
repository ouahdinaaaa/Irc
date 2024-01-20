/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:36:06 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/20 20:07:33 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVEUR_HPP
#define SERVEUR_HPP

#include "Irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"
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
#include "Numeric_Rpl.hpp"

const int MAX_EVENTS = 10;

class Channel;

class serveur
{
    private:
        int                  _socket;
        std::string          _mdp;
        std::string          _mdprecu;
        int                  _port;
        std::vector<Channel> _channel;
        std::vector<Client>  _client;
        int                  _ret;
        
        public:
        serveur() : _channel(), _client(), _ret(1) {};
        serveur(char *port, char *mdp);
        ~serveur() {};
        void    JoinCommand(const std::string &channelName, Client userName, int socket);
        int     FirstParam();
        void    PartCommand(const std::string &channel, int socket, std::string reason);
        void    Use(std::string command, int socket);
        Client  getUser(int socket);
        void    Password(std::string &msg, int socket);
        void    SendMsg(Channel &Channel, std::string &msg, int socket);
        void    PrivChannel(Channel &Channel, int socket, std::string &msg);
        void    connexion(int epollFd); // gere les connexion entrante
        void    PrivMsg(std::string &channel, std::string &msg, int socket);
        void    retrieve_cmd(int ret, char *buffer, epoll_event* events, int i);
};

void    SendRPL(int socket, std::string message);

#endif