/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_first_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 22:44:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/17 15:54:48 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	count_words(const char *str, char c)
{
	size_t	i;
	size_t	words;
	short	is_word;

	i = 0;
	words = 0;
	is_word = 0;
	if (str[i] != c && !is_word)
	{
		while (str[i] && str[i] == '"')
			i++;
		words++;
		is_word = 1;
	}
	while (str[i] && str[i] != '|')
	{
		if (str[i] == '"' && str[i] != c)
		{
			words++;
			is_word = 1;
			i++;
			while (str[i] && str[i] != '"')
				i++;
			if (str[i] == '"')
				i++;
			else
			{
				if (!str[i])
				{
					words--;
					is_word = 0;
					break ;
				}
			}
		}
		else if (str[i] == 39 && str[i] != c)
		{
			words++;
			is_word = 1;
			i++;
			while (str[i] && str[i] != 39)
				i++;
			if (str[i] == 39)
				i++;
			else
			{
				if (!str[i])
				{
					words--;
					is_word = 0;
					break ;
				}
			}
		}
		else if (str[i] == c)
			is_word = 0;
		i++;
	}
	return (words);
}

static char	*extract_word(const char *str, char c, size_t index)
{
	size_t	i;
	size_t quotes;
	i = 0;
	if (str[index + i] != '"'  && str[index + i] != 39 && str[index + i])
	{
		while(str[index + i] != ' ' && str[index + i])
		{
			if (str[index + i] == '"' || str[index + i] == 39)
			{
				i++;
				index++;
				c = '"';
				while(str[index + i] && str[index + i] != '"' && str[index + i] != 39)
					i++;
				if (!str[index + i])
				{
					printf(BLUE"minishell%s: syntax %serror%s: unclosed quote.\n", DEF, RED, DEF);
					return (NULL);
				}
			}
			i++;
		}
	}
	else if (str[index + i] == '"' && str[index + i])
	{
		index++;
		c = '"';
		while (str[index + i] && str[index + i] != c)
			i++;
		if (!str[index + i])
		{
			printf(BLUE"minishell%s: syntax %serror%s: unclosed quote.\n", DEF, RED, DEF);
			return (NULL);
		}
	}
	else if (str[index + i] == 39 && str[index + i])
	{
		index++;
		c = 39;
		while (str[index + i] && str[index + i] != c)
			i++;
		if (!str[index + i])
		{
			printf(BLUE"minishell%s: syntax %serror%s: unclosed quote.\n", DEF, RED, DEF);
			return (NULL);
		}
	}
	while (str[index + i] && str[index + i] != c && str[index + i] != '|')
	i++;
	return (ft_substr(str, index, i));
}

static short	ft_free(char **p, size_t j)
{
	if (!p[j - 1])
	{
		while (j--)
			free(p[j]);
		free(p);
		return (0);
	}
	return (1);
}

char	**ft_split_first_cmd(char const *s, char c)
{
	size_t	i;
	size_t	j;
	char	**p;

	if (!s)
		return (NULL);
	i = 0;
	j = 0;
	p = ft_calloc(count_words(s, c) + 1, 8);
	if (!p)
		return (NULL);
	printf("%zu\n", count_words(s, c));
	while (s[i] && s[i] != '|')
	{
		if (s[i] != c)
		{
			p[j++] = extract_word(s, c, i);
			if (!ft_free(p, j))
				return (NULL);
			if (s[i] == '"')
				i += 2;
			if (s[i] == 39)
				i += 2;
			i += ft_strlen(p[j - 1]);
		}
		i++;
	}
	return (p);
}
