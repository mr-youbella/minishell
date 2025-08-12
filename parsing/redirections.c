/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/06 12:08:35 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_redirections *last_node(t_redirections *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

static void add_node_in_back(t_redirections **lst, t_redirections *new)
{
	if (!new)
		return;
	if (!*lst)
	{
		*lst = new;
		return;
	}
	last_node(*lst)->next = new;
}

static t_redirections *craete_new_node(char *redirect_type, char *file)
{
	t_redirections *node;

	node = malloc(sizeof(t_redirections));
	if (!node)
		return (NULL);
	node->type_redirection = redirect_type;
	node->file_name = file;
	node->next = NULL;
	return (node);
}

short is_exist_redirect_pipe(char *cmd_line, char redirect_pipe)
{
	size_t i;

	i = 0;
	while (cmd_line[i])
	{
		if (cmd_line[i] == '"' || cmd_line[i] == 39)
		{
			i++;
			while (cmd_line[i] && cmd_line[i] != '"' && cmd_line[i] != 39)
				i++;
			if (cmd_line[i] == '"' || cmd_line[i] == 39)
				i++;
		}
		if (!cmd_line[i])
			break;
		if ((redirect_pipe == 'o' && cmd_line[i] == '>') || (redirect_pipe == 'i' && cmd_line[i] == '<' && cmd_line[i + 1] != '<' && cmd_line[i - 1] != '<') || (redirect_pipe == 'i' && cmd_line[i] == '<' && cmd_line[i + 1] == '<') || (redirect_pipe == '|' && cmd_line[i] == '|'))
			return (1);
		i++;
	}
	return (0);
}

t_redirections *list_redirections(char **tokens, char redirect_type)
{
	size_t i;
	t_redirections *list;
	t_redirections *new_node;

	i = 1;
	list = NULL;
	while (tokens[i])
	{
		if (ft_strlen(tokens[i]) == 2 && ((redirect_type == 'o' && !ft_strncmp(tokens[i], ">>", 2)) || (redirect_type == 'i' && !ft_strncmp(tokens[i], "<<", 2))))
		{
			i++;
			if (!tokens[i] || (ft_strlen(tokens[i]) == 2 && (!ft_strncmp(tokens[i], ">>", 2)) || !ft_strncmp(tokens[i], "<<", 2)))
			{
				printf(BLUE "minishell:%s %ssyntax error.\n" DEF, DEF, RED);
				return (NULL);
			}
		}
		else if ((ft_strlen(tokens[i]) == 1) && ((redirect_type == 'o' && !ft_strncmp(tokens[i], ">", 1)) || (redirect_type == 'i' && !ft_strncmp(tokens[i], "<", 1))))
		{
			i++;
			if (!tokens[i] || (ft_strlen(tokens[i]) == 1 && (!ft_strncmp(tokens[i], ">", 1) || !ft_strncmp(tokens[i], "<", 1))))
			{
				printf(BLUE "minishell:%s %ssyntax error.\n" DEF, DEF, RED);
				return (NULL);
			}
		}
		else
		{
			i++;
			continue;
		}
		new_node = craete_new_node(tokens[i - 1], tokens[i]);
		add_node_in_back(&list, new_node);
		i++;
	}
	return (list);
}
char *join_cmd_args(char *cmd_line)
{
	char **p;
	char *arg = NULL;
	size_t i;
	p = ft_split_first_cmd(cmd_line, ' ', 0, NULL);
	if (!p)
		return (NULL);
	arg = ft_strjoin(p[0], " ");
	i = 1;
	while (p[i])
	{
		if ((ft_strlen(p[i]) == 1 && !ft_strncmp(p[i], ">", 1)) || (ft_strlen(p[i]) == 2 && !ft_strncmp(p[i], ">>", 2)) || (ft_strlen(p[i]) == 1 && !ft_strncmp(p[i], "<", 1)) || (ft_strlen(p[i]) == 2 && !ft_strncmp(p[i], "<<", 2)))
			i += 2;
		else
		{
			arg = ft_strjoin(arg, p[i]);
			arg = ft_strjoin(arg, " ");
			i++;
		}
	}
	return (arg);
}

char *join_cmd_redirections(char *cmd_line)
{
	char **p;
	char *red = NULL;
	size_t i;
	p = ft_split_first_cmd(cmd_line, ' ', 0, NULL);
	if (!p)
		return (NULL);
	red = ft_strjoin(p[0], " ");
	i = 1;
	while (p[i])
	{
		if ((ft_strlen(p[i]) == 1 && !ft_strncmp(p[i], ">", 1)) || (ft_strlen(p[i]) == 2 && !ft_strncmp(p[i], ">>", 2)) || (ft_strlen(p[i]) == 1 && !ft_strncmp(p[i], "<", 1)) || (ft_strlen(p[i]) == 2 && !ft_strncmp(p[i], "<<", 2)))
		{
			red = ft_strjoin(red, p[i]);
			red = ft_strjoin(red, " ");
			if (p[i + 1])
			{
				red = ft_strjoin(red, p[i + 1]);
				red = ft_strjoin(red, " ");
			}
			if (p[i + 1])
				i += 2;
			else
				i++;
		}
		else
			i++;
	}
	return (red);
}
