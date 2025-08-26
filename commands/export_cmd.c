/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:42:32 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 06:48:58 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*print_sorted_env(t_var *variables, short is_return)
{
	size_t	count;
	t_list	*tmp;
	char	**env_array;

	count = 0;
	variables->i = 0;
	variables->export = NULL;
	tmp = variables->environment;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	env_array = malloc(sizeof(char *) * count);
	tmp = variables->environment;
	while (variables->i < count)
	{
		env_array[variables->i] = (char *)tmp->content;
		tmp = tmp->next;
		variables->i++;
	}
	sort_env(env_array, count);
	variables->i = 0;
	loop_print_env(env_array, variables, is_return, count);
	return (variables->export);
}

static char	*print_export(t_var *variables, short is_return,
							char *tmp, char *tmp2)
{
	t_list	*all_environment;
	t_list	*new_leak;
	char	*export;

	variables->is_export_cmd = 1;
	variables->environment = all_env(NULL, NULL, 1, variables);
	all_environment = variables->environment;
	while (all_environment)
	{
		new_leak = ft_lstnew(all_environment);
		ft_lstadd_back(&variables->leaks, new_leak);
		all_environment = all_environment->next;
	}
	export = print_sorted_env(variables, is_return);
	variables->is_export_cmd = 1;
	variables->environment = all_env(NULL, NULL, 2, variables);
	all_environment = variables->environment;
	while (all_environment)
	{
		new_leak = ft_lstnew(all_environment);
		ft_lstadd_back(&variables->leaks, new_leak);
		all_environment = all_environment->next;
	}
	1 && (tmp = export, tmp2 = print_sorted_env(variables, is_return));
	return (export = ft_strjoin(export, tmp2), free(tmp), free(tmp2), export);
}

static void	add_export_in_list(char **tokens, size_t i,
							short is_there_equal, t_var *variables)
{
	size_t	j;

	j = 0;
	if (is_exist_var(tokens[i], variables, variables->export_list))
	{
		while (tokens[i][j])
		{
			if (tokens[i][j] == '=')
			{
				is_there_equal = 1;
				break ;
			}
			j++;
		}
		if (is_there_equal)
			all_env(tokens[i], NULL, 0, variables);
	}
	else
		ft_lstadd_back(&variables->export_list, ft_lstnew(tokens[i]));
}

static short	add_again_env(char **tokens, size_t *i,
								size_t j, t_var *variables)
{
	char	*tmp;

	tmp = ft_substr(tokens[*i], 0, j);
	if (is_exist_in_env(tmp, variables->env, -1))
	{
		1 && (all_env(tokens[*i], NULL, 0, variables), (*i)++);
		free(tmp);
		return (0);
	}
	free(tmp);
	return (1);
}

char	*export_cmd(char **tokens, short is_return, t_var *variables)
{
	size_t	i;
	size_t	j;
	short	is_there_equal;

	ft_status(0, 1);
	i = 1;
	if (!tokens[i])
		return (print_export(variables, is_return, NULL, NULL));
	while (tokens[i])
	{
		1 && (j = 0, is_there_equal = 0);
		if (check_export_arg(tokens[i]))
		{
			while (tokens[i][j] && tokens[i][j] != '=')
				j++;
			if (!add_again_env(tokens, &i, j, variables))
				continue ;
			add_export_in_list(tokens, i, is_there_equal, variables);
		}
		i++;
	}
	return (NULL);
}
