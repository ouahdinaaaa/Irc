/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:52:30 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/11 15:14:54 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"


void    serveur::Delete(int socket)
{
    Client _user = getUser(socket);
    for (std::vector<Channel>::iterator it = this->_channel.begin(); it != this->_channel.end(); ++it) {
        it->NewList(_user);
    }
    std::vector<Client> Newlist;
    for (std::vector<Client>::iterator its = this->_client.begin(); its != this->_client.end(); ++its)
    {
        if (_user.get_user() != its->get_user())
            Newlist.push_back(*its);
    }
    this->_client = Newlist;
    if (Newlist.size() < 1)
        this->_client = std::vector<Client> ();
}

void    serveur::EveryDelete(int epollFd, epoll_event *events, epoll_event event)
{
    
    int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
    for (int i = 0; i < numEvents; ++i)
    {
        Delete(events[i].data.fd);
        epoll_ctl(epollFd,  EPOLL_CTL_DEL, events[i].data.fd, &event);
        close(events[i].data.fd);
        close(epollFd);
        close(this->_socket);
    }
}

