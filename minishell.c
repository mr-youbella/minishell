/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 19:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/06/14 22:47:03 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_cmd(char **cmd)
{
	int		i;
	char 	*env_path;
	char 	**split_env_path;
	char	*join_cmd_to_path;

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
	char	*input;
	char	*path_cmd;
	char	**args;
	char	*pwd;
	char	**path;
	char	*this_dir;
	int 	pid;
	
	i = 0;
	pwd = getcwd(NULL, 0);
	path = ft_split(pwd, '/');
	if (!path)
		return (1);
	while (path[i])
		this_dir = path[i++];
	this_dir = ft_strjoin("\033[1;94m", this_dir);
	this_dir = ft_strjoin(this_dir, "\033[0m");
	this_dir = ft_strjoin("\033[32m➜\033[0m ", this_dir);
	this_dir = ft_strjoin(this_dir, " ");
	while (1)
	{
		input = readline(this_dir);
		add_history(input);
		path_cmd = search_cmd(ft_split(input, ' '));
		if (!path_cmd)
			printf(RED "minishell: %s%s%s command not found.\n", BLUE, input, DEF);
		args = ft_split_first_cmd(input, ' ');
		if (!args)
			break ;
		pid = fork();
		if (pid == 0)
    		execve(path_cmd, args, env);
		wait(NULL);
		free(input);
	}
}
