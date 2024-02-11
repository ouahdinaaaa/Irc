/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 16:42:46 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/10 16:05:30 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Irc.hpp"
#include "algorithm"

class Client
{
    private:
        std::string              _user;
        std::string              _name;
        int                      _socketFd;
        int                      _operator;
        int                      _imunite;
        std::vector<std::string> _Invitation;


    public:
    Client(const std::string &name, int socket) : _user(name), _socketFd(socket), _operator(0), _imunite(0), _Invitation() {};
     Client(const std::string &name, std::string user, int socket) : _user(name), _name(user),_socketFd(socket), _operator(0), _imunite(0), _Invitation() {};
    Client(const std::string &name, int socket, int imunite) : _user(name), _socketFd(socket), _operator(0), _imunite(imunite), _Invitation() {};
    ~Client() {};
    
    // gerer le client
    
    std::string     get_user() { return (*this)._user; };
    std::string     get_name() { return this->_name; };
    void            getInvitation(std::string &channel);
    void            set_user(std::string &name) { this->_name = name; };
    void            disconnect();
    void            SetImunite() {this->_imunite = 1; };
    void            Newlist(std::string channel);
    int             VerifInvitation(std::string channel);
    int             GetOperator() {return this->_operator; };
    int             GetImunite() {return this->_imunite; };
    void            SetOperator(int boolean) {this->_operator = boolean;};
    bool            operator==(const Client &other) const {return this->_socketFd == other._socketFd; };
    int             get_socket() { return this->_socketFd;};
};  

#endif