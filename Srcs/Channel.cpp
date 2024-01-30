/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 19:42:15 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/30 12:29:19 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"

void Channel::Banned(const std::string &user)
{
    (void)user;
}

Channel::Channel(const Channel &other)
{
    *this = other;
}

void    Channel::Add(Client user)
{
    this->_list.push_back(user);
}

void    Channel::NewList(Client user)
{
    std::vector<Client> newlist;
    for (std::vector<Client>::iterator it = this->_list.begin(); it != this->_list.end(); ++it)
    {
        if (user.get_user() != (*it).get_user())
            newlist.push_back(*it);
    }
    this->_list = newlist;
    if (newlist.size() < 1)
        this->_list = std::vector<Client>();
        
}

void    Channel::choice_mode(std::string mode, Client name, std::string channel, int socket)
{
    int size = mode.find(' ');
    int limit = 0;
    std::string argument;
    if (size != -1)
    {
        argument = mode.substr(size + 1, mode.length());
        limit = atoi(argument.c_str());
        if (limit <= 0 && (mode[1] == 'l'))
            return (SendRPL(socket, ERR_INVALIDLIMIT(name.get_user(), channel)));
        else if (limit <= 0 && (mode[1] == 'k'))
            return (SendRPL(socket, ERR_INVALIDKEY(name.get_user(), channel)));
    }
    std::string Name = name.get_user();
    std::string User = name.get_name();
    std::string message;
    if (mode[0] != '+' && mode[0] != '-')
        return ;
    switch (mode[1])
    {
        case 'i':
        {
            if (mode[0] == '+')
                this->_mode_i = 1;
            else
                this->_mode_i = 0;
            this->_change = 'i';
            break;
        }
        case 't':
        {
            if (mode[0] == '+')
                this->_mode_t = 1;
            else
                this->_mode_t = 0;
            this->_change = 't';
            break;
        }
        case 'k':
        {
            if (mode[0] == '+' && size)
                this->_mode_k = limit;
            else if (mode[0] == '+' && size == -1)
                return (SendRPL(socket, ERR_INVALIDKEY(name.get_user(), channel)));
            else
                this->_mode_k = 0;
            this->_change = 'k';
            break ;
        }
        case 'o':
        {
            if (mode[0] == '+')
                this->_mode_o = 1;
            else
                this->_mode_o = 0;
            this->_change = 'o';
            break;
        }
        case 'l':
        {
            if (mode[0] == '+' && size == -1)
                this->_mode_l = 5;
            else if (mode[0] == '+' && size)
                this->_mode_l = limit;
            else
                this->_mode_l = 0;
            this->_change = 'l';
            break ;
        }
        default:
        return ;
    }
    if (mode[0] == '+')
        message = SET_CHANEL_MODE(Name, User, "MODE", channel, mode[1]);
    else
        message = UNSET_CHANEL_MODE(Name, User, "MODE", channel, mode[1]);
    SendRPL(socket, message);
}

std::string Channel::ListClient()
{
    std::string list = "";
    if (this->_list.size() < 1)
        return (list);
    for (std::vector<Client>::iterator it = this->_list.begin(); it != this->_list.end(); ++it) {
        list += it->get_user();
        list += " ";
    }
    return list;
}

Channel &Channel::operator=(const Channel &other)
{
    this->_name = other._name;
    this->_Banned = other._Banned;
    this->_list = other._list;
    return (*this);
}

