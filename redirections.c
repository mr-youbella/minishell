/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/07/12 17:41:28 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_redirections	*last_node(t_redirections *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

static void	add_node_in_back(t_redirections **lst, t_redirections *new)
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

static t_redirections	*craete_new_node(char *redirect_type, char *file)
{
	t_redirections	*node;

	node = malloc(sizeof(t_redirections));
	if (!node)
		return (NULL);
	node->type_redirection = redirect_type;
	node->file_name = file;
	node->next = NULL;
	return (node);
}

t_redirections	*add_redirections_out_in_list(char *str)
{
	size_t			i;
	size_t			len;
	size_t			start_namefile;
	t_redirections	*list;
	t_redirections	*new_node;
	char			*redirec_type;
	char			*file_name;

	start_namefile = 0;
	len = 0;
	i = 0;
	list = NULL;
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
				redirec_type = ft_strdup(">>");
			}
			else
			{
				i++;
				if (str[i] == 0)
				{
					printf("minishell: Syntax error.\n");
					return (NULL);
				}
				redirec_type = ft_strdup(">");
			}
			while (str[i] == ' ')
				i++;
			start_namefile = i;
			while (str[i] && str[i] != '>' && str[i] != ' ')
				i++;
			len = i - start_namefile;
			file_name = ft_substr(str, start_namefile, len);
			new_node = craete_new_node(redirec_type, file_name);
			add_node_in_back(&list, new_node);
		}
		else 
			i++;
	}
	return (list);
}

t_redirections	*add_redirections_herdoc_in_list(char *str)
{
	size_t			i;
	size_t			len;
	size_t			start;
	t_redirections	*red;
	t_redirections	*new_node;
	char			*re;
	char			*end_file;

	start = 0;
	len = 0;
	i = 0;
	red = NULL;
	while(str[i])
	{
		if (str[i] == '<')
		{
			if (str[i + 1] == '<')
			{
				i += 2;
				if (str[i] == 0)
				{
					printf("minishell: Syntax error.\n");
					return (NULL);
				}
				re = ft_strdup("<<");
			}
			while (str[i] == ' ')
				i++;
			start = i;
			while (str[i] && str[i] != '<' && str[i] != ' ')
				i++;
			len = i - start;
			end_file = ft_substr(str, start, len);
			new_node = craete_new_node(re, end_file);
			add_node_in_back(&red, new_node);
		}
		else 
			i++;
	}
	return (red);
}

short	is_there_redirect_out(char *cmd_line)
{
	size_t	i;

	i = 0;
	while(cmd_line[i])
	{
		if (cmd_line[i] == '>')
			return (1);
		i++;
	}
	return (0);
}

char	*join_tokens(char **tokens)
{
	size_t	i;
	char	*tokens_in_line;
	
	i = 0;
	tokens_in_line = NULL;
	while(tokens[i])
	{
		tokens_in_line = ft_strjoin(tokens_in_line, tokens[i]);
		if (tokens[i + 1])
			tokens_in_line = ft_strjoin(tokens_in_line, " ");
		i++;
	}
	return (tokens_in_line);
}

size_t	strcpy_until_redirections(char *dst, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	if (!n)
		return (ft_strlen(src));
	while (src[i] && i < n - 1 && src[i] != '>')
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;
	return (ft_strlen(src));
}

size_t	strlen_until_redirections(char *str, char type)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i] && str[i] != '>')
		i++;
	return (i);
}
