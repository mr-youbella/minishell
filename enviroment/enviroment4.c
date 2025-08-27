/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment4.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 04:06:40 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 14:50:59 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*check_env(char *var, t_list *environment,
						size_t i, t_var *variables)
{
	char	*var_env;
	t_list	*new_leak;
	char	*env;

	while (((char *)environment->content)[i])
	{
		if (((char *)environment->content)[i] == '=')
			break ;
		i++;
	}
	var_env = ft_substr((char *)environment->content, 0, i);
	new_leak = ft_lstnew(var_env);
	ft_lstadd_back(&variables->leaks, new_leak);
	if (!var_env)
		return (variables->is_return = 1, NULL);
	env = ft_substr((char *)environment->content, i + 1,
			ft_strlen((char *)environment->content) - i - 1);
	new_leak = ft_lstnew(env);
	ft_lstadd_back(&variables->leaks, new_leak);
	if (!env)
		return (variables->is_return = 1, NULL);
	if (ft_strlen(var) == ft_strlen(var_env)
		&& !ft_strncmp(var, var_env, ft_strlen(var)))
		return (variables->is_return = 1, env);
	return (variables->is_return = 0, NULL);
}

char	*ft_getenv(char *var, t_var *variables)
{
	char	*var_env;
	t_list	*environment;

	if (!var)
		return (NULL);
	environment = variables->environment;
	while (environment)
	{
		if (!environment->content)
		{
			environment = environment->next;
			continue ;
		}
		var_env = check_env(var, environment, 0, variables);
		if (var_env && variables->is_return == 1)
			return (var_env);
		environment = environment->next;
	}
	return (NULL);
}

short	check_export_arg(char *arg)
{
	size_t	i;

	i = 0;
	if (!arg)
		return (0);
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
	{
		ft_putstr_fd("\033[34mminishell: \033[0mexport: ", 2);
		ft_putstr_fd("\033[31mvariable not an identifier.\n\033[0m", 2);
		return (ft_status(1, 1), 0);
	}
	while (arg[i])
	{
		if (arg[i] == '=' || (arg[i] == '+' && arg[i + 1] == '='))
			break ;
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
		{
			ft_putstr_fd("\033[34mminishell: \033[0mexport: ", 2);
			ft_putstr_fd("\033[31mnot valid in this context.\n\033[0m", 2);
			return (ft_status(1, 1), 0);
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
		ft_putstr_fd("\033[34mminishell: \033[0munset: \033[0;36m", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("\033[0m: \033[31minvalid parameter name.\n\033[0m", 2);
		return (ft_status(1, 1), 0);
	}
	while (arg[i])
	{
		if (!ft_isalpha(arg[i]) && !ft_isdigit(arg[i]) && arg[i] != '_')
		{
			ft_putstr_fd("\033[34mminishell: \033[0munset: \033[0;36m", 2);
			ft_putstr_fd(arg, 2);
			ft_putstr_fd("\033[0m: \033[31minvalid parameter name\n\033[0m", 2);
			return (ft_status(1, 1), 0);
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
			if (position == -1 || position == (long)i)
				return (free(var), 1);
		}
		free(var);
		i++;
	}
	return (0);
}
