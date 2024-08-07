/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Serveur.cpp										:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: ayael-ou <ayael-ou@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2024/01/12 11:23:33 by ayael-ou		  #+#	#+#			 */
/*   Updated: 2024/06/25 19:28:01 by ayael-ou		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "../include/serveur.hpp"
#include "algorithm"


int	serveur::FirstParam()
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket == -1)
		return -1;
	int option = 1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
		throw std::runtime_error("Setsockopt failed");
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(this->_port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->_socket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		close(this->_socket);
		return -1;
	}
	if (listen(this->_socket, SOMAXCONN) == -1) {
		close(this->_socket);
		return -1;
	}
	this->_epollFd = epoll_create1(0);
	if (this->_epollFd == -1) {
		close(this->_socket);
		return -1;
	}
	this->_event.events = EPOLLIN;
	this->_event.data.fd = this->_socket;

	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_socket, &this->_event) == -1) {
		close(this->_socket);
		close(this->_epollFd);
		return -1;
	}
	connexion(this->_epollFd);
	close(this->_epollFd);
	close(this->_socket);
	return 0;
}

serveur::~serveur()
{ 
	this->_client.clear();
	this->_channel.clear();
}




void setNonBlocking(int sockfd) {
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

void	serveur::connexion(int epollFd)
{
	char buffer[1024];
	int ret;
	std::vector<int> welcomeSocket;
	this->_events = new epoll_event[MAX_EVENTS];
	this->_event.events = EPOLLIN; 
	int clientSocket;
	CreateBot(epollFd, this->_event);
	while (1)
	{
    	serveur::instance = this;
    	signal(SIGINT, serveur::sig_ctrl_c);
		this->numEvents = epoll_wait(this->_epollFd, this->_events, MAX_EVENTS, -1);
		if (this->numEvents == -1){
			EveryDelete(this->_epollFd, this->_events, this->_event);
			close(this->_epollFd);
			close(this->_socket);
			delete[]this->_events;
			return ;
		}
		for (int i = 0; i < this->numEvents; ++i) {
			if (this->_events[i].data.fd == this->_socket)
			{
				sockaddr_in clientAddr;
				socklen_t clientAddrLen = sizeof(clientAddr);
				clientSocket = accept(this->_socket, (sockaddr*)&clientAddr, &clientAddrLen);
				setNonBlocking(clientSocket);
				this->_event.events = EPOLLIN;
				this->_event.data.fd = clientSocket;
				epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, clientSocket, &this->_event);
			}
			else
			{
				ret = recv(this->_events[i].data.fd, buffer, sizeof(buffer), 0);
				if (ret == -1){
					return ;
				}
				if (ret == 0){
						DeleteChan(this->_events[i].data.fd);
						Delete(this->_events[i].data.fd);
						close(this->_events[i].data.fd);
				}
				buffer[ret] = '\0';
				retrieve_cmd(ret, buffer, this->_event, this->_events, i, this->_epollFd);
			}
		}
	}
	delete []this->_events;
}

void	serveur::Use(std::string command, int socket, epoll_event event, int epollFd)
{
	std::string Chan; 
	std::string newCmd;

	std::vector<std::string> spliit;
	int size = command.find(' ');
	int len_size;
	newCmd = command.substr(0, size);
	Doublons(socket);
	if (newCmd == "CAP" || newCmd == "WHO")
		return ;
	if (verifOP(socket, newCmd))
		return ;
	if (newCmd == "NICK"){
		Chan = command.substr(size + 1, command.length());
		if (UserExist(Chan) && this->_ret[socket] == 1)
			return (this->_ret[socket] = 2, SendRPL(socket, ERR_NICKNAMEINUSE(Chan)));
		else if (this->_ret[socket] == 2 ||  this->_ret[socket] == 0){
			for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); it++) {
    			if (socket == (*it).get_socket()){
        			Client user(Chan, (*it).get_name(), socket);
					(*it).set_name(Chan);
					if (this->_ret[socket] == 2){
        				std::string msp = RPL_WELCOME(Chan);
						Client user2 = getUser(socket);
        				return (SendRPL(socket, msp)); 
					}
  				}
			}
		} if (Chan.length() > 8)
			return ;			
		this->_nick[socket] = Chan;
		this->_ret[socket] = 0;
		return ;
	} else if (newCmd == "PING")
		return(SendRPL(socket, RPL_PONG));
	else if (newCmd == "PASS"){
		this->_ret[socket] = 1;
		Chan = command.substr(size + 1, command.length());
		this->_mdpPort[socket] = Chan;
		return ;
	} else if (newCmd == "JOIN"){
		size = command.find('#') + 1;
		int len = command.find(' ', size);
		int key = 0;
		std::string keys;
		if (len == -1)
			Chan = command.substr(size, command.length());
		else {
			len = command.length() - size;
			Chan = command.substr(size, len);
			if ((int)Chan.find(' ') != -1) {
				std::istringstream  iss(Chan);
				iss >> Chan; 
				}
		} if ((int)command.find(' ', size) != -1) {
			keys = command.substr((int)command.find(' ', size), command.length());
			key = atoi(keys.c_str());
		}
		JoinCommand(Chan, getUser(socket), socket, key);
		return ;
	} else if (newCmd == "USER") {
		std::string msg;
		this->_NickName = 0;
		size_t pos = command.find(' ', size + 1);
		Chan = command.substr(size + 1, pos - (size + 1));
		if (this->_ret[socket] != 0 && this->_ret[socket] != 2)
			return ;
		Client user(this->_nick[socket], Chan, socket);
		(*this)._client.push_back(user);
		if (this->_ret[socket] == 2)
			return ;
		if (!this->_ret[socket])
			msg = RPL_WELCOME(user.get_user());
		if (this->_mdp != this->_mdpPort[socket])
			msg = ERR_PASSWDMISMATCH(Chan);
		return(SendRPL(socket, msg));
	} else if (newCmd == "PART") {
		size = command.find(':');
		if (size != -1) {
			int len = size - command.find('#') - 2;
			std::string channel = command.substr((command.find('#') + 1), len);
			Chan = command.substr(size, command.length());
			if ((int)channel.find(' ') != -1) {
				std::istringstream  iss(channel);
				iss >> channel; 
				}
			PartCommand(channel, socket, Chan);
		} else  {
			size = command.find('#') + 1;
			int len = command.length() - size;
			std::string channels = command.substr(size, len);
			if ((int)channels.find(' ') != -1) {
				std::istringstream  iss(channels);
				iss >> channels; 
				}
			PartCommand(channels, socket, "Good Bye");
		}
		return ;
	}
	len_size = command.find('#');
	if (newCmd == "PRIVMSG" && len_size != -1)  {
		Client user = getUser(socket);
		size = command.find(':');
		if (size == -1)
		{
			Client  clientin = getUser(socket);
			std::string message = ERR_NEEDMOREPARAMS(clientin.get_user(), "PRIVMSG");
			return (SendRPL(socket, message));
		}
		int len = size - command.find('#') - 2;
		std::string Channel = command.substr((command.find('#') + 1), len);
		Chan = command.substr(size, command.length());
		PrivMsg(Channel, Chan, socket);
		return ;
	} else if (newCmd == "PRIVMSG" && len_size == -1)  {
		int len;
		int len2 = 0;
		size = command.find(':') + 1;
			if (size <= ((int)command.find(' ', 8)))
				size++;
			if ((int)command.find(' ', 8) - 7 < -7)
				len2++;
		len = size - command.find(' ', 7) - 3 + len2;
		std::string message = command.substr((command.find(' ', 7) + 1), len);
		Chan = command.substr(size, command.length());
		PrivMsgClient(message, Chan, socket);
		return ;
	} else if (newCmd == "KICK")  {
		size = command.find(':') + 1;
		int len = command.find(':', command.find('#') + 1) - command.find('#') - 2 ;
		std::string message = command.substr(command.find('#') + 1, len);
		Chan = command.substr(size, command.length());
		KickUser(message, Chan, socket);
		return ;
	} else if (newCmd == "INVITE") {
		size = command.find('#') + 1;
		int len = size - command.find(' ', 6) - 3;
		std::string user = command.substr(command.find(' ', 6) + 1, len);
		Chan = command.substr(size, command.length());
		Invite(user, Chan, socket);
		return ;
	} else if (newCmd == "TOPIC"){
		size = command.find('#') + 1;
		int len = command.find(':') - size - 1;
		std::string channel = command.substr(size, len);
		std::string topic = command.substr(command.find(':') + 1, command.length());
		Topic(channel, topic, socket);
		return ;
	} else if (newCmd == "NAMES")   {
		size = command.find('#') + 1;
		std::string channel = command.substr(size, command.length());
		Names(channel, socket);
		return ;
	} else if (newCmd == "WHOIS")  {
		Whois(socket);
		return ;
	} else if (newCmd == "MODE") {
		size = command.find('#') + 1;
		if (!size) {
			int len = command.find(' ', 5) - 5;
			std::string user = command.substr(5, len);
			std::string mode = command.substr(command.find(' ', 5) + 1, command.length());					
			ConfigModeClient(user, mode, socket);
			int socketretriev = RetrieveSocket(user);
			if (socketretriev < 0)
				return (SendRPL(socket, ERR_NOSUCHNICK(user)));
			Client users = getUser(socketretriev);
		} else {
			int debut;
			int len = command.find(' ', size) - size;
			debut = len + 1 + size;
			if (len < 0)
				len = command.length() - size;
			std::string channel = command.substr(size, len);
			std::string mode = command.substr(debut, command.length());
			if (debut == 0)
				mode = "";
			ConfigMode(channel, mode, socket);
		}   return ;
	} else if (newCmd == "QUIT") {
		DeleteChan(socket);
		Delete(socket);
		epoll_ctl(epollFd,  EPOLL_CTL_DEL, socket, &event);
		close(socket);
		return ;
	}  else  {
		if (!ValidUser(socket))
			return ;
		Client _user = getUser(socket);
		std::string msg = ERR_UNKNOWNCOMMAND(_user.get_user(), newCmd);
		SendRPL(socket, msg);
	}
}

void	serveur::ServeurPrint()
{
	for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); it++)
	{
		std::cout << "Name : [" << (*it).get_user() << "] ||| socket : [" << (*it).get_socket() << "]" << std::endl;
	}
}

void	Channel::ChannelPrint()
{
	for (std::vector<Client>::iterator it = this->_list.begin(); it != this->_list.end(); it++)
	{
		std::cout << " Channel List :\nName : [" << (*it).get_user() << "] ||| socket : [" << (*it).get_socket() << "]" << std::endl;
	}
}
   

void	serveur::retrieve_cmd(int ret, char *buffer, epoll_event event, epoll_event* events, int i, int epollFd)
{ 
	std::string string = buffer;
	std::string command = "";
	int j = 0;
	int size;
	int count = -1;
	if (((int)string.find('\n') == -1)) {
		if ((int)string.find('\r') == -1)
			this->_commands[events[i].data.fd] += string;
	}  else {
		if ((int)string.find('\r') == -1){
			this->_commands[events[i].data.fd] += string.substr(0, string.length() -1) + "\r\n";
		} else {
			this->_commands[events[i].data.fd] += string + "\r\n"; }
	}
	if (ret > 0 && ((int)this->_commands[events[i].data.fd].find('\n') != -1) ){
		while (j < (int)this->_commands[events[i].data.fd].length()) {
			size = this->_commands[events[i].data.fd].find('\n', j);
			if (size == -1)
				size = this->_commands[events[i].data.fd].find('\r', j) - 1;
			count = size - j - 1;
			command = this->_commands[events[i].data.fd].substr(j, count);
			if (command.length() > 2)
				Use(command, events[i].data.fd, event, epollFd);
			command = "";
			j = size + 1;
		}
	this->_commands[events[i].data.fd] = "";
	}
}

void	Channel::ChangeClient(std::string mode, std::string name, int socket)
{
	std::stringstream iss(mode);
	std::string word;
	std::string words;

	iss >> word;
	iss >> words;
	
		if (words.length() < 1) {
			std::string message = ERR_NEEDMOREPARAMS(name, "MODE");
			SendRPL(socket, message);
		} for (std::vector<Client>::iterator it = _list.begin(); it != _list.end(); it++) {
			if (words == (*it).get_user())
				(*it).SetOperator(1);
		}
}

void	serveur::Doublons(int socket)
{
	int count = 0;
	for ( std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); it++) {
		if (count == 1 && (*it).get_socket() == socket)
			Delete(socket);
		else if ((*it).get_socket() == socket)
			count++;
	}  
}


int	serveur::verifOP(int socker, std::string command)
{
	for (std::vector<Client>::iterator it = this->_client.begin(); it != this->_client.end(); it++) {
		if (socker == (*it).get_socket() || command == "USER" || command == "CAP" || command == "PASS" || command == "NICK")
			return 0;
	} for (std::vector<Client>::iterator its = this->_client.begin(); its != this->_client.end(); its++) {
		if (socker == (*its).get_socket() && ((*its).get_name().length() < 1 && (command != "USER")))
			return (Delete(socker), 1);
	}
	return 1;
}


serveur::serveur(char *port, char *mdp) : numEvents(0),_mdp(mdp), _mdpPort(), _port(atoi(port)), _channel(), _client(), _ret(), _NickName(0), _commands()
{
    FirstParam(); 
}


serveur* serveur::instance = NULL;

void global_sig_handler(int sig) {
    if (serveur::instance) {
        serveur::instance->sig_ctrl_c(sig);
    }
}


void serveur::handle_sigint(int sig) {
    if (sig == SIGINT) {
        if (instance->numEvents > 0) {
		    for (int i = 0; i < this->numEvents; ++i)
    		{
        		Delete(this->_events[i].data.fd);
        		epoll_ctl(this->_epollFd,  EPOLL_CTL_DEL, this->_events[i].data.fd, &this->_event);
        		close(this->_events[i].data.fd);
        		close(this->_epollFd);
        		close(this->_socket);
    		}
        }
        delete[] instance->_events;
        close(instance->_epollFd);
        close(instance->_socket);
		instance->~serveur();
        exit(0);
    }
}



void serveur::sig_ctrl_c(int sig) {
    if (instance) {
        instance->handle_sigint(sig);
		exit(0);
    }
}


/*
			COMMENT ENVOYER ET RECEVOIR FICHIER

			- /dcc send <nickname> <filename> filename : nom du fichier
			- recoit le fichier
*/