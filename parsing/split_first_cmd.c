/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_first_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/12 17:57:08 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	count_words(const char *s, char c)
{
	size_t	i;
	size_t	count;
	short	is_single_quote;
	short	is_double_quote;

	i = 0;
	count = 0;
	is_single_quote = 0;
	is_double_quote = 0;

	while (s[i])
	{
		while (s[i] == c && !is_single_quote && !is_double_quote)
			i++;
		if (s[i] != '\0')
			count++;
		while (s[i])
		{
			if (s[i] == 39 && !is_double_quote)
				is_single_quote = !is_single_quote;
			else if (s[i] == '"' && !is_single_quote)
				is_double_quote = !is_double_quote;
			else if (s[i] == c && !is_single_quote && !is_double_quote)
				break;
			i++;
		}
	}
	return (count);
}


static char *extract_word(char *str, char c)
{
	size_t len_str;
	char *p;

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

int is_unclose_quotes(size_t single_quote, size_t double_quotes)
{
	if (single_quote % 2 != 0 || double_quotes % 2 != 0)
	{
		printf(RED "syntax error:%s unclosed quote.\n", DEF);
		return (1);
	}
	return (0);
}

size_t len_str(char *str, int status, t_list *environment)
{
	size_t i;
	size_t len_var;
	size_t s;
	size_t len;
	char *env;
	char *var;
	char quote;

	i = 0;
	s = 0;
	len_var = 0;
	len = 0;
	while (str[i])
	{
		if ((str[i] == 39 || str[i] == '"'))
		{
			if (quote == 0)
				quote = str[i];
			else if (quote == str[i])
				quote = 0;
		}
		if (str[i] == '$')
		{
			if (str[i + 1] == '?' && quote != 39)
			{
				env = ft_itoa(status);
				free(env);
				i += 2;
				len += ft_strlen(env);
				continue;
			}
			else if (!ft_isalpha(str[i + 1]) && str[i + 1] != '_')
			{
				i++;
				len++;
				continue;
			}
			else if (quote != 39)
			{
				i++;
				s = i;
				len_var = 0;
				while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
				{
					len_var++;
					i++;
				}
				var = ft_substr(str, s, len_var);
				env = ft_getenv(var, environment);
				len += ft_strlen(env);
				free(var);
				free(env);
				continue;
			}
		}
		i++;
		len++;
	}
	return (len);
}

char *ft_dollar(char *str, int status, t_list *environment)
{
	char *env;
	char *var;
	char *alloc;
	size_t len;
	size_t i;
	size_t k;
	size_t s;
	size_t len_var;
	char quote;

	i = 0;
	k = 0;
	quote = 0;
	len = len_str(str, status, environment);
	alloc = malloc(len + 1);
	if (!alloc)
		return (NULL);
	while (str[i])
	{
		if ((str[i] == 39 || str[i] == '"'))
		{
			if (quote == 0)
				quote = str[i];
			else if (quote == str[i])
				quote = 0;
		}
		if (str[i] == '$')
		{
			if (str[i + 1] == '?' && quote != 39)
			{
				env = ft_itoa(status);
				for (size_t l = 0; env[l]; l++)
					alloc[k++] = env[l];
				free(env);
				i += 2;
				continue;
			}
			else if (!ft_isalpha(str[i + 1]) && str[i + 1] != '_')
			{
				alloc[k++] = str[i++];
				continue;
			}
			else if (quote != 39)
			{
				i++;
				s = i;
				len_var = 0;
				while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
				{
					len_var++;
					i++;
				}
				var = ft_substr(str, s, len_var);
				env = ft_getenv(var, environment);
				free(var);
				if (env)
				{
					for (size_t l = 0; env[l]; l++)
						alloc[k++] = env[l];
				}
				free(env);
				continue;
			}
		}

		alloc[k++] = str[i++];
	}
	alloc[k] = 0;
	return (alloc);
}

char **ft_split_first_cmd(char *s, char c, int status, t_list *environment)
{
	char **tokens;
	char *buffer;
	size_t i;
	size_t j;
	size_t count_single_quote;
	size_t count_double_quotes;
	short is_single_quote;
	short is_double_quote;
	short is_c;

	i = 0;
	j = 0;
	is_double_quote = 0;
	is_single_quote = 0;
	count_single_quote = 0;
	count_double_quotes = 0;
	buffer = NULL;
	is_c = 0;
	tokens = ft_calloc(count_words(s, c) + 1, sizeof(char *));
	if (!tokens)
		return (NULL);
	while (s[i])
	{
		is_c = 0;
		if (s[i] == 39 && !is_double_quote)
		{
			is_single_quote = !is_single_quote;
			count_single_quote++;
		}
		else if (s[i] == '"' && !is_single_quote)
		{
			is_double_quote = !is_double_quote;
			count_double_quotes++;
		}
		else if (s[i] == c && !is_double_quote && !is_single_quote)
		{
			is_c = 1;
			if (buffer)
			{
				tokens[j] = ft_dollar(buffer, status, environment);
				buffer = NULL;
				j++;
			}
		}
		if (!is_c)
			buffer = extract_word(buffer, s[i]);
		i++;
	}
	if (buffer)
		tokens[j] = ft_dollar(buffer, status, environment);
	if (is_unclose_quotes(count_single_quote, count_double_quotes))
		return (NULL);
	return (tokens);
}

char **ft_split_first_cmd2(char *cmd_line, char c, int status, t_list *environment)
{
	size_t i;
	size_t j;
	size_t k;
	char **tokens;
	char **new_tokens;
	char *buffer;
	short is_single_quote;
	short is_double_quote;

	i = 0;
	k = 0;
	is_single_quote = 0;
	is_double_quote = 0;
	tokens = ft_split_first_cmd(cmd_line, c, status, environment);
	new_tokens = ft_calloc((count_words(cmd_line, c)) + 1, sizeof(char *));
	if (!tokens || !new_tokens)
		return (NULL);
	while (tokens[i])
	{
		j = 0;
		buffer = NULL;
		while (tokens[i][j])
		{
			if (tokens[i][j] == 39 && !is_double_quote)
				is_single_quote = !is_single_quote;
			else if (tokens[i][j] == '"' && !is_single_quote)
				is_double_quote = !is_double_quote;
			else
				buffer = extract_word(buffer, tokens[i][j]);
			j++;
		}
		if (buffer)
			new_tokens[k++] = buffer;
		else
			new_tokens[k++] = ft_strdup("");
		i++;
	}
	return (new_tokens);
}

void free_split(char **p)
{
	size_t i;

	i = 0;
	while (p[i])
	{
		free(p[i]);
		i++;
	}
	free(p);
}
