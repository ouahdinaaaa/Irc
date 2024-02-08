/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_Channel.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:53:26 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/08 15:28:26 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"


void    serveur::ConfigModeClient(std::string &user, std::string &mode, int socket)
{
    if (mode.size() != 2)
        return ;
    Client userName = getUser(socket);
    if (mode[1] != 'o' && mode[1] != 'i')
        return (SendRPL(socket, ERR_NEEDMOREPARAMS(userName.get_user(), "MODE")));
    int newsocket = RetrieveSocket(user);
    if (newsocket == -1)
        return ;
    Client newUser = getUser(newsocket);
    std::vector<Client>::iterator it = std::find(this->_client.begin(), this->_client.end(), newUser);
    if (it != this->_client.end()) {
        if (mode[0] == '-' && mode[1] == 'o')
            it->SetOperator(0);
        else if (mode[0] == '+' && mode[1] == 'o')
            it->SetOperator(1);
    }
}

void    serveur::ConfigMode(std::string &channel, std::string &mode, int socket)
{
    Channel Name(channel);
    Client _user = getUser(socket);
    std::string name = _user.get_user();
    std::string message;
    int newsocket = RetrieveSocketChan(channel, name);
    if (newsocket < 0) {
        if (newsocket == -1)
            message = ERR_NOSUCHCHANNEL(name, channel);
        else
            message = ERR_NOTONCHANNEL(name, channel);
        return (SendRPL(socket, message));
    }
    if (mode.length() < 2)
        return ;
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Name);
    if (it != this->_channel.end()){
        // std::cout << "Name : [" << _user.get_user() << "] ||| imunite : [" << _user.GetImunite() << "]" << std::endl;
        if (!_user.GetOperator())
            return (SendRPL(socket, ERR_NOPRIVILEGES(name, channel)));
        it->choice_mode(mode, _user, channel, socket);
    }
}


void    serveur::Topic(std::string &channel, std::string &topic, int socket)
{
    Client       _user = getUser(socket);
    std::string  name = _user.get_user();
    std::string  message;
    int newsocket = RetrieveSocketChan(channel, name);
    if (newsocket < 0) {
        if (newsocket == -1)
            message = ERR_NOSUCHCHANNEL(name, channel);
        else
            message = ERR_NOTONCHANNEL(name, channel);
        SendRPL(socket, message);
    } else {
        Channel chan(channel);
        std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), chan);
        if (it->Getmode_t() && !(_user.GetOperator())){
            message = ERR_NOPRIVILEGES(name, channel);
            return (SendRPL(socket, message));
        }
        it->AddOrChangeTopic(topic);
        message = RPL_TOPIC(name, channel, topic);
        SendRPL(socket, message);
        std::string message2 = SET_TOPIC(name, _user.get_name(), "TOPIC", channel, topic);
        SendMsg((*it), message2, socket, 1);
        SendMsg((*it), topic, socket, 0); // Reflechir a modifier le RPL pour dire a modifier to new topic is et voila
    }
}

void    serveur::Names(std::string channel, int socket)
{
    Channel Chan(channel);
    Client  user = getUser(socket);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Chan);
    if (it != this->_channel.end()) {
        std::string msg = RPL_NAMREPLY(user.get_user(), channel, it->ListClient());
        SendRPL(socket, msg);
    }
    else {
        std::string msg = ERR_NOSUCHCHANNEL(user.get_name(), channel);
        SendRPL(socket, msg);
    }
}