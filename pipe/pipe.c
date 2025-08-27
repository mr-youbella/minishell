/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 04:14:59 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 21:56:41 by youbella         ###   ########.fr       */
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

t_pids	*new_node_pid(int pid)
{
	t_pids	*new_pid;

	new_pid = malloc(sizeof(t_pids));
	if (!new_pid)
		return (NULL);
	new_pid->pid = pid;
	new_pid->next = NULL;
	return (new_pid);
}

t_pids	*last_node_pid(t_pids *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	add_back_pids(t_pids **lst, t_pids *new)
{
	if (!new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	last_node_pid(*lst)->next = new;
}

static int	child_pipe(size_t i, size_t tokens_count,
						int *pipe_fd, t_var_pipe *varpipe)
{
	pid_t	pid;
	t_pids	*new_pid;

	pid = fork();
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

static pid_t	while_pipe(char **split_pipe, int *pipe_fd,
					size_t tokens_count, t_var *variables)
{
	size_t		i;
	pid_t		pid;
	int			in_fd;
	t_var_pipe	*varpipe;
	t_pids		*tmp;

	varpipe = malloc(sizeof(t_var_pipe));
	if (!varpipe)
		return (ft_status(1, 1), -3);
	ft_memset(varpipe, 0, sizeof(t_var_pipe));
	varpipe->split_pipe = split_pipe;
	1 && (varpipe->variables = variables, varpipe->pids = NULL);
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
