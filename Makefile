# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/11 23:47:48 by youbella          #+#    #+#              #
#    Updated: 2025/08/26 16:21:12 by youbella         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror #-g -fsanitize=address
READLINE_INCLUDE = -I$(shell brew --prefix readline)/include
READLINE_LIBRARY = -L$(shell brew --prefix readline)/lib -lreadline

SRC = minishell.c split_commands/split_commands.c split_commands/split_cmd_with_quotes.c split_commands/utils_split_commands.c \
	split_commands/get_tokens_with_redirect.c redirections/redirections.c redirections/redirections2.c redirections/redirections3.c redirections/redirections4.c redirections/redirections5.c redirections/redirections6.c \
	free_leaks/free.c commands/commands.c commands/builtin_commands.c  commands/echo_env_cmd.c commands/export_cmd.c commands/export_cmd2.c commands/search_cmd.c \
	split_commands/dollar_sign_expand.c split_commands/dollar_sign_expand2.c enviroment/enviroment.c enviroment/enviroment2.c enviroment/enviroment3.c enviroment/enviroment4.c commands/exec_command.c \
	pipe/pipe.c pipe/pipe2.c
	
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
# 	@printf "$(BLUE)⚙️  Norminette:$(DEF)$(GREEN) " && norminette $< && echo "$(DEF)"
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
