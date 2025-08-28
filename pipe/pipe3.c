/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:31:01 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 00:18:48 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

short	create_pipes(size_t tokens_count, int *pipe_fd)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (i < tokens_count - 1)
	{
		if (pipe(pipe_fd + i * 2) < 0)
		{
			perror("pipe");
			while (j < i * 2)
				close(pipe_fd[j++]);
			ft_status(1, 1);
			return (free(pipe_fd), 0);
		}
		i++;
	}
	return (1);
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

static t_pids	*last_node_pid(t_pids *lst)
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
