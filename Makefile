# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/05 17:03:19 by ayael-ou          #+#    #+#              #
#    Updated: 2024/06/25 19:05:38 by ayael-ou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = IrcServ
CXXFLAGS = -Wall -Wextra -Werror -g3 -std=c++98

SRC_DIR = ./
SRCS = 	main.cpp\
		./Srcs/Channel.cpp\
		./Srcs/Serveur.cpp\
		./Srcs/Client.cpp\
		./Srcs/Config_Channel.cpp\
		./Srcs/Delete.cpp\
		./Srcs/SendRPL.cpp\
		./Srcs/PrivMsg.cpp\
		./Srcs/outils.cpp\
		./Srcs/command.cpp\
		./Srcs/Bot.cpp\

OBJS = ${SRCS:%.cpp=%.o}
CXX = c++
RM = rm -rf

%.o: %.cpp
	${CXX} $(CXXFLAGS) -c $< -o $@  

${NAME}: ${OBJS}
	${CXX} $(CXXFLAGS) ${OBJS} -o ${NAME}

all : ${NAME}

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all

# .PHONY : all clean fclean re