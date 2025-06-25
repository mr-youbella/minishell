/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkannouf <wkannouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 18:09:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/26 00:42:05 by wkannouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_dollar(char *str)
{
	size_t	i;
	size_t	j;
	char *p;
	
	if(!str)
		return (NULL);
	p = malloc(ft_strlen(str) + 1);
	j = 0;
	i = 0;
	if (!p)
		return (NULL);
	while (str[i])
	{
		if (str[i] == '$' && (ft_isalpha(str[i + 1]) || str[i + 1] == '_'))
		{
			i++;
			while (str[i] && (ft_isalpha(str[i]) || ft_isdigit(str[i]) || str[i] == '_'))
				p[j++] = str[i++];
			p[j++] = ' ';
		}
		else
			i++;
	}
	if (p)
	{
		p[j - 1] = 0;
		return (p);
	}
	return (free(p), NULL);
}

char	*search_env(char *str)
{
	char	*env;
	char	*result;
	char	**p;
	char	*final_result;
	char	*change_val;
	size_t	i;
	size_t j = 0;
	if (!str)
		return (NULL);
	result = search_dollar(str);
	if (!result)
		return (str);
	p = ft_split(result, ' ');
	free(result);
	final_result = ft_strdup("");
	i = 0;
	while (p[i])
	{
		env = getenv(p[i]);
		if (env)
		{
			change_val = ft_strjoin(final_result, env);
			free(final_result);
			final_result = change_val;
		}
		free(p[i]);
		i++;
	}
	free(p);
	while (!env && final_result)
	{
		free(str);
		free(final_result);
		return ("\0");
	}
	while (final_result && *final_result == '\0')
	{
		free(final_result);
		return (str);
	}
	return (final_result);

}
