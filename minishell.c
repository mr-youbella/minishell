/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 19:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/06/16 20:36:01 by youbella         ###   ########.fr       */
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
	int i;
	int status;
	char *input;
	char *path_cmd;
	char **args;
	char *pwd;
	char **path;
	char *this_dir;
	int pid;

	while (1)
	{
		i = 0;
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
		path_cmd = search_cmd(ft_split(input, ' '));
		if (!ft_strncmp(args[0], "cd", 2))
		{
			if (!args[1])
				args[1] = getenv("ZDOTDIR");
			chdir(args[1]);
			continue ;
		}
		if (!path_cmd)
		{
			printf(RED "minishell: %s%s%s command not found.\n", BLUE, input, DEF);
			status = 127;
			continue ;
		}
		pid = fork();
		if (pid == 0)
			execve(path_cmd, args, env);
		else
			wait(&status);
		printf("%d\n", WEXITSTATUS(status));
		free(input);
	}
}
