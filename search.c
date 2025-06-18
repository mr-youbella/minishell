/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/18 22:16:42 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

short	is_unset(char *export, char **unset)
{
	size_t	i;
	
	i = 0;
	while (unset[i])
	{
		if (!ft_strncmp(export, unset[i], ULONG_MAX))
			return (1);
		i++;
	}
	return (0);
}
