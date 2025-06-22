/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directions.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkannouf <wkannouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:23:33 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/22 16:51:47 by wkannouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_redirect	*search_directions_name_file(char *str, t_redirect *redirect)
{
	size_t	i;
	size_t	len;
	size_t	start;

	start = 0;
	len = 0;
	i = 0;
	while(redirect && str[i])
	{
		if (str[i] == '>')
		{
			if (str[i + 1] == '>')
			{
				redirect->type_redirection = 'D';
				i += 2;
			}
			else
			{
				redirect->type_redirection = '>';
				i++;
			}
			while (str[i] == ' ')
				i++;
			start = i;
			if (redirect->type_redirection == 'D' || redirect->type_redirection == '>')
			{
				while (str[i] && str[i] != '>' && str[i] != ' ')
					i++;
			}
			len = i - start;
			redirect->file_name = malloc(len + 1);
			ft_strlcpy(redirect->file_name, &str[start], len + 1);
			redirect = redirect->next;
		}
		else 
			i++;
	}
	
	
	return (NULL);
}
