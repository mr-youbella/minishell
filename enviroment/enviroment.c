/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/26 08:13:13 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	is_exist_varenv(t_var *variables, char *var_name)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (variables->env[i])
	{
		if (!ft_strlen(variables->env[i]))
		{
			i++;
			continue ;
		}
		j = 0;
		while (variables->env[i] && variables->env[i][j]
				&& variables->env[i][j] != '=')
			j++;
		if (var_name && j == ft_strlen(var_name)
			&& !ft_strncmp(variables->env[i], var_name, j))
			return (1);
		i++;
	}
	return (0);
}

static short	is_exist_varexport(t_list *export_list, char *var_name)
{
	size_t	j;

	while (export_list)
	{
		if (!export_list->content)
		{
			export_list = export_list->next;
			continue ;
		}
		j = 0;
		while (((char *)export_list->content)[j]
			&& ((char *)export_list->content)[j] != '=')
			j++;
		if (var_name && j == ft_strlen(var_name)
			&& !ft_strncmp((char *)export_list->content, var_name, j))
			return (1);
		export_list = export_list->next;
	}
	return (0);
}

short	is_exist_var(char *var, t_var *variables, t_list *export_list)
{
	size_t	j;
	t_list	*new_leak;
	char	*var_name;

	j = 0;
	while (var && var[j] && var[j] != '=')
		j++;
	var_name = ft_substr(var, 0, j);
	new_leak = ft_lstnew(var_name);
	ft_lstadd_back(&variables->leaks, new_leak);
	if (is_exist_varenv(variables, var_name))
		return (1);
	if (is_exist_varexport(export_list, var_name))
		return (1);
	return (0);
}

char	**copy_environment(char	**env)
{
	size_t	i;
	char	**copy_env;

	if (!env)
		return (NULL);
	i = 0;
	while (env[i])
		i++;
	copy_env = malloc((i + 1) * sizeof(char *));
	if (!copy_env)
		return (ft_status(1, 1), NULL);
	i = 0;
	while (env[i])
	{
		copy_env[i] = env[i];
		i++;
	}
	copy_env[i] = NULL;
	return (copy_env);
}

short	is_with_value(t_env *env_var)
{
	size_t	i;

	i = 0;
	while (env_var->var_export[i])
	{
		if (env_var->var_export[i] == '=')
			return (1);
		i++;
	}
	return (0);
}
