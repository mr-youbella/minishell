/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe4.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 00:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 12:57:10 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	check_pids(pid_t pid, t_var_pipe *varpipe)
{
	if (pid < 0)
	{
		while (varpipe->pids)
		{
			kill(varpipe->pids->pid, SIGKILL);
			waitpid(varpipe->pids->pid, NULL, 0);
			varpipe->pids = varpipe->pids->next;
		}
		return (ft_status(1, 1), perror("fork"), -3);
	}
	return (1);
}

static void	child_process(size_t i, t_var_pipe *varpipe)
{
	if (is_exist_redirect_pipe(varpipe->split_pipe[i], 'r'))
	{
		if (varpipe->redirections_output)
			printf("%s", varpipe->redirections_output);
		exit(0);
	}
	execute_cmd_pipe(varpipe->tokens,
		varpipe->variables->copy_env, varpipe->variables);
}

static int	child_pipe(size_t i, size_t tokens_count,
						int *pipe_fd, t_var_pipe *varpipe)
{
	pid_t	pid;
	t_pids	*new_pid;

	pid = fork();
	if (check_pids(pid, varpipe) == -3)
		return (-3);
	if (pid == 0)
	{
		mange_pipes(pipe_fd, i, tokens_count);
		child_process(i, varpipe);
		execute_cmd_pipe(varpipe->tokens,
			varpipe->variables->copy_env, varpipe->variables);
	}
	new_pid = new_node_pid(pid);
	add_back_pids(&varpipe->pids, new_pid);
	close_fdpipe(pid, i, tokens_count, pipe_fd);
	if (varpipe->redirections_output)
	{
		free(varpipe->redirections_output);
		varpipe->redirections_output = NULL;
	}
	return (pid);
}

pid_t	while_pipe2(char **split_pipe, size_t tokens_count,
							int *pipe_fd, t_var_pipe *varpipe)
{
	size_t		i;
	int			in_fd;
	pid_t		pid;

	i = 0;
	in_fd = 0;
	while (i < tokens_count)
	{
		if (i)
			in_fd = pipe_fd[(i - 1) * 2];
		else
			in_fd = -3;
		varpipe->tokens = split_command(split_pipe[i], 1, varpipe->variables);
		if (is_exist_redirect_pipe(split_pipe[i], 'r'))
			varpipe->redirections_output = redirections(split_pipe[i],
					in_fd, varpipe->variables, NULL);
		pid = child_pipe(i, tokens_count, pipe_fd, varpipe);
		free_array(varpipe->tokens, 0, varpipe->variables);
		i++;
	}
	return (pid);
}
