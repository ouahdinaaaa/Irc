/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:06:55 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/06/11 12:21:43 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Irc.hpp"
#include "Client.hpp"
#include "serveur.hpp"
#define CRLF "\r\n"


class Channel
{
    private:
        std::string                     _name;
        std::vector<Client>             _list;
        std::string                     _topic;
        std::vector<std::string>        _Banned; //
        std::vector<std::string>        _Inviting; //
        int                             _mode_i;//Fait
        int                             _mode_t;//Fait 
        int                             _mode_k;//Fait
        int                             _mode_o;//Fait mais reste a bien verifier and implementer for user
        int                             _mode_l;//Fait
        std::string                            _changel;
        std::string                            _change;
        std::string                            _changek;
        std::string                            _changet;
        std::string                            _changei;
        int                                     send_msg;
        // int                             _key;
        // std::string                     _mdp;
    
    public:
    Channel(const Channel &other);
    // Channel(const std::string &name) : _name(name), _list(), _Banned(), _mode_i(0) {};
    Channel(const std::string &name) : _name(name), _Banned(), _Inviting(), _mode_i(0), _mode_t(0), _mode_k(0), _mode_o(0), _mode_l(0), _change("0"), send_msg(0) {
        this->_list = std::vector<Client>();
    };
    ~Channel() {};
    
    bool                 operator==(const Channel &other) const { return _name == other._name; };
    void                 Banned(const std::string &user);
    void                 AddOrChangeTopic(std::string &topic) {this->_topic = topic; };
    void                 Add(Client user);
    void                 NewList(Client user);
    // void                 AddInviting(std::string &user);
    int                  Getmode_i() {return this->_mode_i; };
    int                  Getmode_t() {return this->_mode_t; };
    int                  Getmode_k() {return this->_mode_k; };
    int                  Getmode_o() {return this->_mode_o; };
    int                  Getmode_l() {return this->_mode_l; };
    std::string          GetChange() {return this->_change; };
    void                 SetMode_i(int boolean) {this->_mode_i = boolean; };
    void                 SetMode_t(int boolean) {this->_mode_t = boolean; };
    void                 SetMode_k(int boolean) {this->_mode_k = boolean; };
    void                 SetMode_o(int boolean) {this->_mode_o = boolean; };
    void                 SetMode_l(int boolean) {this->_mode_l = boolean; };
    Client               get_client_name(std::string name);
    void                 ChangeClient(std::string mode, std::string name, int socket);
    void                 choice_mode(std::string mode, Client name, std::string channel, int socket);
    Channel              &operator=(const Channel &other);
    std::string          ListClient();
    int                  getmsg() {return this->send_msg;};
    std::string          &getname() {return this->_name; };
    std::string          GetTopic() {return this->_topic; };
    std::vector<Client>  get_client() {return this->_list;};
    void                 verifSend(std::string &mode);
    void                 ChangeList(std::vector<Client> Newlist) {this->_list = Newlist;};
};

std::string create_header();


#endif