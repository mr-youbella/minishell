/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections6.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:18:44 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 15:18:53 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_redirections	*last_node(t_redirections *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	add_node_in_back(t_redirections **lst, t_redirections *new)
{
	if (!new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	last_node(*lst)->next = new;
}

t_redirections	*craete_new_node(char *redirect_type, char *file)
{
	t_redirections	*node;

	node = malloc(sizeof(t_redirections));
	if (!node)
		return (ft_status(1, 1), NULL);
	ft_memset(node, 0, sizeof(t_redirections));
	node->type_redirection = redirect_type;
	node->file_name = file;
	node->next = NULL;
	return (node);
}

char	*read_fd(int fd)
{
	char	*file_content;
	char	*buffer;
	char	*tmp;
	ssize_t	r;

	if (fd < 0)
		return (NULL);
	file_content = NULL;
	buffer = malloc(43);
	if (!buffer)
		return (ft_status(1, 1), NULL);
	buffer[0] = 0;
	while (1)
	{
		r = read(fd, buffer, 42);
		if (r <= 0)
			break ;
		buffer[r] = 0;
		tmp = file_content;
		file_content = ft_strjoin(file_content, buffer);
		free(tmp);
		if (!file_content)
			return (free(buffer), NULL);
	}
	return (free(buffer), file_content);
}

void	check_is_exits_quotes(char *token, t_var *variables)
{
	size_t	i;
	char	**str;

	i = 0;
	str = split_commmand_with_quotes(token, ' ', 0, variables);
	while (str[0][i])
	{
		if (str[0][i] == 39 || str[0][i] == '"')
		{
			free(str[0]);
			free(str);
			variables->is_quotes_herdoc = 1;
			return ;
		}
		i++;
	}
	free(str[0]);
	free(str);
	variables->is_quotes_herdoc = 0;
}
