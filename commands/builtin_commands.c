/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_commands.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:50:56 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 06:30:35 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

short	is_buitin_cmd(char *token)
{
	if ((ft_strlen(token) == 4 && !ft_strncmp(token, "echo", 4))
		|| (ft_strlen(token) == 2 && !ft_strncmp(token, "cd", 2))
		|| (ft_strlen(token) == 3 && (!ft_strncmp(token, "pwd", 3)
				|| !ft_strncmp(token, "PWD", 3)))
		|| (ft_strlen(token) == 6 && !ft_strncmp(token, "export", 6))
		|| (ft_strlen(token) == 5 && !ft_strncmp(token, "unset", 5))
		|| (ft_strlen(token) == 3 && !ft_strncmp(token, "env", 3))
		|| (ft_strlen(token) == 4 && !ft_strncmp(token, "exit", 4)))
		return (1);
	return (0);
}

short	builtin_commands2(char **tokens, t_var *variables)
{
	char	*pwd;

	if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
	{
		unset_cmd(tokens, variables);
		return (ft_status(0, 0));
	}
	else if (ft_strlen(tokens[0]) == 3
		&& (!ft_strncmp(tokens[0], "pwd", 3)
			|| !ft_strncmp(tokens[0], "PWD", 3)))
	{
		pwd = pwd_cmd(0);
		printf("%s\n", pwd);
		free(pwd);
		return (ft_status(0, 1), ft_status(0, 0));
	}
	else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		return (echo_cmd(tokens, 0), ft_status(0, 0));
	else if (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2))
	{
		variables->old_pwd = ft_getenv("PWD", variables);
		cd_cmd(tokens[1], variables);
		return (ft_status(0, 0));
	}
	return (0);
}

short	builtin_commands(char **tokens, char **copy_env, t_var *variables)
{
	struct stat		file;

	if (!tokens || !tokens[0])
		return (free(tokens), 1);
	if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
	{
		exit_cmd(copy_env, variables, tokens, 1);
		return (ft_status(0, 0));
	}
	if (stat(tokens[0], &file) == 0 && S_ISDIR(file.st_mode))
	{
		ft_putstr_fd("\033[34mminishell: \033[31m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m is a directory.\n", 2);
		return (ft_status(126, 1));
	}
	if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6))
	{
		export_cmd(tokens, 0, variables);
		return (ft_status(0, 0));
	}
	if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
		return (env_cmd(1, variables), ft_status(0, 0));
	return (builtin_commands2(tokens, variables));
}
