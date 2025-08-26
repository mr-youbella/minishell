/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:43:10 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 03:53:00 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	sort_env(char **env_arr, int count)
{
	int		i;
	int		j;
	char	*temp;
	size_t	len;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strlen(env_arr[j]) > ft_strlen(env_arr[j + 1]))
				len = ft_strlen(env_arr[j]);
			else
				len = ft_strlen(env_arr[j + 1]);
			if (ft_strncmp(env_arr[j], env_arr[j + 1], len) > 0)
			{
				temp = env_arr[j];
				env_arr[j] = env_arr[j + 1];
				env_arr[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

static char	*print_var(short is_return, char *export, char *var_name)
{
	char	*tmp_export;

	if (is_return)
	{
		tmp_export = export;
		export = ft_strjoin(export, "declare -x ");
		free(tmp_export);
		tmp_export = export;
		export = ft_strjoin(export, var_name);
		free(tmp_export);
	}
	else
		printf("declare -x %s%s%s", GREEN, var_name, DEF);
	return (export);
}

static void	print_val_2(short is_return, t_var *variables,
					char	*tmp_export, char *var_value)
{
	if (is_return)
	{
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, "\"");
		free(tmp_export);
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, var_value);
		free(tmp_export);
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, "\"");
		free(tmp_export);
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, "\n");
		free(tmp_export);
	}
	else
		printf("%s\"%s\"%s\n", BLUE, var_value, DEF);
}

static void	print_val(short is_return, char	**env_array,
					size_t len_var, t_var *var)
{
	char	*tmp_export;
	char	*var_value;
	t_list	*new_leak;

	tmp_export = NULL;
	var_value = ft_substr(env_array[var->i],
			len_var, ft_strlen(env_array[var->i]) - len_var);
	new_leak = ft_lstnew(var_value);
	ft_lstadd_back(&var->leaks, new_leak);
	if ((!var_value[0] && env_array[var->i][len_var - 1] != '=') || !var_value)
	{
		if (is_return)
		{
			tmp_export = var->export;
			var->export = ft_strjoin(var->export, var_value);
			free(tmp_export);
			tmp_export = var->export;
			var->export = ft_strjoin(var->export, "\n");
			free(tmp_export);
		}
		else
			printf("%s%s%s\n", BLUE, var_value, DEF);
	}
	else
		print_val_2(is_return, var, tmp_export, var_value);
}

void	loop_print_env(char **env_array, t_var *variables,
						short is_return, size_t count)
{
	size_t	len_var;
	char	*var_name;
	t_list	*new_leak;

	while (variables->i < count)
	{
		len_var = 0;
		while (env_array[variables->i][len_var]
			&& env_array[variables->i][len_var] != '=')
			len_var++;
		if (env_array[variables->i][len_var] == '=')
			len_var++;
		var_name = ft_substr(env_array[variables->i], 0, len_var);
		new_leak = ft_lstnew(var_name);
		ft_lstadd_back(&variables->leaks, new_leak);
		variables->export = print_var(is_return, variables->export, var_name);
		print_val(is_return, env_array, len_var, variables);
		variables->i++;
	}
	free(env_array);
}
