/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/16 19:11:32 by youbella         ###   ########.fr       */
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

short is_ambiguous_redirect(char *token, t_list *environment)
{
	size_t i;
	size_t j;
	size_t start;
	size_t var_len;
	char *var_name;
	char *var_value;

	i = 0;
	while (token[i])
	{
		var_len = 0;
		j = 0;
		if (token[i] == 39 || token[i] == '"')
		{
			i++;
			while (token[i] && token[i] != 39 && token[i] != '"')
				i++;
			if (token[i] == 39 || token[i] == '"')
				i++;
		}
		if (token[i] == '$')
		{
			start = i;
			i++;
			while ((token[i]) && (ft_isalnum(token[i]) || token[i] == '_'))
			{
				var_len++;
				i++;
			}
			var_name = ft_substr(token, start, var_len + 1);
			var_value = ft_dollar(var_name, 0, environment);
			while (var_value[j])
			{
				if (var_value[j] == ' ')
					return (1);
				j++;
			}
		}
		else
			i++;
	}
	return (0);
}

t_redirections *list_redirections(char **tokens, t_list *environment)
{
	size_t i;
	t_redirections *list;
	t_redirections *new_node;

	i = 0;
	list = NULL;
	while (tokens[i])
	{
		if (ft_strlen(tokens[i]) == 2 && ((!ft_strncmp(tokens[i], ">>", 2)) || (!ft_strncmp(tokens[i], "<<", 2))))
		{
			i++;
			if (!tokens[i] || (ft_strlen(tokens[i]) == 2 && (!ft_strncmp(tokens[i], ">>", 2)) || !ft_strncmp(tokens[i], "<<", 2)))
			{
				printf(BLUE "minishell:%s %ssyntax error.\n" DEF, DEF, RED);
				return (NULL);
			}
		}
		else if ((ft_strlen(tokens[i]) == 1) && ((!ft_strncmp(tokens[i], ">", 1)) || (!ft_strncmp(tokens[i], "<", 1))))
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
		char **tt;
		char **t = ft_split_first_cmd2(tokens[i - 1], ' ', 0, environment);
		if (ft_strlen(tokens[i - 1]) == 2 && !ft_strncmp(tokens[i - 1], "<<", 2))
			new_node = craete_new_node(t[0], tokens[i]);
		else if (is_ambiguous_redirect(tokens[i], environment))
		{
			tokens[i] = ft_strdup("");
			new_node = craete_new_node(t[0], tokens[i]);
		}
		else
		{
			tt = ft_split_first_cmd2(tokens[i], ' ', 0, environment);
			new_node = craete_new_node(t[0], tt[0]);
		}
		add_node_in_back(&list, new_node);
		i++;
	}
	return (list);
}

size_t count_tokens_with_redirection(const char *cmd_line)
{
	size_t i = 0;
	size_t count = 0;
	char c;
	short is_single_quote = 0;
	short is_double_quote = 0;
	short in_token = 0;

	while (cmd_line[i])
	{
		c = cmd_line[i];
		if (c == 39 && !is_double_quote)
		{
			is_single_quote = !is_single_quote;
			in_token = 1;
			i++;
			continue;
		}
		if (c == '"' && !is_single_quote)
		{
			is_double_quote = !is_double_quote;
			in_token = 1;
			i++;
			continue;
		}

		if (!is_single_quote && !is_double_quote)
		{
			if (c == ' ')
			{
				if (in_token)
				{
					count++;
					in_token = 0;
				}
				i++;
				continue;
			}
			if (c == '>' || c == '<')
			{
				if (in_token)
				{
					count++;
					in_token = 0;
				}
				if ((c == '>' && cmd_line[i + 1] == '>') ||
					(c == '<' && cmd_line[i + 1] == '<'))
					i += 2;
				else
					i++;
				count++;
				continue;
			}
		}

		in_token = 1;
		i++;
	}

	if (in_token)
		count++;

	return count;
}

char **get_tokens_with_redirection(const char *cmd_line)
{
	size_t i = 0;
	size_t k = 0;
	char c;
	char *op;
	short is_single_quote = 0;
	short is_double_quote = 0;
	char *buffer = NULL;
	size_t total = count_tokens_with_redirection(cmd_line);
	char **tokens = malloc((total + 1) * sizeof(char *));

	if (!tokens)
		return NULL;
	while (cmd_line[i])
	{
		c = cmd_line[i];
		if (c == 39 && !is_double_quote)
			is_single_quote = !is_single_quote;
		else if (c == '"' && !is_single_quote)
			is_double_quote = !is_double_quote;
		if (!is_single_quote && !is_double_quote && (c == '>' || c == '<'))
		{
			if (buffer)
			{
				tokens[k++] = buffer;
				buffer = NULL;
			}
			if (c == '>' && cmd_line[i + 1] == '>')
			{
				tokens[k++] = ft_strdup(">>");
				i += 2;
				continue;
			}
			else if (c == '<' && cmd_line[i + 1] == '<')
			{
				tokens[k++] = ft_strdup("<<");
				i += 2;
				continue;
			}
			else
			{
				op = extract_word(NULL, c);
				tokens[k++] = ft_strdup(op);
				i++;
				continue;
			}
		}
		if (!is_single_quote && !is_double_quote && c == ' ')
		{
			if (buffer)
			{
				tokens[k++] = buffer;
				buffer = NULL;
			}
			i++;
			continue;
		}
		buffer = extract_word(buffer, c);
		i++;
	}
	if (buffer)
		tokens[k++] = buffer;
	tokens[k] = NULL;
	return tokens;
}

char *join_cmd_args(char *cmd_line, t_list *environment)
{
	char **p;
	char *arg = NULL;
	size_t i;
	p = get_tokens_with_redirection(cmd_line);
	if (!p)
		return (NULL);
	arg = ft_strjoin(p[0], " ");
	i = 1;
	while (p[i])
	{
		if ((ft_strlen(p[i]) == 1 && !ft_strncmp(p[i], ">", 1)) || (ft_strlen(p[i]) == 2 && !ft_strncmp(p[i], ">>", 2)) || (ft_strlen(p[i]) == 1 && !ft_strncmp(p[i], "<", 1)) || (ft_strlen(p[i]) == 2 && !ft_strncmp(p[i], "<<", 2)))
		{
			if (p[i + 1])
				i += 2;
			else
				i++;
		}
		else
		{
			arg = ft_strjoin(arg, p[i]);
			arg = ft_strjoin(arg, " ");
			i++;
		}
	}
	return (arg);
}

char *join_cmd_redirections(char *cmd_line, t_list *environment)
{
	char **p;
	char *red = NULL;
	size_t i;
	p = get_tokens_with_redirection(cmd_line);
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
