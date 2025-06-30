/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_first_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkannouf <wkannouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/26 02:28:18 by wkannouf         ###   ########.fr       */
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
	if (single_quote % 2 != 0 || double_quotes % 2 != 0)
	{
		printf(RED "syntax error:%s unclosed quote.\n", DEF);
		return (1);
	}
	return (0);
}

char	*search_and_replace(const char *str, int status)
{
	size_t	i;
	size_t	j;
	size_t	k;
	size_t	len;
	size_t	n;
	size_t	new_len;
	short	exist;
	char	*int_to_str;
	char	*p;
	
	p = NULL;
	i = 0;
	j = 0;
	n = 0;
	k = 0;
	exist = 0;
	len = ft_strlen(str);
	new_len = 0;
	int_to_str = ft_itoa(status);
	while(str[i])
	{
		if (str[i] == '$' && str[i + 1] == '?')
		{
			n++;
			exist = 1;
		}
		i++;
	}
	i = 0;
	if (exist == 1)
	{
		new_len = len + n * (ft_strlen(int_to_str) - 2);
		p = malloc(new_len + 1);
		if (!p)
			return (NULL);
		while (str[i])
		{
			if (str[i] == '$' && str[i + 1] == '?')
			{
				k = 0;
				while(int_to_str[k])
					p[j++] = int_to_str[k++];
				i += 2;
			}
			else
				p[j++] = str[i++];
		}
		p[j] = 0;
	}
	
	else
		p = ft_strdup(str);
	return (p);
}

char	**ft_split_first_cmd(char const *s, char c, int status)
{
	char	**p;
	char	*buffer;
	size_t	i;
	size_t	j;
	size_t	count_single_quote;
	size_t	count_double_quotes;
	short	is_single_quote;
	short	is_double_quote;
	char *tmp;
	
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
				tmp = search_env(buffer);
				p[j] = search_and_replace(tmp, status);
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
		tmp = search_env(buffer);
		p[j] = search_and_replace(tmp, status);
	}
	if (is_unclose_quotes(count_single_quote, count_double_quotes))
		return (NULL);
	return (p);
}

int main()
{
	int		i = 0;
	char	**p;
	p = ft_split_first_cmd("$USER test $USER", ' ', 3);
	while(p[i])
		printf("%s\n", p[i++]);
}
