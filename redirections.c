/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/23 04:14:01 by youbella         ###   ########.fr       */
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

short	is_exist_redirect_pipe(char *cmd_line, char redirect_pipe)
{
	size_t	i;
	char	quot;

	i = 0;
	while (cmd_line[i])
	{
		if (cmd_line[i] == '"' || cmd_line[i] == 39)
		{
			quot = cmd_line[i];
			i++;
			while (cmd_line[i] && cmd_line[i] != quot)
				i++;
			if (cmd_line[i] == quot)
				i++;
		}
		if (!cmd_line[i])
			break ;
		if ((redirect_pipe == 'r' && cmd_line[i] == '>')
			|| (redirect_pipe == 'r' && cmd_line[i] == '<'
				&& cmd_line[i + 1] != '<' && cmd_line[i - 1] != '<')
			|| (redirect_pipe == 'r' && cmd_line[i] == '<'
				&& cmd_line[i + 1] == '<')
			|| (redirect_pipe == '|' && cmd_line[i] == '|'))
			return (1);
		i++;
	}
	return (0);
}

short	is_ambiguous_redirect(char *token, t_var *variables)
{
	size_t	i;
	size_t	j;
	size_t	start;
	size_t	var_len;
	char	*var_name;
	char	*var_value;
	char	quot;

	i = 0;
	var_value = NULL;
	while (token[i])
	{
		var_len = 0;
		j = 0;
		if (token[i] == 39 || token[i] == '"')
		{
			quot = token[i];
			i++;
			while (token[i] && token[i] != quot)
				i++;
			if (token[i] == quot)
				i++;
		}
		if (!token[i])
			break ;
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
			var_value = ft_dollar(var_name, variables);
			free(var_name);
			while (var_value[j])
			{
				if (var_value[j] == ' ')
					return (free(var_value), 1);
				j++;
			}
		}
		else
			i++;
	}
	return (free(var_value), 0);
}

t_redirections	*list_redirections(char **tokens, t_var *variables)
{
	size_t			i;
	t_redirections	*list;
	t_redirections	*new_node;
	char			**file_name;

	i = 0;
	list = NULL;
	while (tokens[i])
	{
		if ((ft_strlen(tokens[i]) == 2 && ((!ft_strncmp(tokens[i], ">>", 2)) || (!ft_strncmp(tokens[i], "<<", 2)))) || ((ft_strlen(tokens[i]) == 1) && ((!ft_strncmp(tokens[i], ">", 1)) || (!ft_strncmp(tokens[i], "<", 1)))))
		{
			i++;
			if ((!tokens[i] || (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], ">>", 2)) || !ft_strncmp(tokens[i], "<<", 2)) || (!tokens[i] || (ft_strlen(tokens[i]) == 1 && (!ft_strncmp(tokens[i], ">", 1) || !ft_strncmp(tokens[i], "<", 1)))))
			{
				write(2, "\033[34mminishell:\033[31m syntax error in redirection.\n\033[0m", ft_strlen("\033[34mminishell:\033[31m syntax error in redirection.\n\033[0m"));
				ft_status(258, 1);
				return (NULL);
			}
		}
		else
		{
			i++;
			continue ;
		}
		if (ft_strlen(tokens[i - 1]) == 2
			&& !ft_strncmp(tokens[i - 1], "<<", 2))
		{
			file_name = split_command(tokens[i], ' ', 0, variables);
			new_node = craete_new_node(tokens[i - 1], file_name[0]);
			free(file_name);
		}
		else if (is_ambiguous_redirect(tokens[i], variables))
		{
			tokens[i] = ft_strdup("");
			new_node = craete_new_node(tokens[i - 1], tokens[i]);
		}
		else
		{
			file_name = split_command(tokens[i], ' ', 1, variables);
			new_node = craete_new_node(tokens[i - 1], file_name[0]);
			free(file_name);
		}
		add_node_in_back(&list, new_node);
		i++;
	}
	return (list);
}

size_t	count_tokens_with_redirection(const char *cmd_line)
{
	size_t	i;
	size_t	count;
	char	c;
	short	is_single_quote;
	short	is_double_quote;
	short	in_token;

	i = 0;
	count = 0;
	is_single_quote = 0;
	is_double_quote = 0;
	in_token = 0;
	while (cmd_line[i])
	{
		c = cmd_line[i];
		if (c == 39 && !is_double_quote)
		{
			is_single_quote = !is_single_quote;
			in_token = 1;
			i++;
			continue ;
		}
		if (c == '"' && !is_single_quote)
		{
			is_double_quote = !is_double_quote;
			in_token = 1;
			i++;
			continue ;
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
				continue ;
			}
			if (c == '>' || c == '<')
			{
				if (in_token)
				{
					count++;
					in_token = 0;
				}
				if ((c == '>' && cmd_line[i + 1] == '>')
					|| (c == '<' && cmd_line[i + 1] == '<'))
					i += 2;
				else
					i++;
				count++;
				continue ;
			}
		}
		in_token = 1;
		i++;
	}
	if (in_token)
		count++;
	return (count);
}

char	**get_tokens_with_redirection(const char *cmd_line)
{
	size_t	i;
	size_t	k;
	char	c;
	char	*op;
	short	is_single_quote;
	short	is_double_quote;
	char	*buffer;
	char	**tokens;

	i = 0;
	k = 0;
	is_single_quote = 0;
	is_double_quote = 0;
	buffer = NULL;
	tokens = malloc((count_tokens_with_redirection(cmd_line) + 1)
			* sizeof(char *));
	if (!tokens)
		return (NULL);
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
				continue ;
			}
			else if (c == '<' && cmd_line[i + 1] == '<')
			{
				tokens[k++] = ft_strdup("<<");
				i += 2;
				continue ;
			}
			else
			{
				op = extract_word(NULL, c);
				tokens[k++] = op;
				i++;
				continue ;
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
			continue ;
		}
		buffer = extract_word(buffer, c);
		i++;
	}
	if (buffer)
		tokens[k++] = buffer;
	tokens[k] = NULL;
	return (tokens);
}

char	*join_cmd_args(char *cmd_line)
{
	size_t	i;
	char	*cmd_arg;
	char	**tokens;
	char	*tmp;

	i = 1;
	cmd_arg = NULL;
	tokens = get_tokens_with_redirection(cmd_line);
	if (!tokens)
		return (NULL);
	cmd_arg = ft_strjoin(tokens[0], " ");
	while (tokens[i])
	{
		if ((ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], ">", 1))
			|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], ">>", 2))
			|| (ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], "<", 1))
			|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], "<<", 2)))
		{
			if (tokens[i + 1])
				i += 2;
			else
				i++;
		}
		else
		{
			tmp = cmd_arg;
			cmd_arg = ft_strjoin(cmd_arg, tokens[i]);
			free(tmp);
			tmp = cmd_arg;
			cmd_arg = ft_strjoin(cmd_arg, " ");
			free(tmp);
			i++;
		}
	}
	i = 0;
	while (tokens[i])
		free(tokens[i++]);
	free(tokens);
	return (cmd_arg);
}
