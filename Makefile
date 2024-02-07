# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayael-ou <ayael-ou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/05 17:03:19 by ayael-ou          #+#    #+#              #
#    Updated: 2024/02/07 11:07:12 by ayael-ou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = IrcServ

SRC_DIR = ./
SRCS = 	main.cpp\
		Srcs/Channel.cpp\
		./Srcs/Serveur.cpp\
		./Srcs/Client.cpp\
		./Srcs/Config_Channel.cpp\
		./Srcs/Delete.cpp\
		./Srcs/SendRPL.cpp\
		./Srcs/PrivMsg.cpp\
		./Srcs/outils.cpp\
		./Srcs/command.cpp\
		./Srcs/Bot.cpp\

OBJS = ${SRCS:.cpp=.o}
CC = c++
RM = rm -rf

%.o: %.cpp
	${CC} -c $< -o $@ -std=c++98 -Wall -Wextra -Werror -g3

${NAME}: ${OBJS}
	${CC} ${OBJS} -o ${NAME}

all : ${NAME}

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all

.PHONY : all clean fclean re