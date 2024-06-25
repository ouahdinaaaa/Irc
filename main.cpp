/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 17:03:35 by ayael-ou          #+#    #+#             */
/*   Updated: 2024/06/25 19:29:54 by ayael-ou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/Irc.hpp"
#include "./include/serveur.hpp"

bool running = true;

int main(int argc, char **argv)
{
    srand(time(NULL));
    if (argc != 3){
        std::cout << "Error of params in you input. Port and mdp !!!" << std::endl;
        return 1;
    }
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_ctrl_c;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    serveur prin(argv[1], argv[2]);
}


// irssi -c localhost -p 6668 -w mdp -n kello // lancer IRssi client qui vas se co a ton serveur
/*
            Ce qui faut regler
        - Si on quitte le channel et on est operator offrir loperator au suivant du chann
        (donner les droit au derniers ayant rejoin le channel)

        - Completer les modes lorsqu'il ma
        nque les arguments (Renvoyer RPl necessaire)
        - Quand mode est changer envoyer Rpl aux autres User
        - Envoyer RPL pour WHOIS.
        Regarder prblm attribution +o a user

*/
