/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 06:03:27 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/23 13:13:50 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <stdio.h>
# include <signal.h>
# include <termios.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>

# define GREEN "\033[32m"
# define RED "\033[31m"
# define BGRED "\033[41m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define CYAN "\033[0;36m"
# define PINK "\033[35m"
# define DEF "\033[0m"

typedef struct s_var
{
	t_list	*export_list;
	t_list	*environment;
	t_list 	*leaks;
	char 	**env;
	char 	**copy_env;
	short	cd_flag;
	char	*old_pwd;
}	t_var;

typedef struct s_var_pipe
{
	char	*redirections_output;
	char	**split_pipe;
	char	**tokens;
	t_var	*variables;
}	t_var_pipe;

typedef struct s_redirections
{
	char					*type_redirection;
	char					*file_name;
	struct s_redirections	*next;
}	t_redirections;

short			check_export_arg(char *arg);
short			check_unset_arg(char *arg);
short			is_exist_redirect_pipe(char *cmd_line, char redirect_pipe);
t_redirections	*list_redirections(char **tokens, t_var *variables);
char	*ft_getenv(char *var, t_var *variables);
t_list	*all_env(char *var_export, char *var_unset, char **copy_env, short is_export_cmd, short env_export, t_var *variables);
short	is_exist_var(char *var, t_var *variables);

char	*is_there_cmd(char **tokens, t_var *variables);
char	*search_cmd(char *cmd, t_var *variables);
char			*echo_cmd(char **tokens, short is_return);
t_list	*env_cmd(short is_print, char **copy_env, t_var *variables);
char	*export_cmd(char **copy_env, char **tokens, short is_return, t_var *variables);
void	unset_cmd(char **tokens, char **copy_env,t_var *variables);
void	cd_cmd(char *tokens, t_var *variables);
char			*pwd_cmd(short is_print);
void	exit_cmd(char **copy_env, t_var *variables);

char			*join_cmd_args(char *cmd_line);
char			*extract_word(char *str, char c);
char	*ft_dollar(char *str, t_var *variables);
char			**get_tokens_with_redirection(const char *cmd_line);
int				ft_status(int status, short is_change);
short			is_exist_in_env(char *str, char **env, long position);

char	**split_commmand_with_quotes(char *command, char c, short is_dollar, t_var *variables);
char	**split_command(char *cmd_line, char c, short is_dollar, t_var *variables);
void	free_leaks(t_var *variables);
short	is_buitin_cmd(char *token);
#endif
