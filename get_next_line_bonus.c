/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 11:20:41 by youbella          #+#    #+#             */
/*   Updated: 2025/07/04 17:04:09 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strjoin_gnl(char *s1, const char *s2)
{
	size_t	i;
	size_t	j;
	char	*p;

	i = 0;
	j = 0;
	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	p = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!p)
		return (free(s1), NULL);
	while (s1[i])
		p[j++] = s1[i++];
	i = 0;
	while (s2[i])
		p[j++] = s2[i++];
	p[j] = 0;
	return (free(s1), p);
}

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
