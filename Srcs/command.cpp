/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:25:03 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/06/11 13:12:16 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"

void    serveur::JoinCommand(const std::string &channelName, Client userName, int socket, int key)
{
    std::string message;
    std::string Namely;
    std::string message2;
    Channel Name(channelName);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), Name);
    if (it != this->_channel.end()) {
        if (it->Getmode_i() || (it->Getmode_l() && it->Getmode_l() == (int)(it->get_client()).size()) || it->Getmode_k()) 
        {
            if (it->Getmode_i() && !userName.VerifInvitation(channelName) && !userName.GetOperator())
                return (SendRPL(socket, ERR_INVITEONLYCHAN(userName.get_user(), channelName)));
            if (it->Getmode_l() && it->Getmode_l() == (int)(it->get_client()).size() && !userName.GetOperator())
                return(SendRPL(socket, ERR_CHANNELISFULL(userName.get_user(), channelName)));
            if (it->Getmode_k() && it->Getmode_k() != key && !userName.GetOperator())
                return (SendRPL(socket, ERR_BADCHANNELKEY(userName.get_user(), channelName)));
            else  {
                std::vector<Client>::iterator its = std::find(this->_client.begin(), this->_client.end(), userName);
                its->Newlist(channelName);
            }
        }
        Namely = RPL_NAMREPLY(userName.get_user(), channelName, (*it).ListClient());
        std::string EndName = RPL_ENDOFNAMES(userName.get_user(), channelName);
        it->Add(userName);
        message = RPL_JOIN(userName.get_user(), userName.get_name(), channelName);
        Name = *it;
        SendRPL(userName.get_socket(), message);
        if ((*it).GetTopic().length() > 0) {
            SendRPL(socket, RPL_TOPIC(userName.get_user(), channelName, (*it).GetTopic()));
            SendMsg(Name, RPL_TOPIC(userName.get_user(), channelName, (*it).GetTopic()), socket, 1);
        }
        SendRPL(userName.get_socket(), Namely);
        SendRPL(userName.get_socket(), EndName);
        SendMsg(Name, message, userName.get_socket(), 1);
    }
    else {
        userName.SetImunite();
        userName.SetOperator(1);
        Channel NewChan(channelName);
        NewChan.Add(userName);
        this->_channel.push_back(NewChan);
        Name = NewChan;
        Namely = RPL_NAMREPLY(userName.get_user(), channelName, "");
        message = RPL_JOIN(userName.get_user(), userName.get_user(), channelName);
        std::string EndName = RPL_ENDOFNAMES(userName.get_user(), channelName);
        SendRPL(userName.get_socket(), message);
        SendRPL(userName.get_socket(), Namely);
        SendRPL(userName.get_socket(), EndName);
        SendMsg(Name, message, socket, 1);
    }
    return ; 
}

void    serveur::PartCommand(std::string &channel, int socket, std::string reason) 
{
    std::string nameChannel = channel;
    Channel name(channel);
    Client  user = getUser(socket);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), name);
    if (it != this->_channel.end()){
        std::vector<Client> Chan = it->get_client();
        std::vector<Client>::iterator its = std::find(Chan.begin(), Chan.end(), user);
        if (its != Chan.end()) {
            std::string msg = PART_CHANEL((*its).get_user(), (*its).get_name(), "PART", nameChannel);
            std::string message = PART((*its).get_user(), (*its).get_name(), (*it).getname(), reason);
            it->NewList(*its);
            SendRPL(socket, message);
            SendMsg(*it, msg, user.get_socket(), 1);
        }
        else {
            std::string message = ERR_NOTONCHANNEL(user.get_user(), channel);
            SendRPL(socket, message);
        }
    }
    else  {
        std::string message = ERR_NOSUCHCHANNEL(user.get_name(), channel);
        SendRPL(socket, message);
    }
}

void    serveur::Invite(std::string &user, std::string &channel, int socket)
{
    std::vector<Channel>::iterator iti = std::find(this->_channel.begin(), this->_channel.end(), channel);
    Client _user = getUser(socket);
    std::string name = _user.get_user();
    Client _user2 = iti->get_client_name(name);
    int newsocket = RetrieveSocketChan(channel, name); // verifier que lutilisateur est bien present dans le channel
    if (newsocket < 0) {
        std::string message = ERR_NOTONCHANNEL(name, channel);
        SendRPL(socket, message);
        return ;
    }
    int newsocket2 = RetrieveSocketChan(channel, user); // verifier que le user ne fais pas partie du chan
    if (newsocket2 >= 0) {
        std::string message = ERR_USERONCHANNEL(user, channel);
        SendRPL(socket, message);
        return ;
    }
    int targetsock = RetrieveSocket(user);
    if (targetsock < 0)
        return(SendRPL(socket, ERR_NOSUCHNICK(name))); //penser a envoyer msg derreur user dosent existed
    if (!_user2.GetOperator())
        return (SendRPL(socket, ERR_NOPRIVILEGES(name, channel)));
    Client UserInvite = getUser(targetsock);
    std::vector<Client>::iterator it = std::find(this->_client.begin(), this->_client.end(), UserInvite);
    it->getInvitation(channel);
    std::string message = INVITE_CLIENT(name, _user.get_name(), "INVITE", user, channel);
    SendRPL(targetsock, message);
}

void    serveur::KickUser(std::string &channel, std::string &reason, int socket)
{
    std::string Chan = channel.substr(0, channel.find(' '));
    Channel name(Chan);
    std::string user = channel.substr(channel.find(' ') + 1, channel.length());
    // std::cout << "name : [" << user << "]  |||| Channel : [" << Chan << "]" << std::endl;
    std::string message;
    int newsocket = RetrieveSocketChan(Chan, user);
    if (newsocket < 0){
        if (newsocket == -2)
            message = ERR_NOTONCHANNEL(user, Chan);
        else if (newsocket == -1)
            message = ERR_NOSUCHCHANNEL(user, Chan);
        SendRPL(socket, message);
    }
    else
    {
        std::vector<Channel>::iterator iti = std::find(this->_channel.begin(), this->_channel.end(), Chan);
        Client userKick = getUser(newsocket);
        Client userKick2 = iti->get_client_name(userKick.get_user());
        Client _user = getUser(socket);
        Client _user2 = iti->get_client_name(_user.get_user());
        if (userKick2.GetImunite() || !_user2.GetOperator()){
            message = ERR_NOPRIVILEGES(_user.get_user(), channel);
            return (SendRPL(socket, message));
        }
        message = RPL_KICK(_user.get_user(), Chan, user, reason);
        std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), name);
        std::vector<Client> list = (*it).get_client();
        std::vector<Client>::iterator its = std::find(list.begin(), list.end(), userKick);
        if (it != this->_channel.end() && its != list.end()) {
            SendRPL(newsocket, message);
            it->NewList(*its);
        }
    }
}

void    serveur::Whois(int socket)
{
    (void)socket;
    // Client _user = getUser(socket);
    // std::string message;

    // message = RPL_WHOISUSER(_user.get_user(), "", _user.get_name());
    // std::cout << "message Whois [" << message << "]" << std::endl; 
    // SendRPL(socket, message);
}