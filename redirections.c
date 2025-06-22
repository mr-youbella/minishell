/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkannouf <wkannouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/22 19:15:51 by wkannouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct s_redirect
{
	char	*type_redirection;
	char	*file_name;
	struct s_redirect	*next;
}	t_redirect;

t_redirect	*lstlast(t_redirect *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	lstadd_back(t_redirect **lst, t_redirect *new)
{
	if (!new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	lstlast(*lst)->next = new;
}


t_redirect	*lstnew(char *file, char *redirection)
{
	t_redirect	*node;

	node = malloc(sizeof(t_redirect));
	if (!node)
		return (NULL);
	node->type_redirection = redirection;
	node->file_name = file;
	node->next = NULL;
	return (node);
}

t_redirect	*search_directions_name_file(char *str)
{
	size_t		i;
	size_t		len;
	size_t		start;
	t_redirect	*red;
	t_redirect	*new_node;
	char		*re;
	char		*file;

	start = 0;
	len = 0;
	i = 0;
	red = NULL;
	while(str[i])
	{
		if (str[i] == '>')
		{
			if (str[i + 1] == '>')
			{
				i += 2;
				if (str[i] == 0)
				{
					printf("minishell: Syntax error.\n");
					return (NULL);
				}
				re = ft_strdup(">>");
			}
			else
			{
				i++;
				if (str[i] == 0)
				{
					printf("minishell: Syntax error.\n");
					return (NULL);
				}
				re = ft_strdup(">");
			}
			while (str[i] == ' ')
				i++;
			start = i;
			while (str[i] && str[i] != '>' && str[i] != ' ')
				i++;
			len = i - start;
			file = ft_substr(str, start, len);
			new_node = lstnew(file, re);
			lstadd_back(&red, new_node);
		}
		else 
			i++;
	}
	return (red);
}

