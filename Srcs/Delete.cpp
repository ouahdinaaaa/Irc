/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:52:30 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/07/06 18:00:22 by ayael-ou         ###   ########.fr       */
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

void    serveur::DeleteServ(int socket)
{
    Client _user = getUser(socket);
    std::vector<Client> Newlist;
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); it++)
    {
        if (socket != it->get_socket())
            Newlist.push_back(*it);
    }
    if (Newlist.size() < 1)
        this->_client = std::vector<Client> ();
    else
        this->_client = Newlist;
}


void serveur::DeleteChan(int socket)
{
    for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end();)
    {
        std::vector<Client> NewList;
        std::vector<Client> client = (*it).get_client();

        for (std::vector<Client>::iterator its = client.begin(); its != client.end(); its++) {
            if (socket != (*its).get_socket())
                NewList.push_back(*its);
        }
        if (NewList.empty()) {
            it = _channel.erase(it);
        } else {
            it->ChangeList(NewList);
            ++it;
        }
    }
}



void serveur::EveryDelete(int epollFd, struct epoll_event* events, struct epoll_event event)
{ 
    this->numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
    for (int i = 0; i < this->numEvents; ++i)
    {
        Delete(events[i].data.fd);
        epoll_ctl(epollFd,  EPOLL_CTL_DEL, events[i].data.fd, &event);
        close(events[i].data.fd);
        close(epollFd);
        close(this->_socket);
    }
}