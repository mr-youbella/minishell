/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/07/22 16:34:48 by youbella         ###   ########.fr       */
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

t_redirections *add_redirections_out_in_list(char *str)
{
	size_t i;
	size_t len;
	size_t start_namefile;
	t_redirections *list;
	t_redirections *new_node;
	char *redirec_type;
	char *file_name;

	start_namefile = 0;
	len = 0;
	i = 0;
	list = NULL;
	while (str[i])
	{
		if (str[i] == '>')
		{
			if (str[i + 1] == '>')
			{
				i += 2;
				if (str[i] == 0)
				{
					printf(BLUE"minishell:%s %ssyntax error.\n"DEF, DEF, RED);
					return (NULL);
				}
				redirec_type = ft_strdup(">>");
			}
			else
			{
				i++;
				if (str[i] == 0)
				{
					printf(BLUE"minishell:%s %ssyntax error.\n"DEF, DEF, RED);
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

t_redirections *add_redirections_list(char *str, char c)
{
	size_t i;
	size_t len;
	size_t start;
	t_redirections *red;
	t_redirections *new_node;
	char *re;
	char *end_file;
	short exist;

	exist = 0;
	start = 0;
	len = 0;
	i = 0;
	red = NULL;
	while (str[i])
	{
		if (str[i] == '<')
		{
			if (str[i + 1] == '<' && c == 'h')
			{
				exist = 1;
				i += 2;
				if (str[i] == 0)
				{
					printf("minishell: Syntax error.\n");
					return (NULL);
				}
				re = ft_strdup("<<");
			}
			else if (str[i + 1] != '<' && str[i - 1] != '<' && c == '<')
			{
				exist = 1;
				i++;
				if (str[i] == 0)
				{
					printf("minishell: Syntax error.\n");
					return (NULL);
				}
				re = ft_strdup("<");
			}
			else
				i++;
			if (exist == 1)
			{
				exist = 0;
				while (str[i] == ' ')
					i++;
				start = i;
				if (c == 'h')
					while (str[i] && str[i] != '<' && str[i] != ' ')
						i++;
				else
					while (str[i] && str[i] != '<')
						i++;
				len = i - start;
				end_file = ft_substr(str, start, len);
				new_node = craete_new_node(re, end_file);
				add_node_in_back(&red, new_node);
			}
		}
		else
			i++;
	}
	return (red);
}

char *join_tokens(char **tokens)
{
	size_t i;
	char *tokens_in_line;

	i = 0;
	tokens_in_line = NULL;
	while (tokens[i])
	{
		tokens_in_line = ft_strjoin(tokens_in_line, tokens[i]);
		if (tokens[i + 1])
			tokens_in_line = ft_strjoin(tokens_in_line, " ");
		i++;
	}
	return (tokens_in_line);
}

short is_there_redirect(char *cmd_line, char redirect_type)
{
	size_t i;

	i = 0;
	while (cmd_line[i])
	{
		if ((redirect_type == '>' && cmd_line[i] == '>') || (redirect_type == '<' && cmd_line[i] == '<' && cmd_line[i + 1] != '<' && cmd_line[i - 1] != '<') || (redirect_type == 'h' && cmd_line[i] == '<' && cmd_line[i + 1] == '<'))
			return (1);
		i++;
	}
	return (0);
}

size_t strcpy_until_redirections(char *dst, const char *src, size_t n, char redirect_type)
{
	size_t i;
	size_t j;

	i = 0;
	j = 0;
	if (!n)
		return (ft_strlen(src));
	while (src[i] && i < n - 1)
	{
		if ((redirect_type == '>' && src[i] == '>') || (redirect_type == '<' && src[i] == '<' && src[i + 1] != '<' && src[i - 1] != '<') || (redirect_type == 'h' && src[i] == '<' && src[i + 1] == '<'))
		{
			if (redirect_type == 'h')
				i++;
			i++;
			while (src[i] == ' ')
				i++;
			while (ft_isprint(src[i]) && src[i] != ' ')
				i++;
		}
		if (!src[i])
			break ;
		dst[j] = src[i];
		i++;
		j++;
	}
	dst[j] = 0;
	return (ft_strlen(src));
}

size_t strlen_until_redirections(char *str, char redirect_type)
{
	size_t i;
	size_t len;

	i = 0;
	len = 0;
	while (str[i])
	{
		if ((redirect_type == '>' && str[i] == '>') || (redirect_type == '<' && str[i] == '<' && str[i + 1] != '<' && str[i - 1] != '<') || (redirect_type == 'h' && str[i] == '<' && str[i + 1] == '<'))
		{
			if (redirect_type == 'h')
				i++;
			i++;
			while (str[i] == ' ')
				i++;
			while (ft_isprint(str[i]) && str[i] != ' ')
				i++;
		}
		if (!str[i])
			break ;
		len++;
		i++;
	}
	return (len);
}

// int main()
// {
// 	char *dst = malloc(100);
// 	strcpy_until_redirections(dst, "cat << Makefile minishell.c", ft_strlen("cat << Makefile minishell.c") + 1, 'h');
// 	printf("%s\n", dst);
// 	printf("%zu\n", strlen_until_redirections("cat << Makefile minishell.c", 'h'));
// }
