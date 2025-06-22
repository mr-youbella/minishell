/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkannouf <wkannouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/22 10:47:16 by wkannouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*search_in_list(char *str, t_list *list)
{
   while (list)
   {
       if (ft_strlen(str) == ft_strlen((char *)list->content) && !ft_strncmp(str, (char *)list->content, ft_strlen(str)))
           return (list);
       list = list->next;
   }
   return (NULL);
}

short	check_export_arg(char *arg)
{
	size_t	i;
	char	**split_arg;

	i = 0;
	split_arg = ft_split(arg, '=');
	if (!arg)
		return (0);
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
	{
		printf("export: not an identifier: %s\n", split_arg[0]);
		return (0);
	}
	while (arg[i])
	{
		if (!ft_isalpha(arg[i]) && !ft_isdigit(arg[i]) && arg[i] != '_' && arg[i] != '=')
		{
			printf("export: not valid in this context: %s\n", split_arg[0]);
			return (0);
		}
		if (arg[i] == '=' && arg[i + 1] == 0)
			return (2);
		i++;
	}
	return (1);
}

short	check_unset_arg(char *arg)
{
	size_t	i;

	i = 0;
	if (!arg)
		return (0);
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
	{
		printf("unset: %s: invalid parameter name.\n", arg);
		return (0);
	}
	while (arg[i])
	{
		if (!ft_isalpha(arg[i]) && !ft_isdigit(arg[i]) && arg[i] != '_')
		{
			printf("unset: %s: invalid parameter name.\n", arg);
			return (0);
		}
		i++;
	}
	return (1); 
}
