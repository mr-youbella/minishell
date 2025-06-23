/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 11:20:41 by youbella          #+#    #+#             */
/*   Updated: 2025/06/21 22:13:44 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*ft_line(char *str, int fd)
{
	char	*buffer;
	ssize_t	r;

	buffer = malloc((size_t)BUFFER_SIZE + 1);
	if (!buffer)
		return (free(str), NULL);
	buffer[0] = 0;
	while (!ft_strchr(buffer, '\n'))
	{
		r = read(fd, buffer, BUFFER_SIZE);
		if (!r || r == -1)
			break ;
		buffer[r] = 0;
		str = ft_strjoin_gnl(str, buffer);
	}
	return (free(buffer), str);
}

static char	*ft_next_line(char *line)
{
	char	*next_line;
	size_t	i;

	i = 0;
	while (line[i] && line[i] != '\n')
		i++;
	if (line[i] == '\n')
		i++;
	next_line = ft_strdup(&line[i]);
	if (!next_line)
		return (free(line), NULL);
	line[i] = 0;
	return (next_line);
}

char	*get_next_line(int fd)
{
	static char	*nl[OPEN_MAX];
	char		*buffer;
	char		*line;

	if (fd < 0 || fd > OPEN_MAX)
		return (NULL);
	if (BUFFER_SIZE <= 0 || read(fd, 0, 0) < 0)
		return (free(nl[fd]), nl[fd] = NULL, NULL);
	line = ft_line(nl[fd], fd);
	if (!line)
		return (nl[fd] = NULL, NULL);
	if (!*line)
		return (free(nl[fd]), nl[fd] = NULL, NULL);
	nl[fd] = ft_next_line(line);
	if (!nl[fd])
		return (NULL);
	buffer = ft_strdup(line);
	free(line);
	if (!buffer)
		return (free(nl[fd]), nl[fd] = NULL, NULL);
	return (buffer);
}
