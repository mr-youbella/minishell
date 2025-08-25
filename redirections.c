/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/25 23:35:49 by youbella         ###   ########.fr       */
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
			|| (redirect_pipe == 'r' && cmd_line[i] == '<')
			|| (redirect_pipe == '|' && cmd_line[i] == '|'))
			return (1);
		i++;
	}
	return (0);
}

short	check_ambiguous_dollar(char *str, t_var *variables, t_ft_var *var)
{
	size_t	start;
	size_t	var_len;
	size_t	j;
	char	*var_name;
	char	*var_value;

	var_value = NULL;
	if (str[var->j] == '$')
	{
		1 && (j = 0, var_len = 0, start = var->j, var->j++);
		while ((str[var->j]) && (ft_isalnum(str[var->j]) || str[var->j] == '_'))
			1 && (var_len++, var->j++);
		var_name = ft_substr(str, start, var_len + 1);
		var_value = ft_dollar(var_name, variables, 0, 0);
		free(var_name);
		while (var_value[j])
		{
			if (var_value[j] == ' ')
				return (free(var_value), 1);
			j++;
		}
	}
	else
		var->j++;
	return (free(var_value), 0);
}

short	is_ambiguous_redirect(char *token, t_var *variables)
{
	t_ft_var	*var;
	char		quot;

	var = malloc(sizeof(t_ft_var));
	if (!var)
		return (0);
	var->j = 0;
	while (token[var->j])
	{
		if (token[var->j] == 39 || token[var->j] == '"')
		{
			quot = token[var->j];
			var->j++;
			while (token[var->j] && token[var->j] != quot)
				var->j++;
			if (token[var->j] == quot)
				var->j++;
		}
		if (!token[var->j])
			break ;
		if (check_ambiguous_dollar(token, variables, var))
			return (1);
	}
	return (0);
}

short	check_syntax_redirect(char **tokens, t_ft_var *list_var)
{
	if ((ft_strlen(tokens[list_var->j]) == 2
			&& ((!ft_strncmp(tokens[list_var->j], ">>", 2))
				|| (!ft_strncmp(tokens[list_var->j], "<<", 2))))
		|| ((ft_strlen(tokens[list_var->j]) == 1)
			&& ((!ft_strncmp(tokens[list_var->j], ">", 1))
				|| (!ft_strncmp(tokens[list_var->j], "<", 1)))))
	{
		list_var->j++;
		if ((!tokens[list_var->j] || (ft_strlen(tokens[list_var->j]) == 2
					&& !ft_strncmp(tokens[list_var->j], ">>", 2))
				|| !ft_strncmp(tokens[list_var->j], "<<", 2))
			|| (!tokens[list_var->j] || (ft_strlen(tokens[list_var->j]) == 1
					&& (!ft_strncmp(tokens[list_var->j], ">", 1)
						|| !ft_strncmp(tokens[list_var->j], "<", 1)))))
		{
			ft_putstr_fd("\033[34mminishell:\033[31m ", 2);
			ft_putstr_fd("syntax error in redirection.\n\033[0m", 2);
			return (ft_status(258, 1), -1);
		}
	}
	else
		return (list_var->j++, 0);
	return (1);
}

void	add_redirections(char **tokens, t_ft_var *list_var,
							t_redirections **new_node, t_var *variables)
{
	char	**file_name;

	if (ft_strlen(tokens[list_var->j - 1]) == 2
		&& !ft_strncmp(tokens[list_var->j - 1], "<<", 2))
	{
		file_name = split_command(tokens[list_var->j], 0, variables);
		*new_node = craete_new_node(tokens[list_var->j - 1], file_name[0]);
		free(file_name);
	}
	else if (is_ambiguous_redirect(tokens[list_var->j], variables))
	{
		tokens[list_var->j] = ft_strdup("");
		*new_node = craete_new_node(tokens[list_var->j - 1],
				tokens[list_var->j]);
	}
	else
	{
		file_name = split_command(tokens[list_var->j], 1, variables);
		*new_node = craete_new_node(tokens[list_var->j - 1], file_name[0]);
		free(file_name);
	}
}

t_redirections	*list_redirections(char **tokens, t_var *variables)
{
	t_redirections	*list;
	t_redirections	*new_node;
	t_ft_var		*list_var;
	short			return_val;

	list_var = malloc(sizeof(t_ft_var));
	if (!list_var)
		return (NULL);
	1 && (list_var->j = 0, list = NULL);
	while (tokens[list_var->j])
	{
		return_val = check_syntax_redirect(tokens, list_var);
		if (!return_val)
			continue ;
		if (return_val == -1)
			return (NULL);
		add_redirections(tokens, list_var, &new_node, variables);
		add_node_in_back(&list, new_node);
		list_var->j++;
	}
	return (list);
}

short	add_count_with_redirection(char *cmd_line, char c, size_t *count,
									t_ft_var *split_var)
{
	if (!split_var->is_s_quote && !split_var->is_d_quote)
	{
		if (c == ' ')
		{
			if (split_var->in_token)
				1 && ((*count)++, split_var->in_token = 0);
			return (split_var->j++, 0);
		}
		if (c == '>' || c == '<')
		{
			if (split_var->in_token)
			{
				(*count)++;
				split_var->in_token = 0;
			}
			if ((c == '>' && cmd_line[split_var->j + 1] == '>')
				|| (c == '<' && cmd_line[split_var->j + 1] == '<'))
				split_var->j += 2;
			else
				split_var->j++;
			return ((*count)++, 0);
		}
	}
	return (1);
}

short	check_quote(char c, t_ft_var *split_var)
{
	if (c == 39 && !split_var->is_d_quote)
	{
		split_var->is_s_quote = !split_var->is_s_quote;
		split_var->in_token = 1;
		split_var->j++;
		return (0);
	}
	if (c == '"' && !split_var->is_s_quote)
	{
		split_var->is_d_quote = !split_var->is_d_quote;
		split_var->in_token = 1;
		split_var->j++;
		return (0);
	}
	return (1);
}

size_t	count_tokens_with_redirection(char *cmd_line)
{
	size_t		count;
	char		c;
	t_ft_var	*split_var;

	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (0);
	1 && (split_var->j = 0, count = 0, split_var->is_s_quote = 0);
	1 && (split_var->is_s_quote = 0, split_var->is_d_quote = 0);
	split_var->in_token = 0;
	while (cmd_line[split_var->j])
	{
		c = cmd_line[split_var->j];
		if (!check_quote(c, split_var))
			continue ;
		if (!add_count_with_redirection(cmd_line, c, &count, split_var))
			continue ;
		split_var->in_token = 1;
		split_var->j++;
	}
	if (split_var->in_token)
		count++;
	return (count);
}

short	get_redirections(char *cmd_line, char c, size_t *i, t_ft_var *sv)
{
	char	*op;

	if (!sv->is_s_quote && !sv->is_d_quote && (c == '>' || c == '<'))
	{
		if (sv->buffer)
		{
			1 && (sv->tokens[sv->j++] = sv->buffer, sv->buffer = NULL);
		}
		if (c == '>' && cmd_line[*i + 1] == '>')
		{
			sv->tokens[sv->j++] = ft_strdup(">>");
			return ((*i) += 2, 0);
		}
		else if (c == '<' && cmd_line[*i + 1] == '<')
		{
			sv->tokens[sv->j++] = ft_strdup("<<");
			return ((*i) += 2, 0);
		}
		else
		{
			1 && (op = extract_word(NULL, c), sv->tokens[sv->j++] = op);
			return ((*i)++, 0);
		}
	}
	return (1);
}

void	loop_tokens_wr(char *cmd_line, t_ft_var *sv)
{
	size_t	i;
	char	c;

	1 && (sv->is_s_quote = 0, sv->is_d_quote = 0);
	i = 0;
	while (cmd_line[i])
	{
		c = cmd_line[i];
		if (c == 39 && !sv->is_d_quote)
			sv->is_s_quote = !sv->is_s_quote;
		else if (c == '"' && !sv->is_s_quote)
			sv->is_d_quote = !sv->is_d_quote;
		if (!get_redirections(cmd_line, c, &i, sv))
			continue ;
		if (!sv->is_s_quote && !sv->is_d_quote && c == ' ')
		{
			if (sv->buffer)
				1 && (sv->tokens[sv->j++] = sv->buffer, sv->buffer = NULL);
			i++;
			continue ;
		}
		sv->buffer = extract_word(sv->buffer, c);
		i++;
	}
}

char	**get_tokens_with_redirection(char *cmd_line)
{
	char		**tokens;
	t_ft_var	*split_var;

	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (NULL);
	split_var->j = 0;
	split_var->buffer = NULL;
	tokens = malloc((count_tokens_with_redirection(cmd_line) + 1)
			* sizeof(char *));
	if (!tokens)
		return (NULL);
	split_var->tokens = tokens;
	loop_tokens_wr(cmd_line, split_var);
	if (split_var->buffer)
		tokens[split_var->j++] = split_var->buffer;
	tokens[split_var->j] = NULL;
	return (tokens);
}

char	*get_command(char **tokens, size_t i, char *tmp, char *cmd_arg)
{
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
	return (cmd_arg);
}

char	*join_cmd_args(char *cmd_line)
{
	size_t	i;
	char	**tokens;
	char	*cmd_arg;

	i = 0;
	tokens = get_tokens_with_redirection(cmd_line);
	if (!tokens)
		return (NULL);
	if ((ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], ">", 1))
		|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], ">>", 2))
		|| (ft_strlen(tokens[i]) == 1 && !ft_strncmp(tokens[i], "<", 1))
		|| (ft_strlen(tokens[i]) == 2 && !ft_strncmp(tokens[i], "<<", 2)))
	{
		i++;
		if (tokens[i])
			i++;
	}
	cmd_arg = get_command(tokens, i, NULL, NULL);
	i = 0;
	while (tokens[i])
		free(tokens[i++]);
	free(tokens);
	return (cmd_arg);
}
