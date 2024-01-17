            EXPLICATION IRC

                                Fonctionnalite a faire :
-   Vous inscrire avec pseudo ou nom dutilisateur // Fait

-   Pourvoir rejoindre une chanel, envoyer ou recevoir des messages prive //Fait

-   Chaque message envoye par un client dans un canal doit etre recu par tout les cient ayant rejoint ce canal. // Fait

--------------------------------------------------------------------------------------

- Avoir utilisateur et operator normaux. Implementer les commandes specifiques des operateurs.

- Command KICK -Ejecter un client du Channel;

- Command INVITE -inviter un client au Channel;

- Command Topic - Affiche ou modifie le theme du channel

- Mode : change mode du canal
        
        - i : Definir/supprimer le canal sur invitation
        - t : Definir/supprimer les restrciton de la command topic pour operateurs de canaux
        - k : Definir/retirer le privilege de l'operateur de canal
        - o : Donner/retirer le privilege de l'operateur de canal
        - l : Definir/Supprimer la limite d'utilisateur pour le canal.
Bien te renseigner sur comment uiliser la commande MODE


- Pas de fuite et gerer les signaux 

Ctrl + D (msg partiel)
Ctrl + Z (processus suspendu)
Ctrl + C (pas sur mais devra quitter le programme)



                BONUS : 
    Implementer bot + transfert de fichier.

--------------------------------------------------------------------------------------
        - Etape pour bien debuter : 
    
    *Programmation de reseaux : 
comprendre les bases du fonctionnement des reseaux, des sockets, adresses IP, port ainsi que communication entre client et seerveur via socket TCP/IP.
//Fait


    *Manipulation de socket : 
Apprendre a creer configurer, utiliser et fermer des socket pour etablir des connexions entre clien et serveur
//Fait

    Apprendre :
    - creation de socket
    - liaison a une adresse et un port
    - ecoute des connexion entrante (cote serveur) et la connexion a un serveur distant(cote client)
    //Fait

    *Protocole IRC : 
    - Comprendre le protocole IRC (commandes/ message et facon dont les client et serveur interagissent au sein du protocole)
    - Savoir comment les messages sont formates, envoye et interpretes entre client et serveur.
    //Fait

    *GESTIONS DES ENTREE / SORTIE (I/O)
    Apprendre a gerer les entree et sorties pour permettrent au utilisateur d envoyer et recevoir des message dans le client IRC, ainsi que msg entrant sortant sur le serveur.
    //Fait

    *Gestion des erreurs et debogage : 
    Apprendre a gere les erreurs reseaux.
    - Deconnexion inatendue, les problemes de connexion, les messages erronee, etc.
    //Fait

    A LA FIN DERNIERE ETAPE TESTER ET VALIDER LE RENDU 

