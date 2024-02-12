/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   outils.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 10:26:47 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/12 19:04:43 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/serveur.hpp"
#include <csignal>


int serveur::RetrieveSocketChan(std::string &name, std::string &user)
{
    Channel _name(name);
    std::vector<Channel>::iterator it = std::find(this->_channel.begin(), this->_channel.end(), _name);
    if (it != this->_channel.end()) {
        std::vector<Client> cliennt = it->get_client();
        for (std::vector<Client>::iterator its = cliennt.begin(); its != cliennt.end(); ++its) {
            if ((*its).get_user() == user)
                return ((*its).get_socket());
        }
        return -2; // user is not in chan
    }
    return -1; //channel dont exit
}

int     serveur::RetrieveSocket(std::string &name)
{
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it) {
        if ((*it).get_user() == name)
            return (*it).get_socket();
    }
    return -1;
}

Client serveur::getUser(int socket)
{
    std::vector<Client>::iterator it;
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it) {
        if ((*it).get_socket() == socket)
            return *it;
    }
    return *it;
}

void    signal_ctrl_c(int signal)
{
    if (signal == SIGINT) {
        ctrl_c = 1;
    }
}


std::string splitString(std::string &line) 
{
    std::istringstream iss(line);
    std::vector<std::string> words;
    std::string word;
    std::string str;

    while (iss >> word) {
        words.push_back(word);
    }
    for (int i = 0; i < (int)words.size(); i++) {
        if (i + 1 == (int)words.size())
            str += words[i] + '\n';
        else
            str += words[i] + " ";
    }
    return str;
}

int serveur::ValidUser(int socket)
{
    std::vector<Client>::iterator it;
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it) {
        if ((*it).get_socket() == socket)
            return 1;
    }
    return 0;
}


int     serveur::UserExist(std::string &name)
{
    for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); ++it)     {
        if (name == it->get_user()) {
            this->_NickName = 1;
            return 1;
        }
    }
    return 0;
}

