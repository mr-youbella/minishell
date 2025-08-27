/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 21:18:19 by wkannouf          #+#    #+#             */
/*   Updated: 2025/08/27 11:17:38 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	add_export(t_env *env_var, t_list *exp_list,
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

static void	fix_oldpwd(t_var *variables, size_t i, char *name_var_env)
{
	t_list	*new_leak;

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

short	check_pwdenv(char *name_var_env, size_t i, size_t j, t_var *variables)
{
	char	*pwd;
	t_list	*new_leak;

	if (name_var_env && j == ft_strlen(name_var_env)
		&& !ft_strncmp(variables->copy_env[i], "PWD", j))
	{
		pwd = pwd_cmd(0);
		new_leak = ft_lstnew(pwd);
		ft_lstadd_back(&variables->leaks, new_leak);
		name_var_env = ft_strjoin("PWD=", pwd);
		new_leak = ft_lstnew(name_var_env);
		ft_lstadd_back(&variables->leaks, new_leak);
		variables->copy_env[i] = name_var_env;
	}
	else if (name_var_env && j == ft_strlen(name_var_env)
		&& !ft_strncmp(variables->copy_env[i], "OLDPWD", j))
		fix_oldpwd(variables, i, name_var_env);
	return (1);
}

short	check_oldpwd(size_t i, size_t j, t_var *variables, char *name_var_env)
{
	t_list	*new_leak;

	if (name_var_env && j == ft_strlen(name_var_env)
		&& !ft_strncmp(variables->copy_env[i], "OLDPWD", j))
	{
		name_var_env = ft_strdup("OLDPWD");
		new_leak = ft_lstnew(name_var_env);
		ft_lstadd_back(&variables->leaks, new_leak);
		variables->copy_env[i] = name_var_env;
		return (1);
	}
	return (0);
}
