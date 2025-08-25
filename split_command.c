/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 22:26:03 by youbella          #+#    #+#             */
/*   Updated: 2025/08/25 23:32:46 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

short	is_unclose_quotes(size_t single_quote, size_t double_quotes)
{
	if (single_quote % 2 || double_quotes % 2)
	{
		ft_putstr_fd("\033[34mminishell: \033[31msyntax error:", 2);
		ft_putstr_fd("\033[0m unclosed quote.\n", 2);
		ft_status(258, 1);
		return (1);
	}
	return (0);
}

char	*extract_word(char *str, char c)
{
	size_t	len_str;
	char	*p;

	len_str = 0;
	if (str)
		len_str = ft_strlen(str);
	p = ft_calloc(len_str + 2, 1);
	if (!p)
		return (NULL);
	if (str)
		ft_strlcpy(p, str, len_str + 2);
	p[len_str] = c;
	free(str);
	return (p);
}

static size_t	count_words_with_quotes(const char *s, char c,
										short is_single_quote, short is_space)
{
	size_t	i;
	size_t	count;
	short	is_double_quote;

	1 && (is_double_quote = 0, i = 0, count = 0);
	while (s[i])
	{
		while ((s[i] == c || (is_space && s[i] == '\t'))
			&& !is_single_quote && !is_double_quote)
			i++;
		if (s[i] != '\0')
			count++;
		while (s[i])
		{
			if (s[i] == 39 && !is_double_quote)
				is_single_quote = !is_single_quote;
			else if (s[i] == '"' && !is_single_quote)
				is_double_quote = !is_double_quote;
			else if ((s[i] == c || (is_space && s[i] == '\t'))
				&& !is_single_quote && !is_double_quote)
				break ;
			i++;
		}
	}
	return (count);
}

void	split_word_with_quotes(char c, short *is_c, t_ft_var *split_var)
{
	short	is_space;

	is_space = 0;
	if (c == ' ')
		is_space = 1;
	*is_c = 1;
	if (split_var->buffer)
	{
		if (!split_var->is_dollar)
			split_var->tokens[split_var->j] = split_var->buffer;
		else
		{
			split_var->tokens[split_var->j] = ft_dollar(split_var->buffer,
					split_var->variables, 0, is_space);
			free(split_var->buffer);
		}
		split_var->buffer = NULL;
		split_var->j++;
	}
}

void	loop_split_with_quotes(char *command, char c,
								short is_c, t_ft_var *split_var)
{
	size_t	i;

	1 && (i = 0, split_var->is_d_quote = 0, split_var->is_s_quote = 0);
	1 && (split_var->c_double_quotes = 0, split_var->c_single_quote = 0);
	while (command[i])
	{
		is_c = 0;
		if (command[i] == 39 && !split_var->is_d_quote)
		{
			split_var->is_s_quote = !split_var->is_s_quote;
			split_var->c_single_quote++;
		}
		else if (command[i] == '"' && !split_var->is_s_quote)
		{
			split_var->is_d_quote = !split_var->is_d_quote;
			split_var->c_double_quotes++;
		}
		else if ((command[i] == c
				|| (split_var->is_space && command[i] == '\t'))
			&& !split_var->is_d_quote && !split_var->is_s_quote)
			split_word_with_quotes(c, &is_c, split_var);
		if (!is_c)
			split_var->buffer = extract_word(split_var->buffer, command[i]);
		i++;
	}
}

short	finishe_split_with_quotes(t_ft_var *split_var,
									t_var *variables, short is_dollar)
{
	if (split_var->buffer)
	{
		if (!is_dollar)
			split_var->tokens[split_var->j] = split_var->buffer;
		else
		{
			split_var->tokens[split_var->j] = ft_dollar(split_var->buffer,
					variables, 0, 0);
			free(split_var->buffer);
		}
	}
	if (is_unclose_quotes(split_var->c_single_quote,
			split_var->c_double_quotes))
	{
		split_var->j = 0;
		while (split_var->tokens[split_var->j])
			free(split_var->tokens[split_var->j++]);
		free(split_var->tokens);
		return (0);
	}
	return (1);
}

char	**split_commmand_with_quotes(char *command, char c,
										short is_dollar, t_var *variables)
{
	t_ft_var	*split_var;

	if (!command)
		return (NULL);
	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (NULL);
	split_var->is_space = 0;
	if (c == ' ')
		split_var->is_space = 1;
	split_var->buffer = NULL;
	split_var->variables = variables;
	split_var->is_dollar = is_dollar;
	split_var->tokens = ft_calloc(count_words_with_quotes(command, c, 0, 0) + 1,
			sizeof(char *));
	split_var->j = 0;
	if (!split_var->tokens)
		return (NULL);
	loop_split_with_quotes(command, c, 0, split_var);
	if (!finishe_split_with_quotes(split_var, variables, is_dollar))
		return (NULL);
	return (split_var->tokens);
}

static size_t	count_words(char **tokens)
{
	size_t	i;
	size_t	count;
	size_t	j;
	short	is_single_quote;
	short	is_double_quote;

	1 && (i = 0, count = 0);
	while (tokens[i])
	{
		1 && (j = 0, is_single_quote = 0, is_double_quote = 0);
		while (tokens[i][j])
		{
			if (tokens[i][j] == 39 && !is_double_quote)
				is_single_quote = !is_single_quote;
			else if (tokens[i][j] == '"' && !is_single_quote)
				is_double_quote = !is_double_quote;
			else if ((tokens[i][j] == ' ' || tokens[i][j] == '\t')
				&& !is_single_quote && !is_double_quote)
				count++;
			j++;
		}
		count++;
		i++;
	}
	return (count);
}

short	check_tokens(size_t *k, size_t *i, size_t *j, t_ft_var *split_var)
{
	if (split_var->tokens[*i][*j] == 39 && !split_var->is_d_quote)
		return (split_var->is_s_quote = !split_var->is_s_quote, (*j)++, 0);
	else if (split_var->tokens[*i][*j] == '"' && !split_var->is_s_quote)
		return (split_var->is_d_quote = !split_var->is_d_quote, (*j)++, 0);
	else if ((split_var->tokens[*i][*j] == ' '
		|| split_var->tokens[*i][*j] == '\t')
		&& !split_var->is_s_quote && !split_var->is_d_quote)
	{
		if (split_var->buffer)
		{
			split_var->new_tokens[(*k)++] = split_var->buffer;
			split_var->buffer = NULL;
		}
		while ((split_var->tokens[*i][*j] == ' '
			|| split_var->tokens[*i][*j] == '\t'))
			(*j)++;
		return (0);
	}
	return (1);
}

void	loop_splitcmd(size_t k, t_ft_var *split_var)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (split_var->tokens[i])
	{
		1 && (j = 0, split_var->is_s_quote = 0, split_var->is_d_quote = 0);
		split_var->buffer = NULL;
		while (split_var->tokens[i][j])
		{
			if (!check_tokens(&k, &i, &j, split_var))
				continue ;
			split_var->buffer = extract_word(split_var->buffer,
					split_var->tokens[i][j]);
			j++;
		}
		if (split_var->buffer)
			split_var->new_tokens[k++] = split_var->buffer;
		else
			split_var->new_tokens[k++] = ft_strdup("");
		i++;
	}
	split_var->new_tokens[k] = NULL;
}

char	**split_command(char *cmd_line, short is_dollar, t_var *variables)
{
	t_ft_var	*split_var;
	size_t		i;
	char		**tokens;
	char		**new_tokens;

	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (NULL);
	i = 0;
	tokens = split_commmand_with_quotes(cmd_line, ' ', is_dollar, variables);
	if (!tokens)
		return (NULL);
	new_tokens = ft_calloc(count_words(tokens) + 1, sizeof(char *));
	if (!new_tokens)
		return (NULL);
	split_var->tokens = tokens;
	split_var->new_tokens = new_tokens;
	loop_splitcmd(0, split_var);
	i = 0;
	while (tokens[i])
		free(tokens[i++]);
	free(tokens);
	return (new_tokens);
}
