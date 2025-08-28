/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 04:14:59 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 07:12:29 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	close_fdpipe(pid_t pid, size_t i, size_t tokens_count, int *pipe_fd)
{
	if (pid > 0)
	{
		if (i != tokens_count - 1)
			close(pipe_fd[i * 2 + 1]);
		if (i != 0)
			close(pipe_fd[(i - 1) * 2]);
	}
}

static pid_t	while_pipe(char **split_pipe, int *pipe_fd,
					size_t tokens_count, t_var *var)
{
	pid_t		pid;
	t_var_pipe	*varpipe;
	t_pids		*tmp;

	varpipe = malloc(sizeof(t_var_pipe));
	if (!varpipe)
		return (ft_status(1, 1), -3);
	ft_memset(varpipe, 0, sizeof(t_var_pipe));
	varpipe->split_pipe = split_pipe;
	1 && (varpipe->variables = var, varpipe->pids = NULL);
	pid = while_pipe2(tokens_count, pipe_fd, varpipe);
	while (varpipe->pids)
	{
		1 && (tmp = varpipe->pids, varpipe->pids = varpipe->pids->next);
		free(tmp);
	}
	return (free(varpipe), pid);
}

static void	ft_pipe_loop(char **split_pipe, t_var *variables)
{
	pid_t	pid;
	size_t	tokens_count;
	int		*pipe_fd;

	tokens_count = 0;
	while (split_pipe[tokens_count])
		tokens_count++;
	pipe_fd = malloc(sizeof(int) * 2 * (tokens_count - 1));
	if (!pipe_fd)
		return ;
	if (!create_pipes(tokens_count, pipe_fd))
		return ;
	pid = while_pipe(split_pipe, pipe_fd, tokens_count, variables);
	if (pid == -3)
		return (free(pipe_fd));
	end_pipe(tokens_count, pipe_fd, pid, variables);
	return ;
}

void	ft_pipe(char *cmd_line, t_var *variables)
{
	size_t	i;
	char	**split_pipe;

	i = 0;
	split_pipe = get_tokens_pipe(cmd_line, variables);
	if (!split_pipe)
		return ;
	ft_pipe_loop(split_pipe, variables);
	while (split_pipe[i])
		free(split_pipe[i++]);
	free(split_pipe);
}
