/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:36:06 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/12 19:05:19 by ayael-ou         ###   ########.fr       */
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
        serveur(const std::string &port, std::string &mdp) : _port(atoi(port.c_str())), _mdp(mdp), 
        {};
        ~serveur() {};
        // bool    start(int port);
        void    JoinCommand(const std::string &channelName, const std::string &userName);
        // void    finish();
        void    firstParam();
        void    connexion(int ret, int epollFd, int serverSocket); // gere les connexion entrante
        // void    Conexion_client(Client &client);
};

void    SendRPL(int socket, const char *message);

#endif