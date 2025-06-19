# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/11 23:47:48 by youbella          #+#    #+#              #
#    Updated: 2025/06/19 12:22:15 by youbella         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = #-Wall -Wextra -Werror
READLINE_INCLUDE = -I$(shell brew --prefix readline)/include
READLINE_LIBRARY = -L$(shell brew --prefix readline)/lib -lreadline

SRC = minishell.c split_first_cmd.c search.c
OBJS = $(SRC:.c=.o)
NAME = minishell

RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
RESET = \033[0m

all: make_libft $(NAME)

make_libft:
	@make -C libft
	@make bonus -C libft

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(READLINE_LIBRARY) $(OBJS) libft/libft.a -o $@
	@echo "$(GREEN)✅ Compilation Completed Successfully!$(RESET)"

%.o: %.c minishell.h
	@echo "$(YELLOW)⚙️  Compiling $<...$(RESET)"
#	@printf "$(BLUE)⚙️  Norminette:$(RESET)$(GREEN) " && norminette $< && echo "$(RESET)"
	@$(CC) $(CFLAGS) $(READLINE_INCLUDE) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@make clean -C libft
	@echo "$(RED)🧹 Object files removed.$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@rm -f libft/libft.a
	@echo "$(RED)🗑️  Program removed.$(RESET)"

re: fclean all

.PHONY: clean
