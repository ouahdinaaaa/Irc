/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 13:03:51 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/29 12:12:33 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Irc.hpp"
#include "../include/Client.hpp"

void    Client::getInvitation(std::string &channel)
{
    std::vector<std::string>::iterator it = std::find(this->_Invitation.begin(), this->_Invitation.end(), channel);
    if (it != this->_Invitation.end())
        return ;
    else
        this->_Invitation.push_back(channel);
}

void    Client::Newlist(std::string channel)
{
    std::vector<std::string> Newlist;
    for (std::vector<std::string>::iterator it = this->_Invitation.begin(); it != this->_Invitation.end(); it++)
    {
        if (*it != channel)
            Newlist.push_back(*it);
    }
    this->_Invitation = Newlist;
    if (Newlist.size() < 1)
        this->_Invitation = std::vector<std::string> ();
}

int    Client::VerifInvitation(std::string channel)
{
    std::vector<std::string>::iterator it = std::find(this->_Invitation.begin(), this->_Invitation.end(), channel);
    if (it != this->_Invitation.end())
        return 1;
    else
        return (0);
}