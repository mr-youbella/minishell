/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 04:45:56 by youbella          #+#    #+#             */
/*   Updated: 2025/08/20 04:57:28 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

short	g_signle_flag = 0;

int	ft_status(int status, short is_change)
{
	static int	new_status;

	if (is_change)
		new_status = status;
	return (WEXITSTATUS(new_status));
}

void	handle_signal(int sig_num)
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

char	*ft_getenv(char *var, t_list *environment, t_list **leaks)
{
	char	*var_env;
	char	*env;
	int		i;
	t_list	*new_leak;

	while (environment)
	{
		i = 0;
		if (!environment->content)
		{
			environment = environment->next;
			continue ;
		}
		while (((char *)environment->content)[i])
		{
			if (((char *)environment->content)[i] == '=')
				break ;
			i++;
		}
		var_env = ft_substr((char *)environment->content, 0, i);
		new_leak = ft_lstnew(var_env);
		ft_lstadd_back(leaks, new_leak);
		if (!var_env)
			return (NULL);
		env = ft_substr((char *)environment->content, i + 1, ft_strlen((char *)environment->content) - i - 1);
		new_leak = ft_lstnew(env);
		ft_lstadd_back(leaks, new_leak);
		if (!env)
			return (NULL);
		if (ft_strlen(var) == ft_strlen(var_env)
			&& !ft_strncmp(var, var_env, ft_strlen(var)))
			return (env);
		environment = environment->next;
	}
	return (NULL);
}

char	*read_fd(int fd)
{
	char	*file_content;
	char	*buffer;
	char	*tmp;
	ssize_t	r;

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
			break ;
		buffer[r] = 0;
		tmp = file_content;
		file_content = ft_strjoin(file_content, buffer);
		free(tmp);
		if (!file_content)
			return (NULL);
	}
	return (free(buffer), file_content);
}

char	*redirections(char *cmd_line, char **env, t_list *environment, t_list **export_list, short *cd_flag, int fd_pipe, char **old_pwd, t_list **leaks)
{
	int				fd[2];
	int				fd_output[2];
	pid_t			fd_file_output;
	t_list			*new_leak;
	pid_t			fd_file_input;
	pid_t			pid;
	char			*cmd_args;
	char			*input_herdoc;
	char			*input_herdoc_dollar;
	char			*join_herdoc;
	char			*path_cmd;
	char			*output_cmd;
	char			*tmp;
	char			**tokens;
	char			*pipe_output;
	char			**tokens_redirections;
	t_redirections	*redirections;
	t_redirections	*copy_redirections;
	int				status;
	size_t			i;
	char			*echo;

	pipe_output = NULL;
	if (fd_pipe > 0)
		pipe_output = read_fd(fd_pipe);
	fd_file_output = 0;
	fd_file_input = 0;
	join_herdoc = NULL;
	redirections = NULL;
	echo = NULL;
	cmd_args = join_cmd_args(cmd_line);
	tokens = split_command(cmd_args, ' ', environment, 1, leaks);
	i = 0;
	while (tokens && tokens[i])
	{
		new_leak = ft_lstnew(tokens[i]);
		ft_lstadd_back(leaks, new_leak);
		i++;
		if (!tokens[i])
		{
			new_leak = ft_lstnew(tokens);
			ft_lstadd_back(leaks, new_leak);
		}
	}
	free(cmd_args);
	if (!tokens)
		return (NULL);
	if (is_exist_redirect_pipe(cmd_line, 'r'))
	{
		tokens_redirections = get_tokens_with_redirection(cmd_line);
		if (!tokens_redirections)
			return (NULL);
		copy_redirections = list_redirections(tokens_redirections, environment, leaks);
		redirections = copy_redirections;
		i = 0;
		while (tokens_redirections && tokens_redirections[i])
		{
			new_leak = ft_lstnew(tokens_redirections[i]);
			ft_lstadd_back(leaks, new_leak);
			i++;
			if (!tokens_redirections[i])
			{
				new_leak = ft_lstnew(tokens_redirections);
				ft_lstadd_back(leaks, new_leak);
			}
		}
		while (copy_redirections)
		{
			new_leak = ft_lstnew(copy_redirections);
			ft_lstadd_back(leaks, new_leak);
			new_leak = ft_lstnew(copy_redirections->file_name);
			ft_lstadd_back(leaks, new_leak);
			copy_redirections = copy_redirections->next;
		}
		if (!redirections)
			return (NULL);
	}
	while (redirections)
	{
		if (ft_strlen(redirections->type_redirection) == 1
			&& !ft_strncmp(redirections->type_redirection, ">", 1))
		{
			fd_file_output = open(redirections->file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			if (fd_file_output < 0)
			{
				printf(BLUE "minishell: %sambiguous redirect.%s\n", RED, DEF);
				return (NULL);
			}
		}
		else if (ft_strlen(redirections->type_redirection) == 2
			&& !ft_strncmp(redirections->type_redirection, ">>", 2))
		{
			fd_file_output = open(redirections->file_name, O_CREAT | O_APPEND | O_WRONLY, 0644);
			if (fd_file_output < 0)
			{
				printf(BLUE "minishell: %sambiguous redirect.%s\n", RED, DEF);
				return (NULL);
			}
		}
		else if (ft_strlen(redirections->type_redirection) == 2
			&& !ft_strncmp(redirections->type_redirection, "<<", 2))
		{
			free(join_herdoc);
			join_herdoc = NULL;
			while (1)
			{
				input_herdoc = readline(CYAN "heredoc > " DEF);
				if (!input_herdoc)
				{
					if (!join_herdoc)
						join_herdoc = ft_strdup("");
					break ;
				}
				if (ft_strlen(input_herdoc) == ft_strlen(redirections->file_name) && !ft_strncmp(input_herdoc, redirections->file_name, ft_strlen(input_herdoc)))
				{
					free(input_herdoc);
					if (!join_herdoc)
						join_herdoc = ft_strdup("");
					break ;
				}
				input_herdoc_dollar = ft_dollar(input_herdoc, environment, leaks);
				tmp = join_herdoc;
				join_herdoc = ft_strjoin(join_herdoc, input_herdoc_dollar);
				free(input_herdoc_dollar);
				free(tmp);
				tmp = join_herdoc;
				join_herdoc = ft_strjoin(join_herdoc, "\n");
				free(tmp);
				free(input_herdoc);
			}
		}
		else if (ft_strlen(redirections->type_redirection) == 1
			&& !ft_strncmp(redirections->type_redirection, "<", 1))
		{
			join_herdoc = NULL;
			if (!ft_strlen(redirections->file_name))
			{
				printf(BLUE "minishell: %sambiguous redirect.%s\n", RED, DEF);
				return (NULL);
			}
			fd_file_input = open(redirections->file_name, O_RDONLY);
			if (fd_file_input < 0)
			{
				printf(RED "minishell: %s%s%s: No such file or directory.\n",
					BLUE, redirections->file_name, DEF);
				return (NULL);
			}
		}
		redirections = redirections->next;
	}
	if ((ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], ">>", 2))
		|| (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "<<", 2))
		|| (ft_strlen(tokens[0]) == 1 && !ft_strncmp(tokens[0], ">", 1))
		|| (ft_strlen(tokens[0]) == 1 && !ft_strncmp(tokens[0], "<", 1)))
		return (NULL);
	else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
		exit_cmd();
	else if (ft_strlen(tokens[0]) == 6
		&& !ft_strncmp(tokens[0], "export", 6) && tokens[1])
		export_cmd(env, tokens, export_list, NULL);
	else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		unset_cmd(tokens, env, export_list, NULL);
	else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		echo = echo_cmd(tokens, 1);
	else if (ft_strlen(tokens[0]) == 2
		&& !ft_strncmp(tokens[0], "cd", 2) && fd_pipe < 0)
	{
		*old_pwd = pwd_cmd(0);
		cd_cmd(tokens[1], cd_flag);
		return (NULL);
	}
	path_cmd = is_there_cmd(tokens, environment, leaks);
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
		close(fd[1]);
		close(fd_output[1]);
		if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
			printf("%s", echo);
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
			pwd_cmd(1);
		else if (ft_strlen(tokens[0]) == 6
			&& !ft_strncmp(tokens[0], "export", 6) && !tokens[1])
			export_cmd(env, tokens, export_list, leaks);
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
			env_cmd(env, *export_list, 1, NULL);
		else
			execve(path_cmd, tokens, env);
		exit(0);
	}
	else
	{
		waitpid(pid, &status, 0);
		free(join_herdoc);
		free(path_cmd);
		free(echo);
		ft_status(status, 1);
		close(fd[1]);
		close(fd_output[1]);
	}
	output_cmd = read_fd(fd_output[0]);
	close(fd[0]);
	close(fd_output[0]);
	if (fd_file_input > 0)
    	close(fd_file_input);
	if (fd_file_output > 0 && output_cmd)
	{
		write(fd_file_output, output_cmd, ft_strlen(output_cmd));
		free(output_cmd);
		close(fd_file_output);
		return (NULL);
	}
	if (fd_file_output > 0)
		close(fd_file_output);
	if (fd_pipe > 0)
		return (NULL);
	return (output_cmd);
}

short	is_empty_token(char *token)
{
	size_t	i;

	i = 0;
	while (token[i])
	{
		if (token[i] != ' ')
			return (0);
		i++;
	}
	return (1);
}

void	ft_pipe(char *cmd_line, t_list *environment, char **env, t_list **export_list, short *cd_flag, t_list **leaks)
{
	size_t	i;
	size_t	j;
	int		status;
	int		fd[2];
	int		fd_error[2];
	int		in_fd;
	int		exits_redirect;
	int		pid;
	char	check_end_pipe;
	char	**tokens;
	char	**split_pipe;
	char	*join_errors;
	char	*error;
	char	*tmp;
	char	*redirect_output;
	char	*echo;

	i = 0;
	in_fd = 0;
	join_errors = NULL;
	echo = NULL;
	check_end_pipe = cmd_line[ft_strlen(cmd_line) - 1];
	if (cmd_line[0] == '|' || check_end_pipe == '|')
	{
		printf(BLUE "minishell:%s %ssyntax error in pipe.\n" DEF, DEF, RED);
		return ;
	}
	split_pipe = split_commmand_with_quotes(cmd_line, '|', environment, 0, leaks);
	while (split_pipe[i])
	{
		if (is_empty_token(split_pipe[i]))
		{
			printf(BLUE "minishell:%s %ssyntax error in pipe.\n" DEF, DEF, RED);
			return ;
		}
		i++;
	}
	i = 0;
	while (split_pipe[i])
	{
		redirect_output = NULL;
		exits_redirect = 0;
		pipe(fd);
		pipe(fd_error);
		tokens = split_command(split_pipe[i], ' ', environment, 1, leaks);
		if (is_exist_redirect_pipe(split_pipe[i], 'r'))
			exits_redirect = 1;
		else if (ft_strlen(tokens[0]) == 6
			&& !ft_strncmp(tokens[0], "export", 6) && tokens[1])
			export_cmd(env, tokens, export_list, leaks);
		else if (ft_strlen(tokens[0]) == 5
			&& !ft_strncmp(tokens[0], "unset", 5))
			unset_cmd(tokens, env, export_list, leaks);
		else if (ft_strlen(tokens[0]) == 4
			&& !ft_strncmp(tokens[0], "echo", 4))
			echo_cmd(tokens, 0);
		if (exits_redirect)
			redirect_output = redirections(split_pipe[i], env, environment, export_list, cd_flag, in_fd, NULL, leaks);
		pid = fork();
		if (pid == 0)
		{
			dup2(in_fd, 0);
			if (split_pipe[i + 1])
			{
				dup2(fd[1], 1);
				dup2(fd_error[1], 2);
			}
			close(fd[0]);
			close(fd_error[0]);
			close(fd[1]);
			close(fd_error[1]);
			if (exits_redirect)
			{
				if (redirect_output)
					printf("%s", redirect_output);
			}
			else if (ft_strlen(tokens[0]) == 4
				&& !ft_strncmp(tokens[0], "exit", 4))
				exit_cmd();
			else if (ft_strlen(tokens[0]) == 4
				&& !ft_strncmp(tokens[0], "echo", 4))
				printf("%s", echo);
			else if (ft_strlen(tokens[0]) == 3
				&& !ft_strncmp(tokens[0], "pwd", 3))
				pwd_cmd(1);
			else if (ft_strlen(tokens[0]) == 6
				&& !ft_strncmp(tokens[0], "export", 6) && !tokens[1])
				export_cmd(env, tokens, export_list, leaks);
			else if (ft_strlen(tokens[0]) == 3
				&& !ft_strncmp(tokens[0], "env", 3))
				env_cmd(env, *export_list, 1, leaks);
			else
				execve(is_there_cmd(tokens, environment, leaks), tokens, env);
			exit(0);
		}
		else
		{
			waitpid(pid, &status, 0);
			ft_status(status, 1);
			close(fd[1]);
			close(fd_error[1]);
			if (in_fd > 0)
				close(in_fd);
			in_fd = fd[0];
			free(echo);
		}
		j = 0;
		while (tokens[j])
			free(tokens[j++]);
		free(tokens);
		error = read_fd(fd_error[0]);
		if (error)
		{
			tmp = join_errors;
			join_errors = ft_strjoin(join_errors, error);
			free(tmp);
			free(error);
		}
		close(fd_error[0]);
		i++;
	}
	j = 0;
	while (split_pipe[j])
		free(split_pipe[j++]);
	free(split_pipe);
	if (join_errors)
		printf("%s", join_errors);
	free(join_errors);
	if (in_fd > 0)
		close(in_fd);
}

// void	f()
// {
// 	system("leaks minishell");
// }

int	main(int argc, char **argv, char **env)
{
	// atexit(f);
	struct stat		file;
	struct termios	ctr;
	t_list			*leaks;
	t_list			*export_list;
	t_list			*environment;
	t_list			*all_environment;
	t_list			*new_leak;
	t_list			*tmp;
	short			cd_flag;
	int				i;
	int				status;
	pid_t			pid;
	char			*cmd_line;
	char			**tokens;
	char			*pwd;
	char			*old_pwd;
	char			**path;
	char			*this_dir;
	char			*this_dir_tmp;
	char			*path_cmd;
	char			*redirection_output;

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
	old_pwd = NULL;
	leaks = NULL;
	tokens = NULL;
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
		all_environment = all_env(NULL, NULL, env, export_list, 0, 0, &cd_flag, old_pwd, &leaks);
		environment = all_environment;
		while (all_environment)
		{
			new_leak = ft_lstnew(all_environment);
			ft_lstadd_back(&leaks, new_leak);
			all_environment = all_environment->next;
		}
		pwd = pwd_cmd(0);
		new_leak = ft_lstnew(pwd);
		ft_lstadd_back(&leaks, new_leak);
		path = ft_split(pwd, '/');
		while (path[i])
		{
		    this_dir = path[i];
		    i++;
		}
		this_dir = ft_strdup(this_dir);
		i = 0;
		while (path[i])
		{
		    free(path[i]);
		    i++;
		}
		free(path);
		this_dir_tmp = ft_strjoin("\033[1;94m", this_dir);
		free(this_dir);
		this_dir = this_dir_tmp;
		this_dir_tmp = ft_strjoin(this_dir, "\033[0m");
		free(this_dir);
		this_dir = this_dir_tmp;
		this_dir_tmp = ft_strjoin("\033[32m➥\033[0m ", this_dir);
		free(this_dir);
		this_dir = this_dir_tmp;
		this_dir_tmp = ft_strjoin(this_dir, " ");
		free(this_dir);
		this_dir = this_dir_tmp;
		cmd_line = readline(this_dir);
		new_leak = ft_lstnew(cmd_line);
		ft_lstadd_back(&leaks, new_leak);
		free(this_dir);
		if (!cmd_line)
		{
			while (leaks)
			{
				free(leaks->content);
				tmp = leaks;
				leaks = leaks->next;
				free(tmp);
			}
			while (export_list)
			{
				tmp = export_list;
				export_list = export_list->next;
				free(tmp);
			}
			// exit_cmd();
			return (0);
		}
		if (!cmd_line[0])
			continue ;
		add_history(cmd_line);
		tokens = split_command(cmd_line, ' ', environment, 1, &leaks);
		if (!tokens || !tokens[0])
			continue ;
		i = 0;
		while (tokens[i])
		{
			new_leak = ft_lstnew(tokens[i]);
			ft_lstadd_back(&leaks, new_leak);
			i++;
			if (!tokens[i])
			{
				new_leak = ft_lstnew(tokens);
				ft_lstadd_back(&leaks, new_leak);
			}
		}
		if (is_exist_redirect_pipe(cmd_line, '|'))
		{
			ft_pipe(cmd_line, environment, env, &export_list, &cd_flag, &leaks);
			continue ;
		}
		else if (is_exist_redirect_pipe(cmd_line, 'r'))
		{
			redirection_output = redirections(cmd_line, env, environment, &export_list, &cd_flag, -1, &old_pwd, &leaks);
			if (redirection_output)
				printf("%s", redirection_output);
			continue ;
		}
		if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
		{
			printf(RED "exit\n" DEF);
			break ;
		}
		else if (stat(tokens[0], &file) == 0 && S_ISDIR(file.st_mode))
		{
			printf(RED "minishell: %s%s%s is a directory.\n",
				BLUE, tokens[0], DEF);
			continue ;
		}
		else if (ft_strlen(tokens[0]) == 6
			&& !ft_strncmp(tokens[0], "export", 6))
		{
			export_cmd(env, tokens, &export_list, &leaks);
			continue ;
		}
		else if (ft_strlen(tokens[0]) == 5
			&& !ft_strncmp(tokens[0], "unset", 5))
		{
			unset_cmd(tokens, env, &export_list, &leaks);
			continue ;
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
		{
			env_cmd(env, export_list, 1, &leaks);
			continue ;
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
		{
			printf("%s\n", pwd);
			continue ;
		}
		else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		{
			echo_cmd(tokens, 0);
			continue ;
		}
		else if (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2))
		{
			old_pwd = pwd;
			cd_cmd(tokens[1], &cd_flag);
			continue ;
		}
		path_cmd = is_there_cmd(tokens, environment, &leaks);
		if (!path_cmd)
			continue ;
		pid = fork();
		if (pid == 0)
		{
			execve(path_cmd, tokens, env);
			printf(RED "minishell: %s%s%s: %s.\n",
				BLUE, tokens[0], DEF, strerror(errno));
			exit(0);
		}
		else
		{
			free(path_cmd);
			g_signle_flag = pid;
			waitpid(pid, &status, 0);
			ft_status(status, 1);
			g_signle_flag = 0;
		}
	}
	return (0);
}
