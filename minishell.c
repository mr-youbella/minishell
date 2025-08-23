/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 04:45:56 by youbella          #+#    #+#             */
/*   Updated: 2025/08/23 13:16:10 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

short	g_signal_flag = 0;

int	ft_status(int status, short is_change)
{
	static int	new_status;

	if (is_change)
		new_status = status;
	return (new_status);
}

void	handle_signal(int sig_num)
{
	if (sig_num == SIGQUIT)
	{
		if (g_signal_flag > 0)
			g_signal_flag = SIGQUIT;
		else
		{
			rl_on_new_line();
            rl_redisplay();
		}
	}
	else if (g_signal_flag == 0 && sig_num == SIGINT)
	{
		ft_status(1, 1);
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

short	is_buitin_cmd(char *token)
{
	if ((ft_strlen(token) == 4 && !ft_strncmp(token, "echo", 4))
		|| (ft_strlen(token) == 2 && !ft_strncmp(token, "cd", 2))
		|| (ft_strlen(token) == 3 && (!ft_strncmp(token, "pwd", 3) || !ft_strncmp(token, "PWD", 3)))
		|| (ft_strlen(token) == 6 && !ft_strncmp(token, "export", 6))
		|| (ft_strlen(token) == 5 && !ft_strncmp(token, "unset", 5))
		|| (ft_strlen(token) == 3 && !ft_strncmp(token, "env", 3))
		|| (ft_strlen(token) == 4 && !ft_strncmp(token, "exit", 4)))
		return (1);
	return (0);
}

char	*ft_getenv(char *var, t_var *variables)
{
	char	*var_env;
	char	*env;
	int		i;
	t_list	*new_leak;
	t_list	*environment;

	environment = variables->environment;
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
		ft_lstadd_back(&variables->leaks, new_leak);
		if (!var_env)
			return (NULL);
		env = ft_substr((char *)environment->content, i + 1, ft_strlen((char *)environment->content) - i - 1);
		new_leak = ft_lstnew(env);
		ft_lstadd_back(&variables->leaks, new_leak);
		if (!env)
			return (NULL);
		if (ft_strlen(var) == ft_strlen(var_env) && !ft_strncmp(var, var_env, ft_strlen(var)))
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
		if (r <= 0)
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

char	*redirections(char *cmd_line, char **copy_env, int fd_pipe, t_var *variables)
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
	char			*export;
	char			*pwd;

	pipe_output = NULL;
	if (fd_pipe > 0)
		pipe_output = read_fd(fd_pipe);
	fd_file_input = 0;
	fd_file_output = 0;
	join_herdoc = NULL;
	redirections = NULL;
	echo = NULL;
	export = NULL;
	pwd = NULL;
	cmd_args = join_cmd_args(cmd_line);
	tokens = split_command(cmd_args, ' ', 1, variables);
	i = 0;
	while (tokens && tokens[i])
	{
		new_leak = ft_lstnew(tokens[i]);
		ft_lstadd_back(&variables->leaks, new_leak);
		i++;
		if (!tokens[i])
		{
			new_leak = ft_lstnew(tokens);
			ft_lstadd_back(&variables->leaks, new_leak);
		}
	}
	free(cmd_args);
	if (!tokens)
		return (NULL);
	tokens_redirections = get_tokens_with_redirection(cmd_line);
	i = 0;
	while (tokens_redirections && tokens_redirections[i])
	{
		new_leak = ft_lstnew(tokens_redirections[i]);
		ft_lstadd_back(&variables->leaks, new_leak);
		i++;
		if (!tokens_redirections[i])
		{
			new_leak = ft_lstnew(tokens_redirections);
			ft_lstadd_back(&variables->leaks, new_leak);
		}
	}
	if (!tokens_redirections)
		return (NULL);
	copy_redirections = list_redirections(tokens_redirections, variables);
	redirections = copy_redirections;
	while (copy_redirections)
	{
		new_leak = ft_lstnew(copy_redirections->file_name);
		ft_lstadd_back(&variables->leaks, new_leak);
		new_leak = ft_lstnew(copy_redirections);
		ft_lstadd_back(&variables->leaks, new_leak);
		copy_redirections = copy_redirections->next;
	}
	if (!redirections)
		return (NULL);
	while (redirections)
	{
		if (ft_strlen(redirections->type_redirection) == 1
			&& !ft_strncmp(redirections->type_redirection, ">", 1))
		{
			fd_file_output = open(redirections->file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			if (fd_file_output < 0)
			{
				write(2, "\033[34mmminishell: \033[31mambiguous redirect.\033[0m\n", ft_strlen("\033[34mmminishell: \033[31mambiguous redirect.\033[0m\n"));
				ft_status(1, 1);
				return (NULL);
			}
		}
		else if (ft_strlen(redirections->type_redirection) == 2
			&& !ft_strncmp(redirections->type_redirection, ">>", 2))
		{
			fd_file_output = open(redirections->file_name, O_CREAT | O_APPEND | O_WRONLY, 0644);
			if (fd_file_output < 0)
			{
				write(2, "\033[34mmminishell: \033[31mambiguous redirect.\033[0m\n", ft_strlen("\033[34mmminishell: \033[31mambiguous redirect.\033[0m\n"));
				ft_status(1, 1);
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
				input_herdoc_dollar = ft_dollar(input_herdoc, variables);
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
				write(2, "\033[34mmminishell: \033[31mambiguous redirect.\033[0m\n", ft_strlen("\033[34mmminishell: \033[31mambiguous redirect.\033[0m\n"));
				ft_status(1, 1);
				return (NULL);
			}
			fd_file_input = open(redirections->file_name, O_RDONLY);
			if (fd_file_input < 0)
			{
				write(2, "\033[31mminishell: \033[34m", ft_strlen("\033[31mminishell: \033[34m"));
				write(2, redirections->file_name, ft_strlen(redirections->file_name));
				write(2, "\033[0m: No such file or directory.\n", ft_strlen("\033[0m: No such file or directory.\n"));
				ft_status(1, 1);
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
		exit_cmd(copy_env, variables);
	else if (ft_strlen(tokens[0]) == 6
		&& !ft_strncmp(tokens[0], "export", 6))
		export = export_cmd(copy_env, tokens, 1, variables);
	else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		unset_cmd(tokens, copy_env, variables);
	else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		echo = echo_cmd(tokens, 1);
	else if (ft_strlen(tokens[0]) == 3 && (!ft_strncmp(tokens[0], "pwd", 3) || !ft_strncmp(tokens[0], "PWD", 3)))
		pwd = pwd_cmd(0);
	else if (ft_strlen(tokens[0]) == 2
		&& !ft_strncmp(tokens[0], "cd", 2) && fd_pipe < 0)
	{
		variables->old_pwd = ft_getenv("PWD", variables);
		cd_cmd(tokens[1], variables);
		return (NULL);
	}
	path_cmd = is_there_cmd(tokens, variables);
	if (!path_cmd && !is_buitin_cmd(tokens[0]))
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
		{
			if (echo)
				printf("%s", echo);
		}	
		else if (ft_strlen(tokens[0]) == 3 && (!ft_strncmp(tokens[0], "pwd", 3) || !ft_strncmp(tokens[0], "PWD", 3)))
			printf("%s\n", pwd);
		else if (ft_strlen(tokens[0]) == 6
			&& !ft_strncmp(tokens[0], "export", 6))
		{
			if (export)
				printf("%s", export);
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
			env_cmd(1, copy_env, variables);
		else
			execve(path_cmd, tokens, variables->env);
		exit(0);
	}
	else
	{
		g_signal_flag = pid;
		waitpid(pid, &status, 0);
		ft_status(WEXITSTATUS(status), 1);
		free(join_herdoc);
		free(path_cmd);
		free(echo);
		free(export);
		free(pwd);
		free(pipe_output);
		close(fd[1]);
		close(fd_output[1]);
		g_signal_flag = 0;
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
	fd_file_output = 0;
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

char	**get_tokens_pipe(char *cmd_line, t_var *variables)
{
	size_t	i;
	char	**split_pipe;
	char	check_end_pipe;

	i = 0;
	check_end_pipe = cmd_line[ft_strlen(cmd_line) - 1];
	if (cmd_line[0] == '|' || check_end_pipe == '|')
	{
		ft_putstr_fd("\033[34mminishell:\033[31m ", 2);
		ft_putstr_fd("syntax error in pipe.\n\033[0m", 2);
		return (ft_status(258, 1), NULL);
	}
	split_pipe = split_commmand_with_quotes(cmd_line, '|', 0, variables);
	while (split_pipe && split_pipe[i])
	{
		if (is_empty_token(split_pipe[i]))
		{
			ft_putstr_fd("\033[34mminishell:\033[31m ", 2);
			ft_putstr_fd("syntax error in pipe.\n\033[0m", 2);
			return (ft_status(258, 1), NULL);
		}
		i++;
	}
	return (split_pipe);
}

short	builtin_commands2(char **tokens, char **copy_env, t_var *variables)
{
	char	*pwd;

	if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
	{
		unset_cmd(tokens, copy_env, variables);
		return (ft_status(0, 0));
	}
	else if (ft_strlen(tokens[0]) == 3
		&& (!ft_strncmp(tokens[0], "pwd", 3)
			|| !ft_strncmp(tokens[0], "PWD", 3)))
	{
		pwd = pwd_cmd(0);
		printf("%s\n", pwd);
		free(pwd);
		return (ft_status(0, 0));
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
		exit_cmd(copy_env, variables);
		return (ft_status(0, 0));
	}
	if (stat(tokens[0], &file) == 0 && S_ISDIR(file.st_mode))
	{
		ft_putstr_fd("\033[34mminishell: \033[31m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m is a directory.\n", 2);
		ft_status(126, 1);
		return (ft_status(0, 0));
	}
	if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6))
	{
		export_cmd(copy_env, tokens, 0, variables);
		return (ft_status(0, 0));
	}
	if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
		return (env_cmd(1, copy_env, variables), ft_status(0, 0));
	return (builtin_commands2(tokens, copy_env, variables));
}

void	execute_cmd_pipe(char **tokens, char **copy_env, t_var *variables)
{
	char	*path;
	int		status;

	if (!tokens)
		return ;
	status = builtin_commands(tokens, copy_env, variables);
	if (status)
		exit(status);
	path = is_there_cmd(tokens, variables);
	if (path && !is_buitin_cmd(tokens[0]))
	{
		execve(path, tokens, variables->env);
		ft_putstr_fd("\033[31mminishell: \033[34m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m: ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd(".\n", 2);
	}
	exit(1);
}

void	mange_pipes(int *pipe_fd, size_t i, size_t tokens_count)
{
	size_t	j;

	if (i)
		dup2(pipe_fd[(i - 1) * 2], 0);
	if (i != tokens_count - 1)
		dup2(pipe_fd[i * 2 + 1], 1);
	j = 0;
	while (j < 2 * (tokens_count - 1))
		close(pipe_fd[j++]);
}

void	end_pipe(size_t tokens_count, int *pipe_fd, pid_t pid)
{
	size_t	i;
	int		status;

	i = 0;
	while (i < 2 * (tokens_count - 1))
		close(pipe_fd[i++]);
	i = 0;
	waitpid(pid, &status, 0);
	ft_status(WEXITSTATUS(status), 1);
	while (i < tokens_count)
	{
		wait(NULL);
		i++;
	}
}

void	close_fdpipe(pid_t pid, size_t i, size_t tokens_count, int *pipe_fd)
{
	if (pid > 0)
	{
		if (i != tokens_count - 1)
			close(pipe_fd[i * 2 + 1]);
		if (i != 0)
			close(pipe_fd[(i - 1) * 2]);
	}
}

int	child_pipe(size_t i, size_t tokens_count, int *pipe_fd, t_var_pipe *varpipe)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		mange_pipes(pipe_fd, i, tokens_count);
		if (is_exist_redirect_pipe(varpipe->split_pipe[i], 'r'))
		{
			if (varpipe->redirections_output)
				printf("%s", varpipe->redirections_output);
			exit(0);
		}
		execute_cmd_pipe(varpipe->tokens,
			varpipe->variables->copy_env, varpipe->variables);
	}
	close_fdpipe(pid, i, tokens_count, pipe_fd);
	return (pid);
}

pid_t	while_pipe(char **split_pipe, int *pipe_fd,
					size_t tokens_count, t_var *variables)
{
	size_t		i;
	pid_t		pid;
	int			in_fd;
	t_var_pipe	*varpipe;

	varpipe = malloc(sizeof(t_var_pipe));
	varpipe->split_pipe = split_pipe;
	varpipe->variables = variables;
	1 && (i = 0, in_fd = 0);
	while (i < tokens_count)
	{
		if (i)
			in_fd = pipe_fd[(i - 1) * 2];
		varpipe->tokens = split_command(split_pipe[i], ' ', 1, variables);
		if (is_exist_redirect_pipe(split_pipe[i], 'r'))
			varpipe->redirections_output = redirections(split_pipe[i],
					variables->copy_env, in_fd, variables);
		pid = child_pipe(i, tokens_count, pipe_fd, varpipe);
		i++;
	}
	return (free(varpipe), pid);
}

void	ft_pipe_loop(char **split_pipe, t_var *variables)
{
	size_t	i;
	pid_t	pid;
	size_t	tokens_count;
	int		*pipe_fd;

	tokens_count = 0;
	while (split_pipe[tokens_count])
		tokens_count++;
	pipe_fd = malloc(sizeof(int) * 2 * (tokens_count - 1));
	if (!pipe_fd)
		return ;
	i = 0;
	while (i < tokens_count - 1)
	{
		pipe(pipe_fd + i * 2);
		i++;
	}
	i = 0;
	pid = while_pipe(split_pipe, pipe_fd, tokens_count, variables);
	end_pipe(tokens_count, pipe_fd, pid);
	return ;
}

void	ft_pipe(char *cmd_line, t_var *variables)
{
	size_t	i;
	char	**split_pipe;

	i = 0;
	split_pipe = get_tokens_pipe(cmd_line, variables);
	if (!split_pipe)
		return ;
	ft_pipe_loop(split_pipe, variables);
	while (split_pipe[i])
		free(split_pipe[i++]);
	free(split_pipe);
}

void	free_leaks(t_var *variables)
{
	t_list	*tmp;

	while (variables->leaks)
	{
		free(variables->leaks->content);
		tmp = variables->leaks;
		variables->leaks = variables->leaks->next;
		free(tmp);
	}
	while (variables->export_list)
	{
		tmp = variables->export_list;
		variables->export_list = variables->export_list->next;
		free(tmp);
	}
}

void	f(void)
{
	system("leaks minishell");
}

void	path_commands(char **tokens, char **copy_env,
						struct termios *ctr, char *path_cmd)
{
	pid_t	pid;
	int		status;

	if (!path_cmd)
		return ;
	pid = fork();
	if (pid == 0)
	{
		execve(path_cmd, tokens, copy_env);
		ft_putstr_fd("\033[34mminishell: \033[31m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m: ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd(".\n", 2);
		exit(1);
	}
	else
	{
		1 && (g_signal_flag = pid, waitpid(pid, &status, 0));
		ft_status(WEXITSTATUS(status), 1);
		if (g_signal_flag == SIGQUIT)
			1 && (printf("Quit: 3\n"), ft_status(131, 1));
		g_signal_flag = 0;
	}
	return (tcsetattr(0, 0, ctr), free(path_cmd));
}

short	redirections_pipe(char **tokens, char *cmd_line,
							char **copy_env, t_var *variables)
{
	char	*redirection_output;

	if (!tokens || !tokens[0])
		return (free(tokens), 1);
	if (is_exist_redirect_pipe(cmd_line, '|'))
	{
		ft_pipe(cmd_line, variables);
		return (1);
	}
	else if (is_exist_redirect_pipe(cmd_line, 'r'))
	{
		redirection_output = redirections(cmd_line, copy_env, -1, variables);
		if (redirection_output)
			printf("%s", redirection_output);
		return (1);
	}
	return (0);
}

void	free_array(char **arr, short is_stock, t_var *variables)
{
	size_t	i;
	t_list	*new_leak;

	i = 0;
	if (is_stock)
	{
		while (arr && arr[i])
		{
			new_leak = ft_lstnew(arr[i]);
			ft_lstadd_back(&variables->leaks, new_leak);
			i++;
			if (!arr[i])
			{
				new_leak = ft_lstnew(arr);
				ft_lstadd_back(&variables->leaks, new_leak);
			}
		}
	}
	else
	{
		while (arr && arr[i])
			free(arr[i++]);
		free(arr);
	}
}

char	*ft_readline(char *pwd)
{
	char	**path;
	char	*last_dir;
	char	*last_dir_tmp;
	char	*cmd_line;
	size_t	i;

	if (ft_strlen(pwd) == 1 && !ft_strncmp(pwd, "/", 1))
		last_dir = ft_strdup("/");
	else
	{
		1 && (i = 0, path = ft_split(pwd, '/'));
		while (path && path[i])
			last_dir = path[i++];
		last_dir = ft_strdup(last_dir);
		free_array(path, 0, NULL);
	}
	last_dir_tmp = ft_strjoin("\033[1;94m", last_dir);
	free(last_dir);
	last_dir = ft_strjoin(last_dir_tmp, "\033[0m");
	free(last_dir_tmp);
	last_dir_tmp = ft_strjoin("\033[32m➥\033[0m ", last_dir);
	free(last_dir);
	last_dir = ft_strjoin(last_dir_tmp, " ");
	free(last_dir_tmp);
	return (cmd_line = readline(last_dir), free(pwd), free(last_dir), cmd_line);
}

void	free_list(t_var *variables, t_list *list)
{
	t_list	*new_leak;

	while (list)
	{
		new_leak = ft_lstnew(list);
		ft_lstadd_back(&variables->leaks, new_leak);
		list = list->next;
	}
}

void	minishell_loop(t_var *variables, char **copy_env, struct termios *ctr)
{
	t_list	*new_leak;
	char	*command;
	char	**tokens;

	while (1)
	{
		variables->environment = all_env(NULL, NULL, copy_env, 0, 0, variables);
		free_list(variables, variables->environment);
		1 && (command = ft_readline(pwd_cmd(0)), new_leak = ft_lstnew(command));
		ft_lstadd_back(&variables->leaks, new_leak);
		if (!command)
			exit_cmd(copy_env, variables);
		if (!command[0])
			continue ;
		add_history(command);
		tokens = split_command(command, ' ', 1, variables);
		free_array(tokens, 1, variables);
		if (redirections_pipe(tokens, command, copy_env, variables)
			|| builtin_commands(tokens, copy_env, variables))
			continue ;
		path_commands(tokens, copy_env, ctr, is_there_cmd(tokens, variables));
	}
}

void	setup_terminal(struct termios *ctr)
{
	tcgetattr(0, ctr);
	ctr->c_lflag &= ~ECHOCTL;
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
}

char	**copy_environment(char	**env)
{
	size_t	i;
	char	**copy_env;

	if (!env)
		return (NULL);
	i = 0;
	while (env[i])
		i++;
	copy_env = malloc((i + 1) * sizeof(char *));
	if (!copy_env)
		return (NULL);
	i = 0;
	while (env[i])
	{
		copy_env[i] = env[i];
		i++;
	}
	copy_env[i] = NULL;
	return (copy_env);
}

int	main(int argc, char **argv, char **env)
{
	t_var			*variables;
	int				status;
	pid_t			pid;
	char			**copy_env;
	struct termios	ctr;

	// atexit(f);
	if (argc != 1 && argv)
		return (printf(RED "Please do not enter any arguments.\n" DEF), 1);
	status = 0;
	variables = malloc(sizeof(t_var));
	1 && (variables->cd_flag = 0, variables->env = env);
	1 && (variables->environment = NULL, variables->export_list = NULL);
	variables->leaks = NULL;
	variables->cd_flag = 0;
	copy_env = copy_environment(env);
	variables->copy_env = copy_env;
	setup_terminal(&ctr);
	pid = fork();
	if (!pid)
		execve("/usr/bin/clear", (char *[]){"clear", NULL}, env);
	waitpid(pid, &status, 0);
	ft_status(WEXITSTATUS(status), 1);
	printf(YELLOW "↪ Welcome to our MiniShell 🤪 ↩\n" DEF);
	minishell_loop(variables, copy_env, &ctr);
	return (0);
}
