/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 17:03:35 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/02/07 12:03:11 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include "./include/Irc.hpp"
#include "./include/serveur.hpp"
#include <sys/socket.h>

int ctrl_c;

int main(int argc, char **argv)
{
    srand(time(NULL));
    if (argc != 3){
        std::cout << "Error of params in you input. Port and mdp !!!" << std::endl;
        return 1;
    }
    serveur prin(argv[1], argv[2]);
}

// ./IrcServ 6668 mdp executable with port | password
// irssi -c localhost -p 6668 -w mdp -n kello // lancer IRssi client qui vas se co a ton serveur

