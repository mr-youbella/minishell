/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 04:03:56 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 07:02:47 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	check_varenv(t_env *env_var, t_var *variables, size_t i)
{
	size_t			j;
	static short	oldpwd_flag;
	static short	is_chaneg_oldpwd;
	t_list			*new_leak;
	char			*name_var_env;

	j = 0;
	while (variables->copy_env[i][j] && variables->copy_env[i][j] != '=')
		j++;
	name_var_env = ft_substr(variables->copy_env[i], 0, j);
	new_leak = ft_lstnew(name_var_env);
	ft_lstadd_back(&variables->leaks, new_leak);
	if (env_var->name_var_export && j == ft_strlen(env_var->name_var_export)
		&& !ft_strncmp(variables->copy_env[i], env_var->name_var_export, j)
		&& is_with_value(env_var))
		variables->copy_env[i] = env_var->var_export;
	else if (!variables->cd_flag && !oldpwd_flag && !is_chaneg_oldpwd)
		is_chaneg_oldpwd = check_oldpwd(i, j, variables, name_var_env);
	else if (variables->cd_flag)
		oldpwd_flag = check_pwdenv(name_var_env, i, j, variables);
}

static void	is_unset_varenv(size_t i, t_env *env_var,
							t_list **enviroment, t_var *variables)
{
	size_t	j;
	t_list	*new_node;
	t_list	*new_leak;

	j = 0;
	while (variables->copy_env[i][j] && variables->copy_env[i][j] != '=')
		j++;
	if (!(env_var->var_unset && ft_strlen(env_var->var_unset) == j
			&& !ft_strncmp(variables->copy_env[i], env_var->var_unset, j)))
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
}

static void	get_env(t_var *variables, t_list **enviroment, t_env *env_var)
{
	size_t	i;

	i = 0;
	while (variables->copy_env[i])
	{
		if (!ft_strlen(variables->copy_env[i]))
		{
			if (env_var->name_var_export)
			{
				if (is_exist_in_env(env_var->name_var_export,
						variables->env, i))
				{
					variables->copy_env[i] = env_var->var_export;
					env_var->name_var_export = NULL;
				}
			}
			i++;
			continue ;
		}
		check_varenv(env_var, variables, i);
		is_unset_varenv(i, env_var, enviroment, variables);
		i++;
	}
	if (variables->cd_flag)
		variables->cd_flag = 0;
}

static short	is_just_changed(t_env *env_var, t_list *enviroment)
{
	t_list	*tmp;

	if (env_var->var_export || env_var->var_unset)
	{
		while (enviroment)
		{
			tmp = enviroment;
			enviroment = enviroment->next;
			free(tmp);
		}
		return (1);
	}
	return (0);
}

t_list	*all_env(char *var_export, char *var_unset,
					short env_export, t_var *variables)
{
	t_list	*enviroment;
	t_list	*new_leak;
	size_t	i;
	char	*name_var_export;
	t_env	*env_var;

	env_var = malloc(sizeof(t_env));
	env_var->var_export = var_export;
	env_var->var_unset = var_unset;
	env_var->is_export_cmd = variables->is_export_cmd;
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
	if (is_just_changed(env_var, enviroment))
		return (variables->is_export_cmd = 0, NULL);
	return (variables->is_export_cmd = 0, enviroment);
}
