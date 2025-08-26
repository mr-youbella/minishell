/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_env_cmd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:44:46 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 00:10:33 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	check_option_echo(char *token)
{
	size_t	i;

	i = 1;
	if (token[0] != '-')
		return (0);
	while (token[i])
	{
		if (token[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

static void	echo_all_args(char **tokens, short is_return, char **echo, size_t j)
{
	char	*tmp;

	while (tokens[j])
	{
		if (is_return)
		{
			tmp = *echo;
			*echo = ft_strjoin(*echo, tokens[j]);
			free(tmp);
		}
		else
			printf("%s", tokens[j]);
		if (tokens[j + 1] && ft_strlen(tokens[j]))
		{
			if (is_return)
			{
				tmp = *echo;
				*echo = ft_strjoin(*echo, " ");
				free(tmp);
			}
			else
				printf(" ");
		}
		j++;
	}
}

char	*echo_cmd(char **tokens, short is_return)
{
	size_t	j;
	short	is_op_echo;
	char	*echo;
	char	*tmp;

	ft_status(0, 1);
	1 && (j = 1, is_op_echo = 0, echo = NULL);
	while (tokens[j] && check_option_echo(tokens[j]))
	{
		is_op_echo = 1;
		j++;
	}
	echo_all_args(tokens, is_return, &echo, j);
	if (!is_op_echo)
	{
		if (is_return)
		{
			tmp = echo;
			echo = ft_strjoin(echo, "\n");
			free(tmp);
		}
		else
			printf("\n");
	}
	return (echo);
}

static void	print_environment(short is_print, t_list *copy_environment)
{
	short	is_with_value;
	size_t	i;

	while (is_print && copy_environment)
	{
		1 && (is_with_value = 0, i = 0);
		while (((char *)copy_environment->content)[i])
		{
			if (((char *)copy_environment->content)[i] == '=')
			{
				is_with_value = 1;
				break ;
			}
			i++;
		}
		if (is_with_value)
			printf("%s\n", (char *)copy_environment->content);
		copy_environment = copy_environment->next;
	}
}

t_list	*env_cmd(short is_print, t_var *variables)
{
	t_list	*environment;
	t_list	*all_environment;
	t_list	*copy_environment;
	t_list	*new_leak;

	ft_status(0, 1);
	all_environment = all_env(NULL, NULL, 0, variables);
	environment = all_environment;
	while (all_environment)
	{
		new_leak = ft_lstnew(all_environment);
		ft_lstadd_back(&variables->leaks, new_leak);
		all_environment = all_environment->next;
	}
	copy_environment = environment;
	print_environment(is_print, copy_environment);
	return (environment);
}
