/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:36:06 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/12 17:08:03 by ayael-ou         ###   ########.fr       */
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
#include <sstream>
#include "Numeric_Rpl.hpp"

# define BLACK "\033[30m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define SAPIN "\033[38;2;0;96;50m"
# define RED "\033[31m"
# define EMERAUDE "\033[38;2;0;196;140m"
# define GRIS "\033[255;113;30m"
# define ROSE "\033[38;2;169;30;80m"
# define ORANGE "\033[38;5;166m"
# define RESET "\033[1;97m"


extern int ctrl_c;

const int MAX_EVENTS = 10;

class Channel;

class serveur
{
    private:
        int                             _socket;
        std::string                     _mdp;
        std::string                     _mdprecu;
        std::map<int, std::string>      _mdpPort;
        int                             _port;
        std::vector<Channel>            _channel;
        std::vector<Client>             _client;
        std::map<int, int>              _ret;
        int                             _NickName;
        std::map<int, std::string>      _commands;
        
        public:
        serveur() : _mdpPort(), _channel(), _client(), _ret(), _NickName(0), _commands() {};
        serveur(char *port, char *mdp);
        ~serveur() {};
        void    JoinCommand(const std::string &channelName, Client userName, int socket, int key);
        int     FirstParam();
        void    PartCommand(std::string &channel, int socket, std::string reason);
        void    Use(std::string command, int socket, epoll_event event, int epollFd);
        Client  getUser(int socket);
        int     GetNickName() {return this->_NickName; };
        void    ConfigMode(std::string &channel, std::string &mode, int socket);
        void    ConfigModeClient(std::string &user, std::string &mode, int socket);
        int     RetrieveSocketChan(std::string &name, std::string &user);
        void    KickUser(std::string &channel, std::string &reason, int socket);
        int     RetrieveSocket(std::string &name);
        void    SendMsg(Channel &Channel, std::string msg, int socket, int boolean);
        void    PrivChannel(Channel &Channel, int socket, std::string &msg);
        void    connexion(int epollFd); // gere les connexion entrante
        void    Invite(std::string &user, std::string &channel, int socket);
        void    Topic(std::string &channel, std::string &topic, int socket);
        void    PrivMsgClient(std::string &name, std::string &message, int socket);
        void    PrivMsg(std::string &channel, std::string &msg, int socket);
        void    Names(std::string channel, int socket);
        int     UserExist(std::string &name);
        int     ValidUser(int socket);
        void    PrintClient(Channel &chan);
        void    Delete(int socket);
        void    Bot_cmd(int socket);
        void    CreateBot(int epollFd, epoll_event event);
        void    EveryDelete(int epollFd, epoll_event *events, epoll_event event);
        void    retrieve_cmd(int ret, char *buffer, epoll_event event, epoll_event* events, int i, int epollFd);
};

void    signal_ctrl_c(int signal);
void    setNonBlocking(int sockfd);
void    SendRPL(int socket, std::string message);
std::string    splitString(std::string &line);


#endif
