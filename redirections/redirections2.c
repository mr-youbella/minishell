/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:13:43 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 05:53:11 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	redirect_input(char *type_redirection, char *file_name,
							t_var_redirect *var_redirection)
{
	if (ft_strlen(type_redirection) == 1
		&& !ft_strncmp(type_redirection, "<", 1))
	{
		if (var_redirection->fd_file_input > 0)
			close(var_redirection->fd_file_input);
		var_redirection->join_herdoc = NULL;
		if (!ft_strlen(file_name))
		{
			ft_putstr_fd("\033[34mminishell: ", 2);
			ft_putstr_fd("\033[31mambiguous redirect.\033[0m\n", 2);
			return (ft_status(1, 1), 0);
		}
		var_redirection->fd_file_input = open(file_name, O_RDONLY);
		if (var_redirection->fd_file_input < 0)
		{
			ft_putstr_fd("\033[34mminishell: \033[31m", 2);
			ft_putstr_fd(file_name, 2);
			ft_putstr_fd("\033[0m No such file or directory.\n", 2);
			return (ft_status(1, 1), 0);
		}
	}
	return (1);
}

short	setup_redirections(char **tokens_redirections,
							t_var_redirect *var_redirection)
{
	t_redirections	*redirc;

	var_redirection->fd_file_input = 0;
	var_redirection->fd_file_output = 0;
	var_redirection->join_herdoc = NULL;
	redirc = list_redirections(tokens_redirections, var_redirection->variables);
	if (!redirc)
		return (0);
	free_list(var_redirection->variables, NULL, redirc);
	while (redirc)
	{
		if (!redirect_output(redirc->type_redirection,
				redirc->file_name, var_redirection))
			return (free(var_redirection->join_herdoc), 0);
		herdoc(redirc->type_redirection, redirc->file_name,
			var_redirection);
		if (g_signal_flag == -5)
			return (g_signal_flag = 0, 0);
		if (!redirect_input(redirc->type_redirection,
				redirc->file_name, var_redirection))
			return (free(var_redirection->join_herdoc), 0);
		redirc = redirc->next;
	}
	return (1);
}

char	*builtin_cmd_redirections(char **tokens, int fd_pipe,
					t_var *variables, t_var_redirect *var_redirection)
{
	if (!tokens)
		return (var_redirection->return_val = 0, NULL);
	if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
		exit_cmd(variables->copy_env, variables, tokens, 1);
	else if (ft_strlen(tokens[0]) == 6
		&& !ft_strncmp(tokens[0], "export", 6))
		return (export_cmd(tokens, 1, variables, 0));
	else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		unset_cmd(tokens, variables);
	else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		return (echo_cmd(tokens, 1));
	else if (ft_strlen(tokens[0]) == 3 && (!ft_strncmp(tokens[0], "pwd", 3)
			|| !ft_strncmp(tokens[0], "PWD", 3)))
		return (ft_status(0, 1), pwd_cmd(0));
	else if (ft_strlen(tokens[0]) == 2
		&& !ft_strncmp(tokens[0], "cd", 2) && fd_pipe < 0)
	{
		variables->old_pwd = ft_getenv("PWD", variables);
		cd_cmd(tokens[1], variables);
		return (var_redirection->return_val = 0, NULL);
	}
	return (NULL);
}

static void	mange_fd_redirect(t_var_redirect *var_redirection, int *fd)
{
	if (var_redirection->fd_file_output > 0)
		dup2(var_redirection->fd_file_output, 1);
	if (var_redirection->join_herdoc)
		dup2(fd[0], 0);
	else if (var_redirection->fd_file_input > 0)
	{
		dup2(var_redirection->fd_file_input, 0);
		close(var_redirection->fd_file_input);
	}
	else if (var_redirection->fd_pipe > 0)
		dup2(fd[0], 0);
	close(fd[0]);
	close(var_redirection->fd_output[0]);
	close(fd[1]);
	close(var_redirection->fd_output[1]);
}

pid_t	create_process_redirection(t_var_redirect *var_redirection,
					t_var *variables, char **tokens, int *fd)
{
	var_redirection->pid = fork();
	if (!var_redirection->pid)
	{
		mange_fd_redirect(var_redirection, fd);
		if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		{
			if (var_redirection->cmd_result)
				printf("%s", var_redirection-> cmd_result);
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
			printf("%s\n", var_redirection->cmd_result);
		else if (ft_strlen(tokens[0]) == 6
			&& !ft_strncmp(tokens[0], "export", 6))
		{
			if (var_redirection->cmd_result)
				printf("%s", var_redirection->cmd_result);
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
			env_cmd(1, variables);
		else
			execve(var_redirection->path_cmd, tokens, variables->env);
		exit(ft_status(0, 0));
	}
	return (free(var_redirection->cmd_result), var_redirection->pid);
}
