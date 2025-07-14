/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkannouf <wkannouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 18:09:16 by wkannouf          #+#    #+#             */
/*   Updated: 2025/07/13 16:51:39 by wkannouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*search_dollar(char *str)
{
	size_t	i;
	size_t	j;
	char *p;
	
	if(!str)
		return (NULL);
	p = malloc(ft_strlen(str) + 1);
	if (!p)
		return (NULL);
	j = 0;
	i = 0;
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
	p[j - 1] = 0;
	return (p);
}

char	*search_env(char *str)
{
	char	*env;
	char	*result;
	char	**p;
	char	*final_result;
	size_t	i;

	if (!str)
		return (NULL);
	result = search_dollar(str);
	if (!result)
		return (str);
	p = ft_split(result, ' ');
	final_result = NULL;
	i = 0;
	while (p[i])
	{
		env = getenv(p[i]);
		if (env)
			final_result = ft_strjoin(final_result, env);
		i++;
	}
	while (!env && final_result)
		return ("\0");
	while (!final_result)
		return (str);
	return (final_result);
}
