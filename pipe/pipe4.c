/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe4.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 00:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 07:16:30 by youbella         ###   ########.fr       */
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

int	start_process(size_t i, int *pipe_fd, size_t tokens_count, t_var_pipe *p)
{
	size_t		j;
	pid_t		pid;
	int			in_fd;

	1 && (in_fd = 0, pid = fork());
	if (pid == 0)
	{
		j = 0;
		if (i)
			in_fd = pipe_fd[(i - 1) * 2];
		mange_pipes(pipe_fd, i, tokens_count);
		if (is_exist_redirect_pipe(p->split_pipe[i], 'r'))
		{
			redirections(p->split_pipe[i], in_fd, p->variables);
			while (j < 2 * (tokens_count - 1))
				close(pipe_fd[j++]);
			exit(0);
		}
		while (j < 2 * (tokens_count - 1))
			close(pipe_fd[j++]);
		execute_cmd_pipe(p->tokens,
			p->variables->copy_env, p->variables);
		execute_cmd_pipe(p->tokens, p->variables->copy_env, p->variables);
	}
	return (pid);
}

pid_t	while_pipe2(size_t tokens_count,
							int *pipe_fd, t_var_pipe *p)
{
	size_t		i;
	pid_t		pid;

	i = 0;
	while (i < tokens_count)
	{
		p->tokens = split_command(p->split_pipe[i], 1, p->variables);
		pid = start_process(i, pipe_fd, tokens_count, p);
		if (check_pids(pid, p) == -3)
			return (-3);
		add_back_pids(&p->pids, new_node_pid(pid));
		close_fdpipe(pid, i, tokens_count, pipe_fd);
		free_array(p->tokens, 0, p->variables);
		i++;
	}
	return (pid);
}
