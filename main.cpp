/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 17:03:35 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/01/17 17:34:26 by ayael-ou         ###   ########.fr       */
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


int main(int argc, char **argv)
{
    if (argc != 3){
        std::cout << "Error of params in you input. Port and mdp !!!" << std::endl;
        return 1;
    }
    serveur prin(argv[1], argv[2]);
}