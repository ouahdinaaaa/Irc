/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 19:42:15 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/12 18:29:16 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

void        Channel::send_message(const std::string &name)
{
    (void)name;
    // (void)
}

void Channel::Banned(const std::string &user)
{
    (void)user;
}

Channel::Channel(const Channel &other)
{
    *this = other;
}

void    Channel::Add(const std::string &user)
{
    this->_list.push_back(user);
}

std::string &Channel::getname()
{
    return this->_name;
}

bool    Channel::operator==(const Channel &other) const
{
    return (this->_name == other._name);
}

Channel &Channel::operator=(const Channel &other)
{
    this->_name = other._name;
    this->_Banned = other._Banned;
    this->_list = other._list;
    return (*this);
}