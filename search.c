/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/21 21:29:37 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

short	check_export_arg(char *arg)
{
	size_t	i;

	i = 0;
	if (!arg)
		return (0);
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
	{
		printf(BLUE "minishell: %sexport: %svariable not an identifier.%s\n"
			DEF, DEF, RED, CYAN);
		ft_status(1, 1);
		return (0);
	}
	while (arg[i])
	{
		if (arg[i] == '=')
			break ;
		if (arg[i] == '!' || arg[i] == '&' || arg[i] == '(' || arg[i] == ')')
		{
			printf(BLUE "minishell: %sexport: %snot valid in this context. %s\n"
				DEF, DEF, RED, CYAN);
			ft_status(1, 1);
			return (0);
		}
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
		printf(BLUE "minishell: %sunset: %s%s%s: %sinvalid parameter name.\n" DEF, DEF, CYAN, arg, DEF, RED);
		ft_status(1, 1);
		return (0);
	}
	while (arg[i])
	{
		if (!ft_isalpha(arg[i]) && !ft_isdigit(arg[i]) && arg[i] != '_')
		{
			printf(BLUE "minishell: %sunset: %s%s%s: %sinvalid parameter name.\n"DEF, DEF, CYAN, arg, DEF, RED);
			ft_status(1, 1);
			return (0);
		}
		i++;
	}
	return (1);
}

short	is_exist_in_env(char *str, char **env, long position)
{
	size_t	i;
	size_t	j;
	char	*var;
	
	if (!str || !env)
		return (0);
	i = 0;
	while (env[i])
	{
		j = 0;
		while (env[i][j] && env[i][j] != '=')
			j++;
		var = ft_substr(env[i], 0, j);
		if (ft_strlen(str) == ft_strlen(var) && !ft_strncmp(str, var, j))
		{
			if (position == (long)i || position == -1)
				return (free(var), 1);
		}
		free(var);
		i++;
	}
	return (0);
}

t_list	*all_env(char *var_export, char *var_unset, char **env, char **real_env, t_list *export_list, short is_export_cmd, short env_export, short *cd_flag, char *old_pwd, t_list **leaks)
{
	t_list			*enviroment;
	t_list			*new_node;
	t_list			*new_leak;
	t_list			*tmp;
	size_t			i;
	size_t			j;
	static short	oldpwd_flag;
	static short	is_chaneg_oldpwd;
	short			is_with_value;
	char			*name_var_export;
	char			*name_var_env;
	char			*pwd;

	enviroment = NULL;
	i = 0;
	while (var_export && var_export[i] && var_export[i] != '=')
		i++;
	name_var_export = ft_substr(var_export, 0, i);
	new_leak = ft_lstnew(name_var_export);
	ft_lstadd_back(leaks, new_leak);
	i = 0;
	if (!env_export || env_export == 1)
	{
		while (env[i])
		{
			if (!ft_strlen(env[i]))
			{
				if (name_var_export)
				{
					if (is_exist_in_env(name_var_export, real_env, i))
					{
						env[i] = var_export;
						name_var_export = NULL;
					}
				}
				i++;
				continue;
			}
			j = 0;
			while (env[i][j] && env[i][j] != '=')
				j++;
			name_var_env = ft_substr(env[i], 0, j);
			new_leak = ft_lstnew(name_var_env);
			ft_lstadd_back(leaks, new_leak);
			if (name_var_export && j == ft_strlen(name_var_export) && !ft_strncmp(env[i], name_var_export, j))
				env[i] = var_export;
			else if (cd_flag && !*cd_flag && !oldpwd_flag && !is_chaneg_oldpwd)
			{
				if (name_var_env && j == ft_strlen(name_var_env) && !ft_strncmp(env[i], "OLDPWD", j))
				{
					is_chaneg_oldpwd = 1;
					name_var_env = ft_strdup("OLDPWD");
					new_leak = ft_lstnew(name_var_env);
					ft_lstadd_back(leaks, new_leak);
					env[i] = name_var_env;
				}
			}
			else if (cd_flag && *cd_flag)
			{
				if (name_var_env && j == ft_strlen(name_var_env) && !ft_strncmp(env[i], "PWD", j))
				{
					pwd = pwd_cmd(0);
					new_leak = ft_lstnew(pwd);
					ft_lstadd_back(leaks, new_leak);
					name_var_env = ft_strjoin("PWD=", pwd);
					new_leak = ft_lstnew(name_var_env);
					ft_lstadd_back(leaks, new_leak);
					env[i] = name_var_env;
				}
				else if (name_var_env && j == ft_strlen(name_var_env) && !ft_strncmp(env[i], "OLDPWD", j))
				{
					if (old_pwd)
						name_var_env = ft_strjoin("OLDPWD=", old_pwd);
					else
					{
						old_pwd = pwd_cmd(0);
						name_var_env = ft_strjoin("OLDPWD=", old_pwd);
						free(old_pwd);
					}
					new_leak = ft_lstnew(name_var_env);
					ft_lstadd_back(leaks, new_leak);
					env[i] = name_var_env;
				}
				oldpwd_flag = 1;
			}
			if (!(var_unset && ft_strlen(var_unset) == j && !ft_strncmp(env[i], var_unset, j)))
			{
				new_node = ft_lstnew(env[i]);
				ft_lstadd_back(&enviroment, new_node);
			}
			else
			{
				env[i] = ft_strdup("");
				{
					new_leak = ft_lstnew(env[i]);
					ft_lstadd_back(leaks, new_leak);
				}
			}
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
			if (name_var_export && j == ft_strlen(name_var_export) && !ft_strncmp((char *)export_list->content, name_var_export, j))
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
	if (var_export || var_unset)
	{
		while (enviroment)
		{
			tmp = enviroment;
			enviroment = enviroment->next;
			free(tmp);
		}
		return (NULL);
	}
	return (enviroment);
}

short	is_exist_var(char *var, char **env, t_list *export_list, t_list **leaks)
{
	size_t	i;
	size_t	j;
	t_list	*new_leak;
	char	*var_name;

	i = 0;
	while (var && var[i] && var[i] != '=')
		i++;
	var_name = ft_substr(var, 0, i);
	new_leak = ft_lstnew(var_name);
	ft_lstadd_back(leaks, new_leak);
	i = 0;
	while (env[i])
	{
		if (!ft_strlen(env[i]))
		{
			i++;
			continue ;
		}
		j = 0;
		while (env[i] && env[i][j] && env[i][j] != '=')
			j++;
		if (var_name && j == ft_strlen(var_name) && !ft_strncmp(env[i], var_name, j))
			return (1);
		i++;
	}
	while (export_list)
	{
		if (!export_list->content)
		{
			export_list = export_list->next;
			continue ;
		}
		j = 0;
		while (((char *)export_list->content)[j] && ((char *)export_list->content)[j] != '=')
			j++;
		if (var_name && j == ft_strlen(var_name) && !ft_strncmp((char *)export_list->content, var_name, j))
			return (1);
		export_list = export_list->next;
	}
	return (0);
}
