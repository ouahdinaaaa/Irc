/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:06:55 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/15 18:52:18 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Irc.hpp"
#include "Client.hpp"

class Channel
{
    private:
        std::string                     _name;
        std::vector<Client>             _list;
        std::vector<std::string>        _Banned;
    
    public:
    Channel(const std::string &name) : _name(name), _list(), _Banned() {};
    Channel(const Channel &other);
    bool    operator==(const Channel &other) const;
    Channel &operator=(const Channel &other);
    void    send_message(const std::string &name);
    std::vector<Client>  get_client() { return this->_list; };
    void    Banned(const std::string &user);
    // void    DisplayUser();
    std::string &getname() {return this->_name; };
    void    Add(Client user);
    // void    Delete(const std::string &user);
    ~Channel() {};
};


#endif