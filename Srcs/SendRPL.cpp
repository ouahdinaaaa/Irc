/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendRPL.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:36:49 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/07/07 12:38:28 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"

void    SendRPL(int socket, std::string message) {
    ssize_t size =  send(socket, message.c_str(), message.length(), 0);
    if(size == -1)
        return ;
}

void serveur::SendMsg(Channel &Channel, std::string msg, int socket, int boolean)
{
    Client userName = getUser(socket);
    std::vector<Client>::iterator it;
    std::vector<Client> list = Channel.get_client();
    std::string message;
    if (boolean)
        message = RPL_PRIVMSG_CHANNEL(userName.get_user(), Channel.getname(), msg);
    for (it = list.begin(); it != list.end(); ++it) {
        if ((*it).get_socket() != socket) {
            if (!boolean)
                message = RPL_TOPIC((*it).get_user(), Channel.getname(), msg);
            SendRPL((*it).get_socket(), message);
        }
    }
}
