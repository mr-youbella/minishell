/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections5.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 12:57:09 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 23:27:47 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*defualt_env(char *str, t_var *variables)
{
	t_list	*new_leak;
	char	*new_env;

	new_env = ft_strdup(str);
	new_leak = ft_lstnew(new_env);
	ft_lstadd_back(&variables->leaks, new_leak);
	return (new_env);
}

char	**add_env(char **env, t_var *variables)
{
	char	*pwd;
	char	**new_env;

	if (!env[0])
	{
		pwd = getcwd(NULL, 0);
		new_env = malloc(6 * sizeof(char *));
		if (!new_env)
			return (NULL);
		new_env[0] = defualt_env("OLDPWD", variables);
		new_env[1] = defualt_env
			("PATH=/usr/gnu/bin:/usr/local/bin:/bin:/usr/bin:.", variables);
		new_env[2] = defualt_env(ft_strjoin("PWD=", pwd), variables);
		new_env[3] = defualt_env("SHLVL=2", variables);
		new_env[4] = defualt_env("_=/usr/bin/env", variables);
		new_env[5] = NULL;
		free(pwd);
		return (new_env);
	}
	return (NULL);
}

void	change_v_export(char *v_export, char *name_var_export,
							t_var *variables, t_env *env_var)
{
	size_t	i;
	t_list	*new_leak;
	char	*value;
	char	*tmp;

	i = 0;
	while (v_export && v_export[i] && v_export[i] != '=')
		i++;
	if (v_export && variables->is_append_val)
	{
		env_var->var_export = ft_strjoin(name_var_export, "=");
		tmp = env_var->var_export;
		env_var->var_export = ft_strjoin(env_var->var_export,
				ft_getenv(name_var_export, variables));
		tmp = env_var->var_export;
		free(tmp);
		value = ft_substr(v_export, i + 1, ft_strlen(v_export) - i);
		env_var->var_export = ft_strjoin(env_var->var_export, value);
		free(tmp);
		free(value);
	}
	variables->is_append_val = 0;
	new_leak = ft_lstnew(name_var_export);
	ft_lstadd_back(&variables->leaks, new_leak);
}
