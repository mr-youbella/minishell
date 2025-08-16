/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/15 18:23:22 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_list *search_in_list(char *str, t_list *list)
{
	size_t i;
	char *p;

	while (list)
	{
		i = 0;
		while ((char *)list->content && ((char *)list->content)[i] && ((char *)list->content)[i] != '=')
			i++;
		p = ft_substr((char *)list->content, 0, i);
		if (ft_strlen(str) == ft_strlen(p) && !ft_strncmp(str, p, ft_strlen(str)))
			return (list);
		list = list->next;
	}
	return (NULL);
}

short check_export_arg(char *arg)
{
	size_t i;

	i = 0;
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
		i++;
	}
	return (1);
}

short check_unset_arg(char *arg)
{
	size_t i;

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

t_list *all_env(char *var_export, char *var_unset, char **env, t_list *export_list, short is_export_cmd, short env_export, short *cd_flag, char *old_pwd)
{
	t_list *enviroment;
	t_list *new_node;
	size_t i;
	size_t j;
	static short oldpwd_flag;
	short is_with_value;
	char *p;

	enviroment = NULL;
	i = 0;
	while (var_export && var_export[i] && var_export[i] != '=')
		i++;
	p = ft_substr(var_export, 0, i);
	i = 0;
	if (!env_export || env_export == 1)
	{
		while (env[i])
		{
			if (!ft_strlen(env[i]))
			{
				i++;
				continue;
			}
			j = 0;
			while (env[i][j] && env[i][j] != '=')
				j++;
			char *pp = ft_substr(env[i], 0, j);
			if (p && j == ft_strlen(p) && !ft_strncmp(env[i], p, j))
				env[i] = var_export;
			else if (cd_flag && !*cd_flag && !oldpwd_flag)
			{
				if (pp && j == ft_strlen(pp) && !ft_strncmp(env[i], "OLDPWD", j))
				{
					pp = ft_strjoin("OLDPWD", "");
					env[i] = pp;
				}
			}
			else if (cd_flag && *cd_flag)
			{
				if (pp && j == ft_strlen(pp) && !ft_strncmp(env[i], "PWD", j))
				{
					pp = ft_strjoin("PWD=", pwd_cmd(0));
					env[i] = pp;
				}
				else if (pp && j == ft_strlen(pp) && !ft_strncmp(env[i], "OLDPWD", j) && old_pwd)
				{
					pp = ft_strjoin("OLDPWD=", old_pwd);
					env[i] = pp;
				}
				oldpwd_flag = 1;
			}
			if (!(var_unset && ft_strlen(var_unset) == j && !ft_strncmp(env[i], var_unset, j)))
			{
				new_node = ft_lstnew(env[i]);
				ft_lstadd_back(&enviroment, new_node);
			}
			else
				env[i] = ft_strdup("");
			i++;
		}
		if (cd_flag)
			*cd_flag = 0;
	}
	if (!env_export || env_export == 2)
	{
		while (export_list)
		{
			is_with_value = 0;
			if (!export_list->content)
			{
				export_list = export_list->next;
				continue;
			}
			j = 0;
			while (((char *)export_list->content)[j] && ((char *)export_list->content)[j] != '=')
			{
				j++;
				if (((char *)export_list->content)[j] == '=')
					is_with_value = 1;
			}
			if (p && j == ft_strlen(p) && !ft_strncmp((char *)export_list->content, p, j))
				export_list->content = var_export;
			if (!(var_unset && ft_strlen(var_unset) == j && !ft_strncmp((char *)export_list->content, var_unset, j)))
			{
				if (is_with_value || is_export_cmd)
				{
					new_node = ft_lstnew(export_list->content);
					ft_lstadd_back(&enviroment, new_node);
				}
			}
			else
				export_list->content = NULL;
			export_list = export_list->next;
		}
	}
	return (enviroment);
}

short is_exist_var(char *var, char **env, t_list *export_list)
{
	size_t i;
	size_t j;
	char *p;

	i = 0;
	while (var && var[i] && var[i] != '=')
		i++;
	p = ft_substr(var, 0, i);
	i = 0;
	while (env[i])
	{
		if (!ft_strlen(env[i]))
		{
			i++;
			continue;
		}
		j = 0;
		while (env[i] && env[i][j] != '=')
			j++;
		if (p && j == ft_strlen(p) && !ft_strncmp(env[i], p, j))
			return (1);
		i++;
	}
	while (export_list)
	{
		if (!export_list->content)
		{
			export_list = export_list->next;
			continue;
		}
		j = 0;
		while (((char *)export_list->content)[j] && ((char *)export_list->content)[j] != '=')
			j++;
		if (p && j == ft_strlen(p) && !ft_strncmp((char *)export_list->content, p, j))
			return (1);
		export_list = export_list->next;
	}
	return (0);
}
