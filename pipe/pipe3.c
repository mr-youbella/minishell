/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:31:01 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 17:32:04 by youbella         ###   ########.fr       */
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
