/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:33:17 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 10:18:59 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	cd_cmd(char *tokens, t_var *variables)
{
	char	*home;

	ft_status(0, 1);
	variables->cd_flag = 1;
	home = ft_getenv("HOME", variables);
	if (!tokens || (ft_strlen(tokens) == 1 && !ft_strncmp(tokens, "~", 1)))
	{
		if (!home)
		{
			ft_putstr_fd("\033[34mminishell: \033[0mcd: ", 2);
			ft_putstr_fd("\033[31mHOME not set.\033[0m\n", 2);
			1 && (ft_status(1, 1), variables->cd_flag = 0);
		}
		else
			chdir(home);
	}
	else if (chdir(tokens) == -1)
	{
		ft_putstr_fd("\033[34mminishell:\033[0m ", 2);
		ft_putstr_fd("cd: no such file or directory: \033[31m", 2);
		ft_putstr_fd(tokens, 2);
		ft_putstr_fd("\033[0m\n", 2);
		1 && (ft_status(1, 1), variables->cd_flag = 0);
	}
}

void	unset_cmd(char **tokens, t_var *variables)
{
	int	j;

	ft_status(0, 1);
	j = 0;
	while (tokens[j])
	{
		if (check_unset_arg(tokens[j]))
		{
			if (is_exist_var(tokens[j], variables, variables->export_list))
				all_env(NULL, tokens[j], 0, variables);
		}
		j++;
	}
}

char	*pwd_cmd(short is_print)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		pwd = ft_strdup("minishell/unknown");
	if (is_print)
		printf("%s\n", pwd);
	return (pwd);
}

static void	check_exit_args(char **tokens, int *status)
{
	if (*status == -3)
	{
		ft_putstr_fd("\033[34minishell: \033[0mexit: ", 2);
		ft_putstr_fd("\033[31mnumeric argument required.\033[0m\n", 2);
		*status = ft_status(255, 1);
	}
	else if (tokens[2])
	{
		*status = -2;
		ft_putstr_fd("\033[34mminishell:\033[0m exit: ", 2);
		ft_putstr_fd("\033[31mtoo many arguments.\033[0m\n", 2);
		ft_status(1, 1);
	}
}

void	exit_cmd(char **copy_env, t_var *variables,
				char **tokens, short is_print)
{
	int	status;

	status = -2;
	if (is_print)
		printf(RED "exit\n" DEF);
	if (!tokens || !tokens[1])
		status = ft_status(0, 0);
	else if (tokens[1])
	{
		status = ft_atoi(tokens[1]);
		check_exit_args(tokens, &status);
	}
	if (status != -2)
	{
		free(copy_env);
		free_leaks(variables);
		free(variables);
		exit(status);
	}
}
