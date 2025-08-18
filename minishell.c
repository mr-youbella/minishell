/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 19:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/08/18 02:41:08 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_status(int status, short is_change)
{
	static int new_status;

	if (is_change)
		new_status = status;
	return (WEXITSTATUS(new_status));
}

short g_signle_flag = 0;

void handle_signal(int sig_num)
{
	if (g_signle_flag == 0 && sig_num == SIGQUIT)
		write(1, "Quit: 3\n", 9);
	else if (g_signle_flag == 0 && sig_num == SIGINT)
	{
		ft_status(256, 1);
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

char *join_tokens(char **tokens)
{
	size_t i;
	char *tokens_in_line;

	i = 0;
	tokens_in_line = NULL;
	while (tokens[i])
	{
		tokens_in_line = ft_strjoin(tokens_in_line, tokens[i]);
		if (tokens[i + 1])
			tokens_in_line = ft_strjoin(tokens_in_line, " ");
		i++;
	}
	return (tokens_in_line);
}

char *ft_getenv(char *var, t_list *environment)
{
	char *var_env;
	char *env;
	int i;

	while (environment)
	{
		i = 0;
		if (!environment->content)
		{
			environment = environment->next;
			continue;
		}
		while (((char *)environment->content)[i])
		{
			if (((char *)environment->content)[i] == '=')
				break;
			i++;
		}
		var_env = ft_substr((char *)environment->content, 0, i);
		env = ft_substr((char *)environment->content, i + 1, ft_strlen((char *)environment->content) - i - 1);
		if (ft_strlen(var) == ft_strlen(var_env) && !ft_strncmp(var, var_env, ft_strlen(var)))
			return (env);
		environment = environment->next;
	}
	return (NULL);
}

char *read_fd(int fd)
{
	char *file_content;
	char *buffer;
	ssize_t r;

	if (fd < 0)
		return (NULL);
	file_content = NULL;
	buffer = malloc(43);
	if (!buffer)
		return (NULL);
	buffer[0] = 0;
	while (1)
	{
		r = read(fd, buffer, 42);
		if (!r || r == -1)
			break;
		buffer[r] = 0;
		file_content = ft_strjoin(file_content, buffer);
	}
	return (free(buffer), file_content);
}

char *redirections(char *cmd_line, char **env, t_list *environment, t_list **export_list, short *cd_flag, int fd_pipe, char **old_pwd)
{
	int fd[2];
	int fd_output[2];
	int fd_file_output;
	int fd_file_input;
	pid_t pid;
	short is_there_output;
	char *cmd_args;
	char *cmd_redirection;
	char *input_herdoc;
	char *join_herdoc;
	char *path_cmd;
	char *output_cmd;
	char **tokens;
	char *pipe_output;
	char **tokens_redirections;
	t_redirections *redirectionst;
	int status;

	pipe_output = NULL;
	if (fd_pipe > 0)
		pipe_output = read_fd(fd_pipe);
	fd_file_output = 0;
	fd_file_input = 0;
	is_there_output = 0;
	join_herdoc = NULL;
	redirectionst = NULL;
	cmd_args = join_cmd_args(cmd_line);
	tokens = split_command(cmd_args, ' ', environment, 1);
	if (!tokens)
		return (NULL);
	if (is_exist_redirect_pipe(cmd_line, 'r'))
	{
		tokens_redirections = get_tokens_with_redirection(cmd_line);
		if (!tokens_redirections)
			return (NULL);
		redirectionst = list_redirections(tokens_redirections, environment);
		if (!redirectionst)
			return (NULL);
	}
	while (redirectionst)
	{
		if (ft_strlen(redirectionst->type_redirection) == 1 && !ft_strncmp(redirectionst->type_redirection, ">", 1))
		{
			is_there_output = 1;
			fd_file_output = open(redirectionst->file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			if (fd_file_output < 0)
			{
				printf(BLUE "minishell: %sambiguous redirect.%s\n", RED, DEF);
				return (NULL);
			}
		}
		else if (ft_strlen(redirectionst->type_redirection) == 2 && !ft_strncmp(redirectionst->type_redirection, ">>", 2))
		{
			is_there_output = 1;
			fd_file_output = open(redirectionst->file_name, O_CREAT | O_APPEND | O_WRONLY, 0644);
			if (fd_file_output < 0)
			{
				printf(BLUE "minishell: %sambiguous redirect.%s\n", RED, DEF);
				return (NULL);
			}
		}
		else if (ft_strlen(redirectionst->type_redirection) == 2 && !ft_strncmp(redirectionst->type_redirection, "<<", 2))
		{
			join_herdoc = NULL;
			while (1)
			{
				input_herdoc = readline(CYAN "heredoc > " DEF);
				if (!input_herdoc)
				{
					if (!join_herdoc)
						join_herdoc = ft_strdup("");
					break;
				}
				if (ft_strlen(input_herdoc) == ft_strlen(redirectionst->file_name) && !ft_strncmp(input_herdoc, redirectionst->file_name, ft_strlen(input_herdoc)))
				{
					if (!join_herdoc)
						join_herdoc = ft_strdup("");
					break;
				}
				join_herdoc = ft_strjoin(join_herdoc, ft_dollar(input_herdoc, environment));
				join_herdoc = ft_strjoin(join_herdoc, "\n");
			}
		}
		else if (ft_strlen(redirectionst->type_redirection) == 1 && !ft_strncmp(redirectionst->type_redirection, "<", 1))
		{
			join_herdoc = NULL;
			if (!ft_strlen(redirectionst->file_name))
			{
				printf(BLUE "minishell: %sambiguous redirect.%s\n", RED, DEF);
				return (NULL);
			}
			fd_file_input = open(redirectionst->file_name, O_RDONLY);
			if (fd_file_input < 0)
			{
				printf(RED "minishell: %s%s%s: No such file or directory.\n", BLUE, redirectionst->file_name, DEF);
				return (NULL);
			}
		}
		redirectionst = redirectionst->next;
	}
	if ((ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], ">>", 2)) || (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "<<", 2)) || (ft_strlen(tokens[0]) == 1 && !ft_strncmp(tokens[0], ">", 1)) || (ft_strlen(tokens[0]) == 1 && !ft_strncmp(tokens[0], "<", 1)))
		return (NULL);
	else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
		exit_cmd();
	else if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6) && tokens[1])
		export_cmd(env, tokens, export_list);
	else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		unset_cmd(tokens, env, export_list);
	else if (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2) && fd_pipe < 0)
	{
		*old_pwd = pwd_cmd(0);
		cd_cmd(tokens[1], cd_flag);
		return (NULL);
	}
	path_cmd = is_there_cmd(tokens, environment);
	if (!path_cmd)
		return (NULL);
	pipe(fd);
	pipe(fd_output);
	if (join_herdoc)
		write(fd[1], join_herdoc, ft_strlen(join_herdoc));
	else if (fd_pipe > 0)
	{
		if (pipe_output)
			write(fd[1], pipe_output, ft_strlen(pipe_output));
		else
			write(fd[1], "", 0);
	}
	close(fd[1]);
	pid = fork();
	if (!pid)
	{
		if (join_herdoc)
			dup2(fd[0], 0);
		else if (fd_file_input > 0)
			dup2(fd_file_input, 0);
		else if (fd_pipe > 0)
			dup2(fd[0], 0);
		dup2(fd_output[1], 1);
		close(fd[0]);
		close(fd_output[0]);
		if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
			echo_cmd(tokens);
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
			pwd_cmd(1);
		else if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6) && !tokens[1])
			export_cmd(env, tokens, export_list);
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
			env_cmd(env, *export_list, 1);
		else
			execve(path_cmd, tokens, env);
		exit(0);	
	}
	else
	{
		waitpid(pid, &status, 0);
		ft_status(status, 1);
		close(fd[0]);
		close(fd_output[1]);
	}
	output_cmd = read_fd(fd_output[0]);
	close(fd_output[0]);
	if (fd_file_output > 0 && output_cmd)
		write(fd_file_output, output_cmd, ft_strlen(output_cmd));
	if (fd_pipe > 0)
		return (NULL);
	return (output_cmd);
}

short is_empty_token(char *token)
{
	size_t i;

	i = 0;
	while (token[i])
	{
		if (token[i] != ' ')
			return (0);
		i++;
	}
	return (1);
}

void ft_pipe(char *cmd_line, t_list *environment, char **env, t_list **export_list, short *cd_flag)
{
	int i = 0;
	char check_end_pipe = cmd_line[ft_strlen(cmd_line) - 1];
	if (check_end_pipe == '|')
	{
		printf(BLUE "minishell:%s %ssyntax error in pipe.\n" DEF, DEF, RED);
		return;
	}
	char **split_pipe = split_commmand_with_quotes(cmd_line, '|', environment, 0);
	while (split_pipe[i])
	{
		if (is_empty_token(split_pipe[i]))
		{
			printf(BLUE "minishell:%s %ssyntax error in pipe.\n" DEF, DEF, RED);
			return;
		}
		i++;
	}
	i = 0;
	char **tokens;
	char *redirect_output;
	int fd[2];
	int fd_error[2];
	char *join_errors;
	int in_fd = 0;
	char *ou;

	join_errors = NULL;
	int status;
	while (split_pipe[i])
	{
		ou = NULL;
		redirect_output = NULL;
		int exits_redirect = 0;
		pipe(fd);
		pipe(fd_error);
		tokens = split_command(split_pipe[i], ' ', environment, 1);
		if (is_exist_redirect_pipe(split_pipe[i], 'r'))
			exits_redirect = 1;
		else if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6) && tokens[1])
			export_cmd(env, tokens, export_list);
		else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
			unset_cmd(tokens, env, export_list);
		if (exits_redirect)
			ou = redirections(split_pipe[i], env, environment, export_list, cd_flag, in_fd, NULL);
		int pid = fork();
		if (pid == 0)
		{
			dup2(in_fd, 0);
			if (split_pipe[i + 1])
				dup2(fd[1], 1);
			if (split_pipe[i + 1])
				dup2(fd_error[1], 2);
			close(fd[0]);
			close(fd_error[0]);
			if (exits_redirect)
			{
				if (ou)
					printf("%s", ou);
			}
			else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
				exit_cmd();
			else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
				echo_cmd(tokens);
			else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
				pwd_cmd(1);
			else if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6) && !tokens[1])
				export_cmd(env, tokens, export_list);
			else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
				env_cmd(env, *export_list, 1);
			else
				execve(is_there_cmd(tokens, environment), tokens, env);
			exit(0);
		}
		else
		{
			waitpid(pid, &status, 0);
			ft_status(status, 1);
			close(fd[1]);
			close(fd_error[1]);
			in_fd = fd[0];
		}
		char *er = read_fd(fd_error[0]);
		if (er)
			join_errors = ft_strjoin(join_errors, er);
		i++;
	}
	if (join_errors)
		printf("%s", join_errors);
}

int main(int argc, char **argv, char **env)
{
	struct stat file;
	struct termios ctr;
	t_list *export_list;
	t_list *environment;
	int i;
	int status;
	pid_t pid;
	char *cmd_line;
	char **tokens;
	char *pwd;
	char *old_pwd;
	char **path;
	char *this_dir;
	char *path_cmd;
	char *herdoc_output;
	short cd_flag;
	char *redirection_output;

	if (argc != 1)
		return (printf(RED "Please do not enter any arguments.\n" DEF), 1);
	(void)argc;
	(void)argv;
	tcgetattr(0, &ctr);
	ctr.c_lflag &= ~ECHOCTL;
	tcsetattr(0, 0, &ctr);
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
	export_list = NULL;
	herdoc_output = NULL;
	old_pwd = NULL;
	status = 0;
	cd_flag = 0;
	pid = fork();
	if (!pid)
		execve("/usr/bin/clear", (char *[]){"clear", NULL}, env);
	waitpid(pid, &status, 0);
	printf(YELLOW "↪ Welcome to our MiniShell 🤪 ↩\n" DEF);
	while (1)
	{
		i = 0;
		environment = all_env(NULL, NULL, env, export_list, 0, 0, &cd_flag, old_pwd);
		pwd = pwd_cmd(0);
		path = ft_split(pwd, '/');
		while (path[i])
			this_dir = path[i++];
		this_dir = ft_strjoin("\033[1;94m", this_dir);
		this_dir = ft_strjoin(this_dir, "\033[0m");
		this_dir = ft_strjoin("\033[32m➥\033[0m ", this_dir);
		this_dir = ft_strjoin(this_dir, " ");
		cmd_line = readline(this_dir);
		if (!cmd_line)
			exit_cmd();
		if (!cmd_line[0])
			continue;
		add_history(cmd_line);
		if (is_exist_redirect_pipe(cmd_line, '|'))
		{
			ft_pipe(cmd_line, environment, env, &export_list, &cd_flag);
			continue;
		}
		else if (is_exist_redirect_pipe(cmd_line, 'r'))
		{
			redirection_output = redirections(cmd_line, env, environment, &export_list, &cd_flag, -1, &old_pwd);
			if (redirection_output)
				printf("%s", redirection_output);
			continue;
		}
		tokens = split_command(cmd_line, ' ', environment, 1);
		if (!tokens || !tokens[0])
			continue;
		if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
		{
			printf(RED "exit\n" DEF);
			break;
		}
		else if (stat(tokens[0], &file) == 0 && S_ISDIR(file.st_mode))
		{
			printf(RED "minishell: %s%s%s is a directory.\n", BLUE, tokens[0], DEF);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6))
		{
			export_cmd(env, tokens, &export_list);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		{
			unset_cmd(tokens, env, &export_list);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
		{
			env_cmd(env, export_list, 1);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
		{
			printf("%s\n", pwd);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		{
			echo_cmd(tokens);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2))
		{
			old_pwd = pwd;
			cd_cmd(tokens[1], &cd_flag);
			continue;
		}
		path_cmd = is_there_cmd(tokens, environment);
		if (!path_cmd)
			continue;
		pid = fork();
		if (pid == 0)
		{
			execve(path_cmd, tokens, env);
			printf(RED "minishell: %s%s%s: %s.\n", BLUE, tokens[0], DEF, strerror(errno));
			exit(0);
		}
		else
		{
			g_signle_flag = pid;
			waitpid(pid, &status, 0);
			ft_status(status, 1);
			g_signle_flag = 0;
		}
		free(cmd_line);
	}
	return (0);
}
