/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 19:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/06/16 23:30:22 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *search_cmd(char **cmd)
{
	int i;
	char *env_path;
	char **split_env_path;
	char *join_cmd_to_path;

	i = 0;
	if (cmd[0][0] == '.' && cmd[0][1] == '/')
		return (cmd[0]);
	env_path = getenv("PATH");
	split_env_path = ft_split(env_path, ':');
	while (split_env_path[i])
	{
		join_cmd_to_path = ft_strjoin(split_env_path[i], "/");
		join_cmd_to_path = ft_strjoin(join_cmd_to_path, cmd[0]);
		if (!access(join_cmd_to_path, X_OK))
			return (join_cmd_to_path);
		i++;
	}
	return (NULL);
}

int main(int argc, char **argv, char **env)
{
	int		i;
	int		j;
	int		status;
	int		pid;
	short	is_op_echo;
	char	*input;
	char	*path_cmd;
	char	**args;
	char	*pwd;
	char	**path;
	char	*this_dir;
	char	*cd_path;

	while (1)
	{
		i = 0;
		j = 1;
		is_op_echo = 0;
		pwd = getcwd(NULL, 0);
		path = ft_split(pwd, '/');
		while (path[i])
			this_dir = path[i++];
		this_dir = ft_strjoin("\033[1;94m", this_dir);
		this_dir = ft_strjoin(this_dir, "\033[0m");
		this_dir = ft_strjoin("\033[32m➜\033[0m ", this_dir);
		this_dir = ft_strjoin(this_dir, " ");
		input = readline(this_dir);
		add_history(input);
		args = ft_split_first_cmd(input, ' ');
		if (!args)
			continue ;
		if (!ft_strncmp(args[0], "pwd", 3) && ft_strlen(args[0]) == 3)
		{
			printf("%s\n", pwd);
			continue ;
		}
		else if (!ft_strncmp(args[0], "echo", 4) && ft_strlen(args[0]) == 4)
		{
			if (args[1] && !ft_strncmp(args[1], "-n", 2) && ft_strlen(args[1]) == 2)
			{
				is_op_echo = 1;
				j++;
			}
			while (args[j])
			{
				printf("%s ", args[j]);
				j++;
			}
			if (!is_op_echo)
				printf("\n");
			continue ;
		}
		path_cmd = search_cmd(ft_split_first_cmd(input, ' '));
		if (!ft_strncmp(args[0], "cd", 2))
		{
			if (chdir(args[1]) == -1)
				printf(BLUE"minishell%s: cd: no such file or directory: %s%s%s\n", DEF, RED, args[1], DEF);
			continue ;
		}
		if (!path_cmd)
		{
			printf(RED "minishell: %s%s%s command not found.\n", BLUE, args[0], DEF);
			status = 127;
			continue ;
		}
		pid = fork();
		if (pid == 0)
			execve(path_cmd, args, env);
		else
			wait(&status);
		free(input);
	}
}
