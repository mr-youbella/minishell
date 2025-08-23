/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 22:26:03 by youbella          #+#    #+#             */
/*   Updated: 2025/08/23 05:38:39 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

short	is_unclose_quotes(size_t single_quote, size_t double_quotes)
{
	if (single_quote % 2 || double_quotes % 2)
	{
		write(2, "\033[34mminishell: \033[31msyntax error:\033[0m unclosed quote.\n", ft_strlen("\033[34mminishell: \033[31msyntax error:\033[0m unclosed quote.\n"));
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

static size_t	count_words_with_quotes(const char *s, char c)
{
	size_t	i;
	size_t	count;
	short	is_single_quote;
	short	is_double_quote;
	short	is_space;

	is_space = 0;
	if (c == ' ')
		is_space = 1;
	1 && (i = 0, count = 0);
	1 && (is_single_quote = 0, is_double_quote = 0);
	while (s[i])
	{
		while ((s[i] == c || (is_space && s[i] == '\t')) && !is_single_quote && !is_double_quote)
			i++;
		if (s[i] != '\0')
			count++;
		while (s[i])
		{
			if (s[i] == 39 && !is_double_quote)
				is_single_quote = !is_single_quote;
			else if (s[i] == '"' && !is_single_quote)
				is_double_quote = !is_double_quote;
			else if ((s[i] == c || (is_space && s[i] == '\t')) && !is_single_quote && !is_double_quote)
				break ;
			i++;
		}
	}
	return (count);
}

char	**split_commmand_with_quotes(char *command, char c, short is_dollar, t_var *variables)
{
	char	**tokens;
	char	*buffer;
	size_t	i;
	size_t	j;
	size_t	count_single_quote;
	size_t	count_double_quotes;
	short	is_single_quote;
	short	is_double_quote;
	short	is_c;
	short	is_space;

	is_space = 0;
	if (c == ' ')
		is_space = 1;
	i = 0;
	j = 0;
	is_double_quote = 0;
	is_single_quote = 0;
	count_single_quote = 0;
	count_double_quotes = 0;
	buffer = NULL;
	is_c = 0;
	tokens = ft_calloc(count_words_with_quotes(command, c) + 1, sizeof(char *));
	if (!tokens)
		return (NULL);
	while (command[i])
	{
		is_c = 0;
		if (command[i] == 39 && !is_double_quote)
		{
			is_single_quote = !is_single_quote;
			count_single_quote++;
		}
		else if (command[i] == '"' && !is_single_quote)
		{
			is_double_quote = !is_double_quote;
			count_double_quotes++;
		}
		else if ((command[i] == c || (is_space && command[i] == '\t'))&& !is_double_quote && !is_single_quote)
		{
			is_c = 1;
			if (buffer)
			{
				if (!is_dollar)
					tokens[j] = buffer;
				else
				{
					tokens[j] = ft_dollar(buffer, variables);
					free(buffer);
				}
				buffer = NULL;
				j++;
			}
		}
		if (!is_c)
			buffer = extract_word(buffer, command[i]);
		i++;
	}
	if (buffer)
	{
		if (!is_dollar)
			tokens[j] = buffer;
		else
		{
			tokens[j] = ft_dollar(buffer, variables);
			free(buffer);
		}
	}
	if (is_unclose_quotes(count_single_quote, count_double_quotes))
	{
		i = 0;
		while (tokens[i])
			free(tokens[i++]);
		free(tokens);
		return (NULL);
	}
	return (tokens);
}

static size_t	count_words(char **tokens, char c)
{
	size_t	i;
	size_t	count;
	size_t	j;
	short	is_single_quote;
	short	is_double_quote;
	short	is_space;

	is_space = 0;
	if (c == ' ')
		is_space = 1;
	1 && (i = 0, count = 0);
	while (tokens[i])
	{
		j = 0;
		1 && (is_single_quote = 0, is_double_quote = 0);
		while (tokens[i][j])
		{
			if (tokens[i][j] == 39 && !is_double_quote)
				is_single_quote = !is_single_quote;
			else if (tokens[i][j] == '"' && !is_single_quote)
				is_double_quote = !is_double_quote;
			else if ((tokens[i][j] == c || (is_space && tokens[i][j] == '\t')) && !is_single_quote && !is_double_quote)
				count++;
			j++;
		}
		count++;
		i++;
	}
	return (count);
}

char	**split_command(char *cmd_line, char c, short is_dollar, t_var *variables)
{
	size_t	i;
	size_t	j;
	size_t	k;
	char	**tokens;
	char	**new_tokens;
	char	*buffer;
	short	is_single_quote;
	short	is_double_quote;
	short	is_space;

	i = 0;
	k = 0;
	is_space = 0;
	if (c == ' ')
		is_space = 1;
	tokens = split_commmand_with_quotes(cmd_line, c, is_dollar, variables);
	if (!tokens)
		return (NULL);
	new_tokens = ft_calloc(count_words(tokens, c) + 1, sizeof(char *));
	if (!new_tokens)
		return (NULL);
	while (tokens[i])
	{
		j = 0;
		is_single_quote = 0;
		is_double_quote = 0;
		buffer = NULL;
		while (tokens[i][j])
		{
			if (tokens[i][j] == 39 && !is_double_quote)
			{
				is_single_quote = !is_single_quote;
				j++;
				continue ;
			}
			else if (tokens[i][j] == '"' && !is_single_quote)
			{
				is_double_quote = !is_double_quote;
				j++;
				continue ;
			}
			else if ((tokens[i][j] == c || (is_space && tokens[i][j] == '\t')) && !is_single_quote && !is_double_quote)
			{
				if (buffer)
				{
					new_tokens[k++] = buffer;
					buffer = NULL;
				}
				while (tokens[i][j] == c || (is_space && tokens[i][j] == '\t'))
					j++;
				continue ;
			}
			buffer = extract_word(buffer, tokens[i][j]);
			j++;
		}
		if (buffer)
			new_tokens[k++] = buffer;
		else
			new_tokens[k++] = ft_strdup("");
		i++;
	}
	new_tokens[k] = NULL;
	i = 0;
	while (tokens[i])
		free(tokens[i++]);
	free(tokens);
	return (new_tokens);
}
