/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:06:55 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/12 18:24:37 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Irc.hpp"

class Channel
{
    private:
        std::string                     _name;
        // std::map<std::string, int>      _list;
        std::vector<std::string>        _list;
        std::vector<std::string>        _Banned;
    
    public:
    Channel(const std::string &name) : _name(name), _list(), _Banned() {};
    Channel(const Channel &other);
    bool    operator==(const Channel &other) const;
    Channel &operator=(const Channel &other);
    void    send_message(const std::string &name);
    void    Banned(const std::string &user);
    // void    DisplayUser();
    std::string &getname();
    void    Add(const std::string &user);
    // void    Delete(const std::string &user);
    ~Channel() {};
};


#endif