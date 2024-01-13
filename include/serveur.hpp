/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:36:06 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/13 19:00:19 by ayael-ou         ###   ########.fr       */
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


class serveur
{
    private:
        int                  _socket;
        std::string          _mdp;
        int                  _port;
        // std::vector<Client> Client; // creation liste de cliens co
        std::vector<Channel> _channel;
        
        public:
        serveur() : _channel() {};
        serveur(char *port, char *mdp);
        ~serveur() {};
        // bool    start(int port);
        void    JoinCommand(const std::string &channelName, const std::string &userName);
        // void    finish();
        int     FirstParam();
        void    Use(std::string command);
        void    connexion(int epollFd); // gere les connexion entrante
        void    retrieve_cmd(int ret, char *buffer, epoll_event* events, int i);
        // void    Conexion_client(Client &client);
};

void    SendRPL(int socket, std::string message);

#endif