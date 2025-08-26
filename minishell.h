/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 06:03:27 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/26 07:03:27 by youbella         ###   ########.fr       */
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
# include <sys/wait.h>
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

int	g_signal_flag;

typedef struct s_env
{
	char	*var_export;
	char	*var_unset;
	char	*name_var_export;
	short	is_export_cmd;
}	t_env;

typedef struct s_var
{
	struct termios	*ctr;
	size_t			i;
	t_list			*export_list;
	t_list			*environment;
	t_list			*leaks;
	char			*export;
	char			**env;
	char			**copy_env;
	short			cd_flag;
	char			*old_pwd;
	short			is_return;
	short			is_export_cmd;
}	t_var;

typedef struct s_var_pipe
{
	char	*redirections_output;
	char	**split_pipe;
	char	**tokens;
	t_var	*variables;
}	t_var_pipe;

typedef struct s_var_redirect
{
	pid_t	pid;
	t_var	*variables;
	int		fd_file_input;
	int		fd_file_output;
	char	*join_herdoc;
	short	return_val;
	char	*cmd_result;
	int		fd_output[2];
	int		fd_pipe;
	char	*path_cmd;
	char	*pipe_output;
	char	**tokens;
	char	*output_cmd;
}	t_var_redirect;

typedef struct s_redirections
{
	char					*type_redirection;
	char					*file_name;
	struct s_redirections	*next;
}	t_redirections;

typedef struct s_dollar
{
	char	*str;
	size_t	len;
	size_t	i;
	size_t	j;
}	t_dollar;

typedef struct s_ft_var
{
	char	*buffer;
	char	**tokens;
	char	**new_tokens;
	short	is_space;
	short	is_dollar;
	t_var	*variables;
	size_t	c_single_quote;
	size_t	c_double_quotes;
	short	is_d_quote;
	short	is_s_quote;
	size_t	j;
	short	in_token;
}	t_ft_var;

int				ft_status(int status, short is_change);
// Split commands
char			**split_command(char *cmd_line, short is_dollar,
					t_var *variables);
char			**split_commmand_with_quotes(char *command, char c,
					short is_dollar, t_var *variables);
char			*extract_word(char *str, char c);
short			is_unclose_quotes(size_t single_quote, size_t double_quotes);
char			**get_tokens_with_redirection(char *cmd_line);
size_t			count_tokens_with_redirection(char *cmd_line);
char			*ft_dollar(char *str, t_var *variables, size_t len, char quote);
short			len_dollar(size_t *i, char quote,
					t_var *variables, t_dollar *dollar_var);
// Redirections
t_redirections	*list_redirections(char **tokens, t_var *variables);
t_redirections	*craete_new_node(char *redirect_type, char *file);
char			*redirections(char *cmd_line, int fd_pipe,
					t_var *variables, char *s);
pid_t			create_process_redirection(t_var_redirect *var_redirection,
					t_var *variables, char **tokens, int *fd);
short			redirect_output(char *type_redirection, char *file_name,
					t_var_redirect *var_redirection);
char			*join_cmd_args(char *cmd_line);
short			check_syntax_redirect(char **tokens, t_ft_var *list_var);
char			*read_fd(int fd);
short			setup_redirections(char **tokens_redirections,
					t_var_redirect *var_redirection);
char			*builtin_cmd_redirections(char **tokens, int fd_pipe,
					t_var *variables, t_var_redirect *var_redirection);
void			herdoc(char *type_redirection, char *file_name,
					t_var_redirect *var_redirection);
short			is_ambiguous_redirect(char *token, t_var *variables);
void			add_node_in_back(t_redirections **lst, t_redirections *new);
short			is_exist_redirect_pipe(char *cmd_line, char redirect_pipe);
char			*join_cmd_args(char *cmd_line);
// Commands
short			builtin_commands2(char **tokens, t_var *variables);
short			builtin_commands(char **tokens, char **copy_env,
					t_var *variables);
char			*is_there_cmd(char **tokens, t_var *variables);
void			exit_cmd(char **copy_env, t_var *variables,
					char **tokens, short is_print);
char			*echo_cmd(char **tokens, short is_return);
t_list			*env_cmd(short is_print, t_var *variables);
char			*export_cmd(char **tokens, short is_return, t_var *variables);
void			unset_cmd(char **tokens, t_var *variables);
void			cd_cmd(char *tokens, t_var *variables);
char			*pwd_cmd(short is_print);
void			sort_env(char **env_arr, int count);
void			loop_print_env(char **env_array, t_var *variables,
					short is_return, size_t count);
char			*search_cmd(char *cmd, t_var *variables);
short			is_buitin_cmd(char *token);
void			minishell_loop(t_var *variables,
					char **copy_env, struct termios *ctr);
// Enviroment
char			**copy_environment(char	**env);
t_list			*all_env(char *var_export, char *var_unset,
					short env_export, t_var *variables);
short			is_exist_var(char *var, t_var *variables, t_list *export_list);
short			check_oldpwd(size_t i, size_t j,
					t_var *variables, char *name_var_env);
short			check_pwdenv(char *name_var_env, size_t i,
					size_t j, t_var *variables);
void			get_export(t_var *variables,
					t_list **enviroment, t_env *env_var);
short			check_export_arg(char *arg);
short			check_unset_arg(char *arg);
char			*ft_getenv(char *var, t_var *variables);
short			is_exist_in_env(char *str, char **env, long position);
short			is_with_value(t_env *env_var);
// pipe
void			ft_pipe(char *cmd_line, t_var *variables);
void			mange_pipes(int *pipe_fd, size_t i, size_t tokens_count);
void			mange_pipes(int *pipe_fd, size_t i, size_t tokens_count);
void			execute_cmd_pipe(char **tokens, char **copy_env,
					t_var *variables);
void			end_pipe(size_t tokens_count, int *pipe_fd,
					pid_t pid, t_var *variables);
char			**get_tokens_pipe(char *cmd_line, t_var *variables);
// Free
void			free_array(char **arr, short is_stock, t_var *variables);
void			free_list(t_var *variables, t_list *list,
					t_redirections *redirections);
void			free_leaks(t_var *variables);
#endif
