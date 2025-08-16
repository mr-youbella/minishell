/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 06:03:27 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/16 16:30:08 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include "libft/libft.h"
#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define BGRED "\033[41m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[0;36m"
#define PINK "\033[35m"
#define DEF "\033[0m"

typedef struct s_redirections
{
	char *type_redirection;
	char *file_name;
	struct s_redirections *next;
} t_redirections;

char **ft_split_first_cmd(char *s, char c, int status, t_list *environment);
t_list *search_in_list(char *str, t_list *list);
short check_export_arg(char *arg);
short check_unset_arg(char *arg);
short is_exist_redirect_pipe(char *cmd_line, char redirect_pipe);
t_redirections *list_redirections(char **tokens, char redirect_type, t_list *environment);
char *ft_getenv(char *var, t_list *environment);
t_list *all_env(char *var_export, char *var_unset, char **env, t_list *export_list, short is_export_cmd, short env_export, short *cd_flag, char *old_pwd);
short is_exist_var(char *var, char **env, t_list *export_list);
// Commands
char *is_there_cmd(char **tokens, t_list *environment, int *status);
char *search_cmd(char *cmd, t_list *environment);
short echo_cmd(char **tokens);
t_list *env_cmd(char **env, t_list *export_list, short is_print);
void export_cmd(char **env, char **tokens, t_list **export_list);
void unset_cmd(char **tokens, char **env, t_list **export_list);
void cd_cmd(char *tokens, short *cd_flag);
char *pwd_cmd(short is_print);
void exit_cmd(int status);

char **ft_split_first_cmd2(char *cmd_line, char c, int status, t_list *environment);
char *join_cmd_args(char *cmd_line, t_list *environment);
char *join_cmd_redirections(char *cmd_line, t_list *environment);
char *extract_word(char *str, char c);
char *ft_dollar(char *str, int status, t_list *environment);
char **get_tokens_with_redirection(const char *cmd_line);

#endif
