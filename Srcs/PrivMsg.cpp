/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivMsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:01:26 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/07 11:02:32 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"

void    serveur::PrivMsg(std::string &channel, std::string &msg, int socket)
{
    Channel name(channel);
    std::cout << "msg : [" << msg << "]" << std::endl;
    if (msg == ":Bot" || msg == ":BOT")
        return (Bot_cmd(socket));
    std::vector<Channel>::iterator  it = std::find(this->_channel.begin(), this->_channel.end(), name);
    if (it != this->_channel.end()) 
        PrivChannel(*it, socket, msg);
}

void    serveur::PrivChannel(Channel &Channel, int socket, std::string &msg)
{
    std::vector<Client> list = Channel.get_client();
    std::vector<Client>::iterator it = list.begin();
    Client userName = getUser(socket);
    std::string message = RPL_PRIVMSG_CHANNEL(userName.get_user(), Channel.getname(), msg);
    for (it = list.begin(); it != list.end(); ++it){
        if ((*it).get_socket() != socket){
            if ((*it).get_socket() != socket)
                SendRPL((*it).get_socket(), message);
        }
    }
}

void    serveur::PrivMsgClient(std::string &name, std::string &message, int socket)
{
    Client user = getUser(socket);
    int newsocket = RetrieveSocket(name);
    std::string msg = RPL_PRIVMSG_CLIENT(user.get_user(), user.get_name(), name, message);
    if (newsocket == -1) {
        std::string msg = ERR_NOSUCHNICK(name);
        return (SendRPL(socket, msg));
    }
    SendRPL(newsocket, msg);
}

void    serveur::PrintClient(Channel &chan)
{
    std::vector<Client> list = chan.get_client();
    for (std::vector<Client>::iterator it = list.begin(); it != list.end(); ++it) {
        std::cout << "name : [" << (*it).get_user() << "]  |||| socket : " << (*it).get_socket() << std::endl;
    }       
}
