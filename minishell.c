/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 19:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/06/18 22:49:12 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *search_cmd(char *cmd)
{
	int i;
	char *env_path;
	char **split_env_path;
	char *join_cmd_to_path;

	i = 0;
	if (cmd[0] == '.' && cmd[1] == '/')
		return (cmd);
	env_path = getenv("PATH");
	split_env_path = ft_split(env_path, ':');
	while (split_env_path[i])
	{
		join_cmd_to_path = ft_strjoin(split_env_path[i], "/");
		join_cmd_to_path = ft_strjoin(join_cmd_to_path, cmd);
		if (!access(join_cmd_to_path, X_OK))
			return (join_cmd_to_path);
		i++;
	}
	return (NULL);
}

void	handle_signal(int sig)
{
	(void)sig;
	printf("\n");
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

int main(int argc, char **argv, char **env)
{
	struct sigaction	sig;
    struct termios		ctr;
	int					i;
	int					j;
	int					status;
	int					pid;
	short				is_op_echo;
	char				*input;
	char				*path_cmd;
	char				**args;
	char				*pwd;
	char				**path;
	char				*this_dir;
	char				*args_export;
	char				*args_unset;
	char				**split_export;
	char				**split_unset;

	tcgetattr(0, &ctr);
    ctr.c_lflag &= ~ECHOCTL;
    tcsetattr(0, 0, &ctr);
    sig.sa_handler = handle_signal;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig, NULL);
    signal(SIGQUIT, SIG_IGN);
	args_export = NULL;
	args_unset = NULL;
	while (1)
	{
		i = 0;
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
		if (!input)
			break ;
		if (!input[0])
			continue ;
		add_history(input);
		args = ft_split_first_cmd(input, ' ', WEXITSTATUS(status));
		if (!args)
			continue ;
		if (!ft_strncmp(args[0], "exit", 4) && ft_strlen(args[0]) == 4)
			break ;
		else if (!ft_strncmp(args[0], "export", 6) && ft_strlen(args[0]) == 6)
		{
			j = 1;
			while (args[j])
			{
				args_export = ft_strjoin(args_export, args[j]);
				args_export = ft_strjoin(args_export, " ");
				j++;
			}
			continue ;
		}
		else if (!ft_strncmp(args[0], "unset", 5) && ft_strlen(args[0]) == 5)
		{
			j = 1;
			while (args[j])
			{
				args_unset = ft_strjoin(args_unset, args[j]);
				args_unset = ft_strjoin(args_unset, " ");
				j++;
			}
			continue ;
		}
		else if (!ft_strncmp(args[0], "env", 3) && ft_strlen(args[0]) == 3)
		{
			j = 1;
			while (env[j])
				printf("%s\n", env[j++]);
			j = 0;
			split_export = ft_split(args_export, ' ');
			split_unset = ft_split(args_unset, ' ');
			while (split_export && split_export[j])
			{
				if (!is_unset(split_export[j], split_unset))
					printf("%s\n", split_export[j]);
				j++;
			}
			continue ;
		}
		else if (!ft_strncmp(args[0], "pwd", 3) && ft_strlen(args[0]) == 3)
		{
			printf("%s\n", pwd);
			continue ;
		}
		else if (!ft_strncmp(args[0], "echo", 4) && ft_strlen(args[0]) == 4)
		{
			j = 1;
			if (args[j] && !ft_strncmp(args[j], "-n", 2) && ft_strlen(args[j]) == 2)
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
		else if (!ft_strncmp(args[0], "cd", 2))
		{
			if (chdir(args[1]) == -1)
				printf(BLUE"minishell%s: cd: no such file or directory: %s%s%s\n", DEF, RED, args[1], DEF);
			continue ;
		}
		path_cmd = search_cmd(args[0]);
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

// is_unset(char *export, char **unset);