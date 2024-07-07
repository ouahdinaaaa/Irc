/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:52:30 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/07/07 12:59:07 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"


void serveur::Delete(int socket)
{
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); )
    {
        if (socket == it->get_socket())
            it = this->_client.erase(it);
        else
            ++it;
    }
}


void    Channel::DeleteChan(int socket)
{
    for (std::vector<Client>::iterator it = this->_list.begin(); it != this->_list.end(); )
    {
        if (it->get_socket() == socket)
            it = this->_list.erase(it);
        else
            ++it;
    }
}

void serveur::DeleteChan(int socket)
{
    for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end();) {
        std::vector<Client> clients = it->get_client(); 
        
        it->DeleteChan(socket);
        if (clients.empty())
            it = _channel.erase(it);
        else
            ++it;
    }
}


void serveur::EveryDelete(int epollFd, struct epoll_event* events, struct epoll_event event)
{ 
    this->numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
    for (int i = 0; i < this->numEvents; ++i)
    {
        DeleteChan(events[i].data.fd);
        Delete(events[i].data.fd);
        epoll_ctl(epollFd,  EPOLL_CTL_DEL, events[i].data.fd, &event);
        close(events[i].data.fd);
        close(epollFd);
        close(this->_socket);
    }
}

