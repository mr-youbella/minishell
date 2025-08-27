/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:01:37 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 13:01:47 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	take_var_export_without_plus(char *token, char *new_token)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (token[i])
	{
		if (token[i] == '+')
		{
			i++;
			continue ;
		}
		if (token[i] == '=')
			break ;
		new_token[j++] = token[i++];
	}
	while (token[i])
		new_token[j++] = token[i++];
	new_token[j] = 0;
}

char	*var_export_without_plus(char *token, t_var *variables)
{
	char	*new_token;
	size_t	i;

	i = 0;
	variables->is_append_val = 0;
	while (token[i])
	{
		if (token[i] == '+' && token[i + 1] == '=')
			variables->is_append_val = 1;
		if (token[i] == '=')
			break ;
		i++;
	}
	new_token = malloc(ft_strlen(token) - variables->is_append_val + 1);
	if (!new_token)
		return (NULL);
	take_var_export_without_plus(token, new_token);
	return (new_token);
}
