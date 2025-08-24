/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/24 11:38:14 by youbella         ###   ########.fr       */
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
		write(2, "\033[34mminishell: \033[31mexport: \033[0;36mvariable not an identifier.\n\033[0m", ft_strlen("\033[34mminishell: \033[31mexport: \033[0;36mvariable not an identifier.\n\033[0m"));
		ft_status(1, 1);
		return (0);
	}
	while (arg[i])
	{
		if (arg[i] == '=')
			break ;
		if (arg[i] == '!' || arg[i] == '&' || arg[i] == '(' || arg[i] == ')')
		{
			write(2, "\033[34mminishell: \033[31mexport: \033[0;36mnot valid in this context.\n\033[0m", ft_strlen("\033[34mminishell: \033[31mexport: \033[0;36mnot valid in this context.\n\033[0m"));
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
		write(2, "\033[34mminishell: \033[0munset: \033[0;36m", ft_strlen("\033[34mminishell: \033[0munset: \033[0;36m"));
		write(2, arg, ft_strlen(arg));
		write(2, "\033[0m: \033[31minvalid parameter name.\n\033[0m", ft_strlen("\033[0m: \033[31minvalid parameter name.\n\033[0m"));
		ft_status(1, 1);
		return (0);
	}
	while (arg[i])
	{
		if (!ft_isalpha(arg[i]) && !ft_isdigit(arg[i]) && arg[i] != '_')
		{
			write(2, "\033[34mminishell: \033[0munset: \033[0;36m", ft_strlen("\033[34mminishell: \033[0munset: \033[0;36m"));
			write(2, arg, ft_strlen(arg));
			write(2, "\033[0m: \033[31minvalid parameter name.\n\033[0m", ft_strlen("\033[0m: \033[31minvalid parameter name.\n\033[0m"));
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

short	add_export(t_env *env_var, t_list *exp_list,
					t_list **enviroment, short is_with_value)
{
	t_list	*new_node;
	size_t	j;

	j = 0;
	while (((char *)exp_list->content)[j]
		&& ((char *)exp_list->content)[j] != '=')
	{
		j++;
		if (((char *)exp_list->content)[j] == '=')
			is_with_value = 1;
	}
	if (env_var->name_var_export && j == ft_strlen(env_var->name_var_export)
		&& !ft_strncmp((char *)exp_list->content, env_var->name_var_export, j))
		exp_list->content = env_var->var_export;
	if (!(env_var->var_unset && ft_strlen(env_var->var_unset) == j
			&& !ft_strncmp((char *)exp_list->content, env_var->var_unset, j)))
	{
		if (is_with_value || env_var->is_export_cmd)
		{
			new_node = ft_lstnew(exp_list->content);
			ft_lstadd_back(enviroment, new_node);
		}
		return (1);
	}
	return (0);
}

void	get_export(t_var *variables, t_list **enviroment, t_env *env_var)
{
	t_list	*export_list;

	export_list = variables->export_list;
	while (export_list)
	{
		if (!export_list->content)
		{
			export_list = export_list->next;
			continue ;
		}
		if (!add_export(env_var, export_list, enviroment, 0))
			export_list->content = NULL;
		export_list = export_list->next;
	}
}

void	get_env(t_var *variables, t_list **enviroment, t_env *env_var)
{
	size_t	i;
	size_t	j;
	t_list	*new_node;
	t_list	*new_leak;
	char	*pwd;
	char	*name_var_env;
	static short	oldpwd_flag;
	static short	is_chaneg_oldpwd;

	i = 0;
	while (variables->copy_env[i])
	{
		if (!ft_strlen(variables->copy_env[i]))
		{
			if (env_var->name_var_export)
			{
				if (is_exist_in_env(env_var->name_var_export, variables->env, i))
				{
					variables->copy_env[i] = env_var->var_export;
					env_var->name_var_export = NULL;
				}
			}
			i++;
			continue;
		}
		j = 0;
		while (variables->copy_env[i][j] && variables->copy_env[i][j] != '=')
			j++;
		name_var_env = ft_substr(variables->copy_env[i], 0, j);
		new_leak = ft_lstnew(name_var_env);
		ft_lstadd_back(&variables->leaks, new_leak);
		if (env_var->name_var_export && j == ft_strlen(env_var->name_var_export) && !ft_strncmp(variables->copy_env[i], env_var->name_var_export, j))
			variables->copy_env[i] = env_var->var_export;
		else if (!variables->cd_flag && !oldpwd_flag && !is_chaneg_oldpwd)
		{
			if (name_var_env && j == ft_strlen(name_var_env) && !ft_strncmp(variables->copy_env[i], "OLDPWD", j))
			{
				is_chaneg_oldpwd = 1;
				name_var_env = ft_strdup("OLDPWD");
				new_leak = ft_lstnew(name_var_env);
				ft_lstadd_back(&variables->leaks, new_leak);
				variables->copy_env[i] = name_var_env;
			}
		}
		else if (variables->cd_flag)
		{
			if (name_var_env && j == ft_strlen(name_var_env) && !ft_strncmp(variables->copy_env[i], "PWD", j))
			{
				pwd = pwd_cmd(0);
				new_leak = ft_lstnew(pwd);
				ft_lstadd_back(&variables->leaks, new_leak);
				name_var_env = ft_strjoin("PWD=", pwd);
				new_leak = ft_lstnew(name_var_env);
				ft_lstadd_back(&variables->leaks, new_leak);
				variables->copy_env[i] = name_var_env;
			}
			else if (name_var_env && j == ft_strlen(name_var_env) && !ft_strncmp(variables->copy_env[i], "OLDPWD", j))
			{
				if (variables->old_pwd)
					name_var_env = ft_strjoin("OLDPWD=", variables->old_pwd);
				else
				{
					variables->old_pwd = pwd_cmd(0);
					name_var_env = ft_strjoin("OLDPWD=", variables->old_pwd);
					free(variables->old_pwd);
				}
				new_leak = ft_lstnew(name_var_env);
				ft_lstadd_back(&variables->leaks, new_leak);
				variables->copy_env[i] = name_var_env;
			}
			oldpwd_flag = 1;
		}
		if (!(env_var->var_unset && ft_strlen(env_var->var_unset) == j && !ft_strncmp(variables->copy_env[i], env_var->var_unset, j)))
		{
			new_node = ft_lstnew(variables->copy_env[i]);
			ft_lstadd_back(enviroment, new_node);
		}
		else
		{
			variables->copy_env[i] = ft_strdup("");
			{
				new_leak = ft_lstnew(variables->copy_env[i]);
				ft_lstadd_back(&variables->leaks, new_leak);
			}
		}
		i++;
	}
	if (variables->cd_flag)
		variables->cd_flag = 0;
}

t_list	*all_env(char *var_export, char *var_unset, short is_export_cmd, short env_export, t_var *variables)
{
	t_list			*enviroment;
	t_list			*new_leak;
	t_list			*tmp;
	size_t			i;
	char			*name_var_export;
	t_env			*env_var;

	env_var = malloc(sizeof(t_env));
	env_var->var_export = var_export;
	env_var->var_unset = var_unset;
	env_var->is_export_cmd = is_export_cmd;
	enviroment = NULL;
	i = 0;
	while (var_export && var_export[i] && var_export[i] != '=')
		i++;
	name_var_export = ft_substr(var_export, 0, i);
	env_var->name_var_export = name_var_export;
	new_leak = ft_lstnew(name_var_export);
	ft_lstadd_back(&variables->leaks, new_leak);
	if (!env_export || env_export == 1)
		get_env(variables, &enviroment, env_var);
	if (!env_export || env_export == 2)
		get_export(variables, &enviroment, env_var);
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

short	is_exist_var(char *var, t_var *variables, t_list *export_list)
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
	ft_lstadd_back(&variables->leaks, new_leak);
	i = 0;
	while (variables->env[i])
	{
		if (!ft_strlen(variables->env[i]))
		{
			i++;
			continue ;
		}
		j = 0;
		while (variables->env[i] && variables->env[i][j] && variables->env[i][j] != '=')
			j++;
		if (var_name && j == ft_strlen(var_name) && !ft_strncmp(variables->env[i], var_name, j))
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
