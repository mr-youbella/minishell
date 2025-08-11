/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 19:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/08/09 23:13:17 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void handle_signal(int sig)
{
	(void)sig;
	printf("\n");
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
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

char *redirections(char *cmd_line, char **env, int *status, int pipe_fd, short is_return, t_list *environment)
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
	char *pipe_input;
	char **tokens;
	t_redirections *redirections_output;
	t_redirections *redirections_input;

	fd_file_output = 0;
	fd_file_input = 0;
	is_there_output = 0;
	join_herdoc = NULL;
	redirections_output = NULL;
	redirections_input = NULL;
	pipe_input = read_fd(pipe_fd);
	// cmd_args = join_cmd_args(cmd_line);
	// cmd_redirection = join_cmd_redirections(cmd_line);
	cmd_args = ft_strdup("ls");
	cmd_redirection = ft_strdup("ls > $name");
	tokens = ft_split_first_cmd(cmd_args, ' ', *status, environment, 0);
	if (is_exist_redirect_pipe(cmd_redirection, 'o'))
	{
		redirections_output = list_redirections(ft_split_first_cmd(cmd_redirection, ' ', 0, environment, 0), 'o');
		if (!redirections_output)
			return (NULL);
	}
	if (is_exist_redirect_pipe(cmd_redirection, 'i'))
	{
		redirections_input = list_redirections(ft_split_first_cmd(cmd_redirection, ' ', 0, environment, 0), 'i');
		if (!redirections_input)
			return (NULL);
	}
	while (redirections_output)
	{
		is_there_output = 1;
		if (ft_strlen(redirections_output->type_redirection) == 1 && !ft_strncmp(redirections_output->type_redirection, ">", 1))
			fd_file_output = open(redirections_output->file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644);
		else
			fd_file_output = open(redirections_output->file_name, O_CREAT | O_APPEND | O_WRONLY, 0644);
		redirections_output = redirections_output->next;
	}
	while (redirections_input)
	{
		pipe_input = NULL;
		join_herdoc = NULL;
		if (ft_strlen(redirections_input->type_redirection) == 2 && !ft_strncmp(redirections_input->type_redirection, "<<", 2))
		{
			while (1)
			{
				input_herdoc = readline(CYAN "heredoc > " DEF);
				if (!input_herdoc)
				{
					join_herdoc = NULL;
					break;
				}
				if (ft_strlen(input_herdoc) == ft_strlen(redirections_input->file_name) && !ft_strncmp(input_herdoc, redirections_input->file_name, ft_strlen(input_herdoc)))
					break;
				join_herdoc = ft_strjoin(join_herdoc, input_herdoc);
				join_herdoc = ft_strjoin(join_herdoc, "\n");
			}
		}
		else
		{
			fd_file_input = open(redirections_input->file_name, O_RDONLY);
			if (fd_file_input < 0)
			{
				printf(RED "minishell: %s%s%s: No such file or directory.\n", BLUE, redirections_input->file_name, DEF);
				return (NULL);
			}
		}
		redirections_input = redirections_input->next;
	}
	path_cmd = is_there_cmd(tokens, environment, status);
	if (!path_cmd)
		return (NULL);
	pipe(fd);
	pipe(fd_output);
	if (pipe_input)
		write(fd[1], pipe_input, ft_strlen(pipe_input));
	else if (join_herdoc)
		write(fd[1], join_herdoc, ft_strlen(join_herdoc));
	close(fd[1]);
	pid = fork();
	if (!pid)
	{
		if (join_herdoc || pipe_input)
			dup2(fd[0], 0);
		if (fd_file_input > 0)
			dup2(fd_file_input, 0);
		if (is_there_output || is_return)
			dup2(fd_output[1], 1);
		close(fd[0]);
		close(fd_output[0]);
		execve(path_cmd, tokens, env);
		exit(1);
	}
	else
	{
		waitpid(pid, status, 0);
		close(fd[0]);
		close(fd_output[1]);
	}
	if (is_there_output || is_return)
	{
		output_cmd = read_fd(fd_output[0]);
		close(fd_output[0]);
		if (fd_file_output > 0 && output_cmd)
			write(fd_file_output, output_cmd, ft_strlen(output_cmd));
	}
	return (output_cmd);
}

void ft_pipe(char *cmd_line, int status, t_list *environment, char **env, char *pwd)
{
	char **split_pipe = ft_split_first_cmd(cmd_line, '|', status, environment, 0);
	char **tokens;
	char *redirect_output;
	int i = 0;
	int fd[2];
	int in_fd = 0;

	while (split_pipe[i])
	{
		redirect_output = NULL;
		int exits_redirect = 0;
		pipe(fd);
		tokens = ft_split_first_cmd(split_pipe[i], ' ', status, environment, 0);
		if (is_exist_redirect_pipe(split_pipe[i], 'o') || is_exist_redirect_pipe(split_pipe[i], 'i'))
		{
			redirect_output = redirections(split_pipe[i], env, &status, in_fd, 1, environment);
			exits_redirect = 1;
		}
		if (redirect_output)
			write(fd[1], redirect_output, ft_strlen(redirect_output));
		int pid = fork();
		if (pid == 0)
		{
			dup2(in_fd, 0);
			if (split_pipe[i + 1])
				dup2(fd[1], 1);
			close(fd[0]);
			if (!exits_redirect)
				execve(is_there_cmd(tokens, environment, &i), tokens, env);
			exit(1);
		}
		else
		{
			wait(NULL);
			close(fd[1]);
			in_fd = fd[0];
		}
		i++;
	}
}

int main(int argc, char **argv, char **env)
{
	struct stat	file;
	struct sigaction sig;
	struct termios ctr;
	t_list *export_list;
	t_list *environment;
	int i;
	int status;
	pid_t pid;
	char *cmd_line;
	char **tokens;
	char *pwd;
	char **path;
	char *this_dir;
	char *path_cmd;
	char *herdoc_output;

	if (argc != 1)
		return (printf(RED "Please do not enter any arguments.\n" DEF), 1);
	(void)argc;
	(void)argv;
	tcgetattr(0, &ctr);
	ctr.c_lflag &= ~ECHOCTL;
	tcsetattr(0, 0, &ctr);
	sig.sa_handler = handle_signal;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sig, NULL);
	signal(SIGQUIT, SIG_IGN);
	export_list = NULL;
	herdoc_output = NULL;
	status = 0;
	pid = fork();
	if (!pid)
		execve("/usr/bin/clear", (char *[]){"clear", NULL}, env);
	waitpid(pid, &status, 0);
	printf(YELLOW "↪ Welcome to our MiniSheel 🤪 ↩\n" DEF);
	while (1)
	{
		i = 0;
		environment = all_env(NULL, NULL, env, export_list, 0);
		pwd = getcwd(NULL, 0);
		path = ft_split(pwd, '/');
		while (path[i])
			this_dir = path[i++];
		this_dir = ft_strjoin("\033[1;94m", this_dir);
		this_dir = ft_strjoin(this_dir, "\033[0m");
		this_dir = ft_strjoin("\033[32m➥\033[0m ", this_dir);
		this_dir = ft_strjoin(this_dir, " ");
		cmd_line = readline(this_dir);
		if (!cmd_line)
		{
			printf(RED "exit\n" DEF);
			break;
		}
		if (!cmd_line[0])
			continue;
		add_history(cmd_line);
		tokens = ft_split_first_cmd(cmd_line, ' ', WEXITSTATUS(status), environment, 0);
		i = 0;
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
		else if (is_exist_redirect_pipe(cmd_line, '|'))
		{
			ft_pipe(cmd_line, status, environment, env, pwd);
			continue;
		}
		else if (is_exist_redirect_pipe(cmd_line, 'o') || is_exist_redirect_pipe(cmd_line, 'i'))
		{
			redirections(cmd_line, env, &status, -1, 0, environment);
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
			all_env(NULL, NULL, env, export_list, 1);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
		{
			printf("%s\n", pwd);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		{
			echo_cmd(tokens, &status, 0);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2))
		{
			cd_cmd(tokens[1]);
			continue;
		}
		path_cmd = is_there_cmd(tokens, environment, &status);
		if (!path_cmd)
			continue;
		pid = fork();
		if (pid == 0)
		{	
			execve(path_cmd, tokens, env);
			printf(RED "minishell: %s%s%s: %s.\n", BLUE, tokens[0], DEF, strerror(errno));
			exit(1);
		}
		else
			wait(&status);
		free(cmd_line);
	}
	return (0);
}
