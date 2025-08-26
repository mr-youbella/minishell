/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 04:20:41 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 13:48:08 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	exec_commands(char **tokens, char **copy_env,
						struct termios *ctr, char *path_cmd)
{
	pid_t	pid;
	int		status;

	if (!path_cmd)
		return ;
	pid = fork();
	if (pid < 0)
		return ;
	if (!pid)
	{
		execve(path_cmd, tokens, copy_env);
		ft_putstr_fd("\033[34mminishell: \033[31m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m: ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd(".\n", 2);
		exit(1);
	}
	else
	{
		1 && (g_signal_flag = pid, waitpid(pid, &status, 0));
		if (!WIFSIGNALED(status))
			ft_status(WEXITSTATUS(status), 1);
		g_signal_flag = 0;
	}
	return (tcsetattr(0, 0, ctr), free(path_cmd));
}

static short	is_redirections_pipe(char **tokens,
									char *cmd_line, t_var *variables)
{
	char	*redirection_output;

	if (!tokens || !tokens[0])
		return (free(tokens), 1);
	if (is_exist_redirect_pipe(cmd_line, '|'))
	{
		ft_pipe(cmd_line, variables);
		return (1);
	}
	else if (is_exist_redirect_pipe(cmd_line, 'r'))
	{
		redirection_output = redirections(cmd_line, -1, variables, NULL);
		if (redirection_output)
			printf("%s", redirection_output);
		return (free(redirection_output), 1);
	}
	return (0);
}

static char	*ft_readline(char *pwd, size_t i, char *cmd_line)
{
	char	**path;
	char	*last_dir;
	char	*last_dir_tmp;

	last_dir = NULL;
	if (ft_strlen(pwd) == 1 && !ft_strncmp(pwd, "/", 1))
		last_dir = ft_strdup("/");
	else
	{
		path = ft_split(pwd, '/');
		while (path && path[i])
			last_dir = path[i++];
		if (last_dir)
			last_dir = ft_strdup(last_dir);
		free_array(path, 0, NULL);
	}
	last_dir_tmp = ft_strjoin("\033[1;94m", last_dir);
	free(last_dir);
	last_dir = ft_strjoin(last_dir_tmp, "\033[0m");
	free(last_dir_tmp);
	if (ft_status(0, 0))
		last_dir_tmp = ft_strjoin("\033[31m➥\033[0m ", last_dir);
	else
		last_dir_tmp = ft_strjoin("\033[32m➥\033[0m ", last_dir);
	free(last_dir);
	last_dir = ft_strjoin(last_dir_tmp, " ");
	free(last_dir_tmp);
	return (cmd_line = readline(last_dir), free(pwd), free(last_dir), cmd_line);
}

void	minishell_loop(t_var *variables, char **copy_env, struct termios *ctr)
{
	t_list	*new_leak;
	char	*command;
	char	**tokens;

	while (1)
	{
		variables->environment = all_env(NULL, NULL, 0, variables);
		free_list(variables, variables->environment, NULL);
		command = ft_readline(pwd_cmd(0), 0, NULL);
		new_leak = ft_lstnew(command);
		ft_lstadd_back(&variables->leaks, new_leak);
		if (!command)
			exit_cmd(copy_env, variables, NULL, 1);
		if (!command[0])
			continue ;
		add_history(command);
		tokens = split_command(command, 1, variables);
		free_array(tokens, 1, variables);
		if (is_redirections_pipe(tokens, command, variables)
			|| builtin_commands(tokens, copy_env, variables))
			continue ;
		exec_commands(tokens, copy_env, ctr, is_there_cmd(tokens, variables));
	}
}
