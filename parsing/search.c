/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#`+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/06/22 11:57:53 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	short	is_exist_equal;

	i = 0;
	is_exist_equal = 0;
	if (!arg)
		return (0);
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
	{
		printf(BLUE "minishell: %sexport: %svariable not an identifier.%s\n" DEF, DEF, RED, CYAN);
		return (0);
	}
	while (arg[i])
	{
		if (arg[i] == '!' || arg[i] == '&' || arg[i] == '(' || arg[i] == ')')
		{
			printf(BLUE "minishell: %sexport: %snot valid in this context. %s\n" DEF, DEF, RED, CYAN);
			return (0);
		}
		if (arg[i] == '=')
			is_exist_equal = 1;
		i++;
	}
	if (!is_exist_equal)
		return (0);
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
		printf(BLUE "minishell: %sunset: %s%s%s: %sinvalid parameter name.\n" DEF, DEF, CYAN, arg, DEF, RED);
		return (0);
	}
	while (arg[i])
	{
		if (!ft_isalpha(arg[i]) && !ft_isdigit(arg[i]) && arg[i] != '_')
		{
			printf(BLUE "minishell: %sunset: %s%s%s: %sinvalid parameter name.\n" DEF, DEF, CYAN, arg, DEF, RED);
			return (0);
		}
		i++;
	}
	return (1); 
}
