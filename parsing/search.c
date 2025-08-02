/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkannouf <wkannouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:11:52 by wkannouf          #+#    #+#             */
/*   Updated: 2025/07/24 13:34:30 by wkannouf         ###   ########.fr       */
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

t_list *all_env(char *var, char **env, t_list *export_list)
{
	t_list	*enviroment;
	t_list	*new_node;
	size_t	i;
	size_t	j;
	char	*p;
	char *exp;

	enviroment = NULL;
	i = 0;
	while (var[i] != '=')
		i++;
	p = ft_substr(var, 0, i);
	i = 0;
	while (env[i])
	{
		j = 0;
		while (env[i][j] != '=')
			j++;
		if (j == ft_strlen(p) && !ft_strncmp(env[i], p, j))
			env[i] = var;
		new_node = ft_lstnew(env[i]);
		ft_lstadd_back(&enviroment, new_node);
		i++;
	}
	while (export_list)
	{
		j = 0;
		exp = export_list->content;
		while (exp[j] != '=')
			j++;
		if (j == ft_strlen(p) && !ft_strncmp(exp, p, j))
			exp = var;
		new_node = ft_lstnew(export_list->content);
		ft_lstadd_back(&enviroment, new_node);
		export_list = export_list->next;
	}
	return (enviroment);
}

int main(int argc, char **argv, char **env)
{
	char p[] = "namee=l3zi";
	t_list *e = all_env(p, argv, NULL);
	t_list *en = all_env(p, env, e);
	while (en)
	{
		printf("%s\n", (char *)en->content);
		en = en->next;
	}
}
