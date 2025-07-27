/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_first_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/07/27 08:50:26 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_getenv(char *var, t_list *export_list);

static size_t	count_words(const char *str, char c)
{
	size_t	i;
	size_t	words;
	short	is_word;
	short	is_double_quote;
	short	is_single_quote;

	(1) && (words = 0, is_word = 0, i = 0);
	is_double_quote = 0;
	is_single_quote = 0;
	while (str[i])
	{
		if (str[i] == 39 && !is_double_quote)
			is_single_quote = !is_single_quote;
		else if (str[i] == '"' && !is_single_quote)
			is_double_quote = !is_double_quote;
		else if (str[i] == c && !is_double_quote && !is_single_quote)
			is_word = 0;
		else if (!is_word)
		{
			words++;
			is_word = 1;
		}
		i++;
	}
	return (words);
}

static char	*extract_word(char *str, char c)
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

int	is_unclose_quotes(size_t single_quote, size_t double_quotes)
{
	if (single_quote % 2 != 0 || double_quotes % 2 != 0)
	{
		printf(RED "syntax error:%s unclosed quote.\n", DEF);
		return (1);
	}
	return (0);
}

size_t	len_str(char *str, int status, t_list *export_list)
{
	size_t	i;
	size_t	j;
	size_t	s;
	size_t	len;
	char	*env;
	char	*var;

	i = 0;
	s = 0;
	len = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] == '?')
		{
			var = ft_itoa(status);
			len += ft_strlen(var);
			free(var);
			i += 2;
		}
		else if (str[i] == '$' && !ft_isalpha(str[i + 1]) && str[i + 1] != '_')
		{
			len++;
			i++;
		}
		else if (str[i] == '$')
		{
			j = 0;
			i++;
			s = i;
			while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
			{
				i++;
				j++;
			}
			var = ft_substr(str, s, j);
			env = ft_getenv(var, export_list);
			if (!env)
				len += 0;
			else
				len += ft_strlen(env);
			free(var);
			free(env);
		}
		else
		{
			len++;
			i++;
		}
	}
	return (len);
}

char	*ft_dollar(char *str, int status, t_list *export_list)
{
	char	*env;
	char	*var;
	char	*alloc;
	size_t	len;
	size_t	i;
	size_t	k;
	size_t	l;
	size_t	s;
	size_t	len_var;

	i = 0;
	k = 0;
	l = 0;
	s = 0;
	len_var = 0;
	len = len_str(str, status, export_list);
	alloc = malloc(len + 1);
	if (!alloc)
		return (NULL);
	while (str[i])
	{
		l = 0;
		len_var = 0;
		if (str[i] == '$' && str[i + 1] == '?')
		{
			env = ft_itoa(status);
			while (env[l])
			{
				alloc[k] = env[l];
				k++;
				l++;
			}
			free(env);
			i += 2;
		}
		else if (str[i] == '$' && !ft_isalpha(str[i + 1]) && str[i + 1] != '_')
		{
			alloc[k++] = str[i];
			i++;
		}
		else if (str[i] == '$' && str[i + 1] != '?')
		{
			i++;
			s = i;
			while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
			{
				len_var++;
				i++;
			}
			var = ft_substr(str, s, len_var);
			env = ft_getenv(var, export_list);
			free(var);
			if (env)
			{
				while (env[l])
				{
					alloc[k] = env[l];
					k++;
					l++;
				}
			}
			free(env);
		}
		else
			alloc[k++] = str[i++];
	}
	alloc[k] = 0;
	return (alloc);
}

char	**ft_split_first_cmd(char const *s, char c, int status, t_list *export_list)
{
	char	**p;
	char	*buffer;
	size_t	i;
	size_t	j;
	size_t	count_single_quote;
	size_t	count_double_quotes;
	short	is_single_quote;
	short	is_double_quote;
	short	check_single_quote;

	i = 0;
	j = 0;
	is_double_quote = 0;
	is_single_quote = 0;
	count_single_quote = 0;
	count_double_quotes = 0;
	check_single_quote = 0;
	buffer = NULL;
	p = ft_calloc(count_words(s, c) + 1, sizeof(char *));
	if (!p)
		return (NULL);
	while (s[i])
	{
		if (s[i] == ';' || s[i] == '\\')
		{
			printf(RED "syntax error:%s imposter char.\n", DEF);
			return (NULL);
		}
		if (s[i] == 39 && !is_double_quote)
		{
			is_single_quote = !is_single_quote;
			check_single_quote++;
			count_single_quote++;
		}
		else if (s[i] == '"' && !is_single_quote)
		{
			is_double_quote = !is_double_quote;
			count_double_quotes++;
		}
		else if (s[i] == c && !is_double_quote && !is_single_quote)
		{
			if (buffer)
			{
				if (check_single_quote == 2)
				{
					p[j] = buffer;
					check_single_quote = 0;
				}
				else
					p[j] = ft_dollar(buffer, status, export_list);
				buffer = NULL;
				j++;
			}
		}
		else
			buffer = extract_word(buffer, s[i]);
		i++;
	}
	if (buffer)
	{
		if (check_single_quote == 2)
			p[j] = buffer;
		else
			p[j] = ft_dollar(buffer, status, export_list);
	}
	if (is_unclose_quotes(count_single_quote, count_double_quotes))
		return (NULL);
	return (p);
}

void	free_split(char **p)
{
	size_t	i;

	i = 0;
	while (p[i])
	{
		free(p[i]);
		i++;
	}
	free(p);
}
