/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_first_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/17 23:51:13 by youbella         ###   ########.fr       */
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

char	**ft_split_first_cmd(char const *s, char c)
{
	char	**p;
	char	*buffer;
	size_t	i;
	size_t	j;
	short	is_single_quote;
	short	is_double_quote;
	
	i = 0;
	j = 0;
	is_double_quote = 0;
	is_single_quote = 0;
	buffer = NULL;
	printf ("%zu\n", count_words(s, c));
	p =	ft_calloc(count_words(s, c) + 1, sizeof(char *));
	if (!p)
		return (NULL);
	while (s[i])
	{
		if (s[i] == 39 && !is_double_quote)
			is_single_quote = !is_single_quote;
		else if(s[i] == '"' && !is_single_quote)
			is_double_quote = !is_double_quote;
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
	return (p);
}
