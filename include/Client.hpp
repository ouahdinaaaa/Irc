/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 16:42:46 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/20 20:19:35 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Irc.hpp"
#include "algorithm"

class Client
{
    private:
        std::string _user;
        std::string _name;
        int         _socketFd;

    public:
    Client(const std::string &name, int socket) : _user(name), _socketFd(socket) {};
    ~Client() {};

    // gerer le client

    std::string     get_user() { return this->_user; };
    std::string     get_name() { return this->_name; };
    void            set_user(std::string &name) { this->_name = name; };
    void            disconnect();
    bool            operator==(const Client &other) const {return this->_socketFd == other._socketFd; };
    int             get_socket() { return this->_socketFd;};
};  


#endif