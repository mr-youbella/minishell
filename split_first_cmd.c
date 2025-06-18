/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_first_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/18 04:16:00 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		else if(str[i] == '"' && !is_single_quote)
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

static char	*extract_word(const char *str, char c)
{
	size_t	len_str;
	size_t	quotes;
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
	return (p);
}

int	is_unclose_quotes(size_t single_quote, size_t double_quotes)
{
	size_t	i;
	
	i = 0;
	if (single_quote % 2 != 0 || double_quotes % 2 != 0)
	{
		printf(RED "syntax error:%s unclosed quote.\n", DEF);
		return (1);
	}
	return (0);
}

int search_and_replace(char *str)
{
	size_t	i;
	size_t	len;
	int		status;
	char	*int_to_str;

	i = 0;
	status = 137;
	int_to_str = ft_itoa(status);
	len = ft_strlen(str);
	while(str[i])
	{
		if (str[i] == '$' && str[i + 1] == '?')
		{
			printf("%s\n", int_to_str);
		}
		i++;
	}
	return (1);
}


char	**ft_split_first_cmd(char const *s, char c)
{
	char	**p;
	char	*buffer;
	size_t	i;
	size_t	j;
	size_t	count_single_quote;
	size_t	count_double_quotes;
	short	is_single_quote;
	short	is_double_quote;
	
	i = 0;
	j = 0;
	is_double_quote = 0;
	is_single_quote = 0;
	count_single_quote = 0;
	count_double_quotes = 0;
	buffer = NULL;
	p =	ft_calloc(count_words(s, c) + 1, sizeof(char *));
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
			count_single_quote++;
		}
		else if(s[i] == '"' && !is_single_quote)
		{
			is_double_quote = !is_double_quote;
			count_double_quotes++;
		}
		else if (s[i] == c && !is_double_quote && !is_single_quote)
		{
			if (buffer)
			{
				p[j] = buffer;
				buffer = NULL;
				j++;
			}
		}
		else
			buffer = extract_word(buffer, s[i]);
		i++;
	}
	if (buffer)
		p[j] = buffer;
	if (is_unclose_quotes(count_single_quote, count_double_quotes))
		return (NULL);
	return (p);
}
