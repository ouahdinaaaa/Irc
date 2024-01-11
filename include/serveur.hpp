/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 17:36:06 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/09 15:05:38 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVEUR_HPP
#define SERVEUR_HPP

#include "Irc.hpp"
#include "Client.hpp"

class serveur
{
    private:
        int     _socket;
        std::vector<Client> Client; // creation liste de cliens co

    public:
        serveur();
        ~serveur();
        bool    start(int port);
        void    finish();
        void    connexion(); // gere les connexion entrante
        // void    Conexion_client(Client &client);
};

#endif