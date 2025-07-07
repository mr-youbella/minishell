# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/11 23:47:48 by youbella          #+#    #+#              #
#    Updated: 2025/07/04 17:05:50 by youbella         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror
READLINE_INCLUDE = -I$(shell brew --prefix readline)/include
READLINE_LIBRARY = -L$(shell brew --prefix readline)/lib -lreadline

SRC = minishell.c split_first_cmd.c search.c redirections.c get_next_line_bonus.c
OBJS = $(SRC:.c=.o)
NAME = minishell

RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
CYAN = \033[0;36m
DEF = \033[0m

all: make_libft print_compiler_minishell $(NAME)
	@echo "$(GREEN)All files $(BLUE)minishell$(DEF)$(GREEN) have been compiled.$(DEF)"

print_compiler_minishell:
	@echo "$(CYAN)================================$(DEF)"
	@echo "	$(YELLOW)Running MiniShell$(DEF)"
	@echo "$(CYAN)================================$(DEF)"
	@sleep 0.5

make_libft:
	@echo "$(CYAN)==============================$(DEF)"
	@echo "	$(YELLOW)Running libft$(DEF)"
	@echo "$(CYAN)==============================$(DEF)"
	@sleep 0.5
	@make -C libft
	@make bonus -C libft

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(READLINE_LIBRARY) $(OBJS) libft/libft.a -o $@

%.o: %.c minishell.h
	@echo "$(YELLOW)⚙️  Compiling $<...$(DEF)"
#	@printf "$(BLUE)⚙️  Norminette:$(DEF)$(GREEN) " && norminette $< && echo "$(DEF)"
	@$(CC) $(CFLAGS) $(READLINE_INCLUDE) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@make clean -C libft
	@echo "$(RED)🧹 Object files removed.$(DEF)"

fclean: clean
	@rm -f $(NAME)
	@rm -f libft/libft.a
	@echo "$(RED)🗑️  Program removed.$(DEF)"

re: fclean all

.PHONY: clean
