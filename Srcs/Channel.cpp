/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 19:42:15 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/20 20:27:44 by ayael-ou         ###   ########.fr       */
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

void    Channel::Delete(std::vector<Client>::iterator it)
{
    this->_list.erase(it);
}

Channel &Channel::operator=(const Channel &other)
{
    this->_name = other._name;
    this->_Banned = other._Banned;
    this->_list = other._list;
    return (*this);
}