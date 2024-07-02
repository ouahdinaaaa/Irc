/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 19:42:15 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/07/02 15:39:46 by ayael-ou         ###   ########.fr       */
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
    if (newlist.size() < 1)
        this->_list = std::vector<Client>();
    this->_list = newlist;
}

Client               Channel::get_client_name(std::string name)
{
    for (std::vector<Client>::iterator it = _list.begin(); it != _list.end(); ++it)
    {
        if ((*it).get_user() == name)
            return (*it);
    }
    return (*_list.begin());
}


void    Channel::choice_mode(std::string mode, Client name, std::string channel, int socket)
{
    int size = mode.find(' ');
    if (mode.length() < 2)
        return ;
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
    // std::cout << "Mode : [" << mode << "] ||| Chan : [" << channel << "]" << std::endl;
    std::string message;
    if (mode[0] != '+' && mode[0] != '-')
        return ;
    verifSend(mode);
    switch (mode[1])
    {
        case 'i':
        {
            if (mode[0] == '+')
                this->_mode_i = 1;
            else if (mode[0] == '-')
                    this->_mode_i = 0;
            break;
        }
        case 't':
        {
            if (mode[0] == '+')
                this->_mode_t = 1;
            else if (mode[0] == '-')
                this->_mode_t = 0;
            break;
        }
        case 'k':
        {
            std::stringstream iss(mode);
            std::string word;
            std::vector<std::string> words;

            iss >> word;
            iss >> word;
            if (mode[0] == '+' && size) {
                this->_mode_k = limit;
            }
            else if (mode[0] == '+' && size == -1)
                return (SendRPL(socket, ERR_INVALIDKEY(name.get_user(), channel)));
            else if (mode[0] == '-')
                    this->_mode_k = 0;
            break ;
        }
        case 'o':
        {
            ChangeClient(mode, Name, socket);
            break;
        }
        case 'l':
        {
            if (mode[0] == '+' && size == -1) 
                this->_mode_l = 5;
            else if (mode[0] == '+' && size)
                this->_mode_l = limit;
            else if (mode[0] == '-')
                this->_mode_l = 0;
            break ;
        }
        default:
        return ;
    }
    if (mode[0] == '+' && getmsg())
        message = SET_CHANEL_MODE(Name, User, "MODE", channel, mode[1]);
    else if (mode [0] == '-' && getmsg())
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


void    Channel::verifSend(std::string &mode)
{

    switch (mode [1])
    {
        case 'i':
        {
            if (Getmode_i() && mode[0] == '+')
                this->send_msg = 0;
            else if (!Getmode_i() && mode[0] == '-')
                this->send_msg = 0;
            else 
                this->send_msg = 1;
            break;
        }
        case 't':
        {
            if (Getmode_t() && mode[0] == '+')
                this->send_msg = 0;
            else if (!Getmode_t() && mode[0] == '-')
                this->send_msg = 0;
            else 
                this->send_msg = 1;
            break;

        }
        case 'k':
        {
            if (Getmode_k() && mode[0] == '+')
                this->send_msg = 0;
            else if (!Getmode_k() && mode[0] == '-')
                this->send_msg = 0;
            else 
                this->send_msg = 1;
            break;

        }
        case 'l':
        {
            if (Getmode_l() && mode[0] == '+')
                this->send_msg = 0;
            else if (!Getmode_l() && mode[0] == '-')
                this->send_msg = 0;
            else 
                this->send_msg = 1;
            break;

        }
        case 'o':
        {
            if (Getmode_o() && mode[0] == '+')
                this->send_msg = 0;
            else if (!Getmode_o() && mode[0] == '-')
                this->send_msg = 0;
            else 
                this->send_msg = 1;
            break;

        }
        default:
        return ;
    }
}

Channel &Channel::operator=(const Channel &other)
{
    this->_name = other._name;
    this->_Banned = other._Banned;
    this->_list = other._list;
    this->_mode_i = other._mode_i;
    this->_mode_t = other._mode_t;
    this->_mode_k = other._mode_k;
    this->_mode_o = other._mode_o;
    this->_mode_l = other._mode_l;
    return (*this);
}


std::string create_header()
{
        std::string header = \
    CRLF \
    "  ██╗██████╗  ██████╗    ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗"  CRLF \
    "  ██║██╔══██╗██╔════╝    ██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗" CRLF \
    "  ██║██████╔╝██║         ███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝" CRLF \
    "  ██║██╔══██╗██║         ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗" CRLF \
    "  ██║██║  ██║╚██████╗    ███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║" CRLF \
    "  ╚═╝╚═╝  ╚═╝ ╚═════╝    ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\x1b[0m" CRLF;
    return header;
}