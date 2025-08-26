/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 04:14:59 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 04:30:18 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	close_fdpipe(pid_t pid, size_t i, size_t tokens_count, int *pipe_fd)
{
	if (pid > 0)
	{
		if (i != tokens_count - 1)
			close(pipe_fd[i * 2 + 1]);
		if (i != 0)
			close(pipe_fd[(i - 1) * 2]);
	}
}

static int	child_pipe(size_t i, size_t tokens_count,
						int *pipe_fd, t_var_pipe *varpipe)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		mange_pipes(pipe_fd, i, tokens_count);
		if (is_exist_redirect_pipe(varpipe->split_pipe[i], 'r'))
		{
			if (varpipe->redirections_output)
				printf("%s", varpipe->redirections_output);
			exit(0);
		}
		execute_cmd_pipe(varpipe->tokens,
			varpipe->variables->copy_env, varpipe->variables);
	}
	close_fdpipe(pid, i, tokens_count, pipe_fd);
	return (pid);
}

static pid_t	while_pipe(char **split_pipe, int *pipe_fd,
					size_t tokens_count, t_var *variables)
{
	size_t		i;
	pid_t		pid;
	int			in_fd;
	t_var_pipe	*varpipe;

	varpipe = malloc(sizeof(t_var_pipe));
	varpipe->split_pipe = split_pipe;
	varpipe->variables = variables;
	1 && (i = 0, in_fd = 0);
	while (i < tokens_count)
	{
		if (i)
			in_fd = pipe_fd[(i - 1) * 2];
		varpipe->tokens = split_command(split_pipe[i], 1, variables);
		if (is_exist_redirect_pipe(split_pipe[i], 'r'))
			varpipe->redirections_output = redirections(split_pipe[i],
					in_fd, variables, NULL);
		pid = child_pipe(i, tokens_count, pipe_fd, varpipe);
		free_array(varpipe->tokens, 0, variables);
		i++;
	}
	return (free(varpipe), pid);
}

static void	ft_pipe_loop(char **split_pipe, t_var *variables)
{
	size_t	i;
	pid_t	pid;
	size_t	tokens_count;
	int		*pipe_fd;

	tokens_count = 0;
	while (split_pipe[tokens_count])
		tokens_count++;
	pipe_fd = malloc(sizeof(int) * 2 * (tokens_count - 1));
	if (!pipe_fd)
		return ;
	i = 0;
	while (i < tokens_count - 1)
	{
		pipe(pipe_fd + i * 2);
		i++;
	}
	i = 0;
	pid = while_pipe(split_pipe, pipe_fd, tokens_count, variables);
	end_pipe(tokens_count, pipe_fd, pid, variables);
	free(pipe_fd);
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
