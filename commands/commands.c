/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:33:17 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 16:26:37 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	check_cd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		ft_putstr_fd("\033[34mminishell: \033[0mcd: ", 2);
		ft_putstr_fd("\033[31mcannot access parent directories.\033[0m\n", 2);
	}
	free(pwd);
}

void	cd_cmd(char *tokens, t_var *variables)
{
	char	*home;

	1 && (ft_status(0, 1), variables->cd_flag = 1);
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
	check_cd();
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
