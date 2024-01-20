/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:06:55 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/20 18:25:53 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Irc.hpp"
#include "Client.hpp"
#include "serveur.hpp"

class Channel
{
    private:
        std::string                     _name;
        std::vector<Client>             _list;
        std::vector<std::string>        _Banned;
        // int                             _key;
        // std::string                     _mdp;
    
    public:
    Channel(const Channel &other);
    Channel(const std::string &name) : _name(name), _list(), _Banned() {};
    ~Channel() {};
    
    bool                 operator==(const Channel &other) const { return _name == other._name; };
    void                 Banned(const std::string &user);
    void                 Delete(std::vector<Client>::iterator it);
    void                 Add(Client user);
    Channel              &operator=(const Channel &other);
    std::string          &getname() {return this->_name; };
    std::vector<Client>  get_client() { return this->_list; };
};


#endif