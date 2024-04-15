# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/15 12:27:52 by atoof             #+#    #+#              #
#    Updated: 2024/04/15 12:27:52 by atoof            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_irc
SRC_DIR = src
OBJ_DIR = obj/
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17
RM = rm -rf
HEADERS := $(shell find $(SRC_DIR) -type f -name "*.h")
SRCS := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
