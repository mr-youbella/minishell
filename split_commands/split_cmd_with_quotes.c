/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_cmd_with_quotes.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:00:35 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 23:51:01 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static void	split_word_with_quotes(char c, short *is_c, t_ft_var *split_var)
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
					split_var->variables, is_space);
			free(split_var->buffer);
		}
		split_var->buffer = NULL;
		split_var->j++;
	}
}

static void	loop_split_with_quotes(char *command, char c,
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

static short	finishe_split_with_quotes(t_ft_var *split_var,
									t_var *variables, short is_dollar)
{
	if (split_var->buffer)
	{
		if (!is_dollar)
			split_var->tokens[split_var->j] = split_var->buffer;
		else
		{
			split_var->tokens[split_var->j] = ft_dollar(split_var->buffer,
					variables, 0);
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
	char		**tokens;

	if (!command)
		return (NULL);
	split_var = malloc(sizeof(t_ft_var));
	if (!split_var)
		return (ft_status(1, 1), NULL);
	ft_memset(split_var, 0, sizeof(t_ft_var));
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
		return (free(split_var), NULL);
	loop_split_with_quotes(command, c, 0, split_var);
	if (!finishe_split_with_quotes(split_var, variables, is_dollar))
		return (free(split_var), NULL);
	tokens = split_var->tokens;
	return (free(split_var), tokens);
}
