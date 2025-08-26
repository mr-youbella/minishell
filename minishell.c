/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 04:45:56 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 01:30:25 by youbella         ###   ########.fr       */
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

void	handle_sigint(pid_t	pid)
{
	if (pid > 0)
	{
		printf("^\\Quit: 3\n");
		if (g_signal_flag)
			ft_status(131, 1);
	}
	else
	{
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	handle_signal(int sig_num)
{
	pid_t	pid;

	pid = wait(NULL);
	if (sig_num == SIGINT)
	{
		if (pid > 0)
		{
			if (g_signal_flag)
			{
				printf("^C\n");
				ft_status(130, 1);
			}
		}
		else
		{
			printf("\n");
			rl_on_new_line();
			rl_replace_line("", 0);
			rl_redisplay();
			ft_status(1, 1);
		}
	}
	else if (sig_num == SIGQUIT)
		handle_sigint(pid);
}

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

char	*check_env(char *var, t_list *environment, size_t i, t_var *variables)
{
	char	*var_env;
	t_list	*new_leak;
	char	*env;

	var_env = ft_substr((char *)environment->content, 0, i);
	new_leak = ft_lstnew(var_env);
	ft_lstadd_back(&variables->leaks, new_leak);
	if (!var_env)
		return (variables->is_return = 1, NULL);
	env = ft_substr((char *)environment->content, i + 1,
			ft_strlen((char *)environment->content) - i - 1);
	new_leak = ft_lstnew(env);
	ft_lstadd_back(&variables->leaks, new_leak);
	if (!env)
		return (variables->is_return = 1, NULL);
	if (ft_strlen(var) == ft_strlen(var_env)
		&& !ft_strncmp(var, var_env, ft_strlen(var)))
		return (variables->is_return = 1, env);
	return (variables->is_return = 0, NULL);
}

char	*ft_getenv(char *var, t_var *variables)
{
	char	*var_env;
	size_t	i;
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
		var_env = check_env(var, environment, i, variables);
		if (var_env && variables->is_return == 1)
			return (var_env);
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

void	free_list(t_var *variables, t_list *list, t_redirections *redirections)
{
	t_list	*new_leak;

	while (redirections)
	{
		new_leak = ft_lstnew(redirections->file_name);
		ft_lstadd_back(&variables->leaks, new_leak);
		new_leak = ft_lstnew(redirections);
		ft_lstadd_back(&variables->leaks, new_leak);
		redirections = redirections->next;
	}
	while (list)
	{
		new_leak = ft_lstnew(list);
		ft_lstadd_back(&variables->leaks, new_leak);
		list = list->next;
	}
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

short	end_herdoc(char *input_herdoc, char *file_name,
					t_var_redirect *var_redirection)
{
	if (ft_strlen(input_herdoc) == ft_strlen(file_name)
		&& !ft_strncmp(input_herdoc, file_name, ft_strlen(input_herdoc)))
	{
		free(input_herdoc);
		if (!var_redirection->join_herdoc)
			var_redirection->join_herdoc = ft_strdup("");
		return (0);
	}
	return (1);
}

void	join_herdoc(t_var_redirect *var_redirection, char *input_herdoc)
{
	char	*input_herdoc_dollar;
	char	*tmp;

	input_herdoc_dollar = ft_dollar(input_herdoc,
			var_redirection->variables, 0, 0);
	tmp = var_redirection->join_herdoc;
	var_redirection->join_herdoc = ft_strjoin(var_redirection->join_herdoc,
			input_herdoc_dollar);
	free(input_herdoc_dollar);
	free(tmp);
	tmp = var_redirection->join_herdoc;
	var_redirection->join_herdoc = ft_strjoin(var_redirection->join_herdoc,
			"\n");
	free(tmp);
	free(input_herdoc);
}

void	herdoc(char *type_redirection, char *file_name,
				t_var_redirect *var_redirection)
{
	char	*input_herdoc;

	if (ft_strlen(type_redirection) == 2
		&& !ft_strncmp(type_redirection, "<<", 2))
	{
		free(var_redirection->join_herdoc);
		var_redirection->join_herdoc = NULL;
		while (1)
		{
			input_herdoc = readline(CYAN "heredoc > " DEF);
			if (!input_herdoc)
			{
				if (!var_redirection->join_herdoc)
					var_redirection->join_herdoc = ft_strdup("");
				break ;
			}
			if (!end_herdoc(input_herdoc, file_name, var_redirection))
				break ;
			join_herdoc(var_redirection, input_herdoc);
		}
	}
}

short	redirect_output(char *type_redirection, char *file_name,
							t_var_redirect *var_redirection)
{
	if (ft_strlen(type_redirection) == 1
		&& !ft_strncmp(type_redirection, ">", 1))
	{
		var_redirection->fd_file_output = open(file_name,
				O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (var_redirection->fd_file_output < 0)
		{
			ft_putstr_fd("\033[34mminishell: ", 2);
			ft_putstr_fd("\033[31mambiguous redirect.\033[0m\n", 2);
			return (ft_status(1, 1), 0);
		}
	}
	else if (ft_strlen(type_redirection) == 2
		&& !ft_strncmp(type_redirection, ">>", 2))
	{
		var_redirection->fd_file_output = open(file_name,
				O_CREAT | O_APPEND | O_WRONLY, 0644);
		if (var_redirection->fd_file_output < 0)
		{
			ft_putstr_fd("\033[34mminishell: ", 2);
			ft_putstr_fd("\033[31mambiguous redirect.\033[0m\n", 2);
			return (ft_status(1, 1), 0);
		}
	}
	return (1);
}

short	redirect_input(char *type_redirection, char *file_name,
							t_var_redirect *var_redirection)
{
	if (ft_strlen(type_redirection) == 1
		&& !ft_strncmp(type_redirection, "<", 1))
	{
		var_redirection->join_herdoc = NULL;
		if (!ft_strlen(file_name))
		{
			ft_putstr_fd("\033[34mminishell: ", 2);
			ft_putstr_fd("\033[31mambiguous redirect.\033[0m\n", 2);
			return (ft_status(1, 1), 0);
		}
		var_redirection->fd_file_input = open(file_name, O_RDONLY);
		if (var_redirection->fd_file_input < 0)
		{
			ft_putstr_fd("\033[34mminishell: \033[31m", 2);
			ft_putstr_fd(file_name, 2);
			ft_putstr_fd("\033[0m No such file or directory.\n", 2);
			return (ft_status(1, 1), 0);
		}
	}
	return (1);
}

short	setup_redirections(char **tokens_redirections,
							t_var_redirect *var_redirection)
{
	t_redirections	*redirc;

	var_redirection->fd_file_input = 0;
	var_redirection->fd_file_output = 0;
	var_redirection->join_herdoc = NULL;
	redirc = list_redirections(tokens_redirections, var_redirection->variables);
	free_list(var_redirection->variables, NULL, redirc);
	if (!redirc)
		return (0);
	while (redirc)
	{
		if (!redirect_output(redirc->type_redirection,
				redirc->file_name, var_redirection))
			return (0);
		herdoc(redirc->type_redirection, redirc->file_name,
			var_redirection);
		if (!redirect_input(redirc->type_redirection,
				redirc->file_name, var_redirection))
			return (0);
		redirc = redirc->next;
	}
	return (1);
}

char	*builtin_cmd_redirections(char **tokens, int fd_pipe,
						t_var *variables, t_var_redirect *var_redirection)
{
	if (!tokens)
		return (var_redirection->return_val = 0, NULL);
	if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
		exit_cmd(variables->copy_env, variables, tokens, 1);
	else if (ft_strlen(tokens[0]) == 6
		&& !ft_strncmp(tokens[0], "export", 6))
		return (export_cmd(tokens, 1, variables));
	else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		unset_cmd(tokens, variables);
	else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		return (echo_cmd(tokens, 1));
	else if (ft_strlen(tokens[0]) == 3 && (!ft_strncmp(tokens[0], "pwd", 3)
			|| !ft_strncmp(tokens[0], "PWD", 3)))
		return (pwd_cmd(0));
	else if (ft_strlen(tokens[0]) == 2
		&& !ft_strncmp(tokens[0], "cd", 2) && fd_pipe < 0)
	{
		variables->old_pwd = ft_getenv("PWD", variables);
		cd_cmd(tokens[1], variables);
		return (var_redirection->return_val = 0, NULL);
	}
	return (NULL);
}

void	mange_fd_redirect(t_var_redirect *var_redirection, int *fd)
{
	if (var_redirection->join_herdoc)
		dup2(fd[0], 0);
	else if (var_redirection->fd_file_input > 0)
		dup2(var_redirection->fd_file_input, 0);
	else if (var_redirection->fd_pipe > 0)
		dup2(fd[0], 0);
	dup2(var_redirection->fd_output[1], 1);
	close(fd[0]);
	close(var_redirection->fd_output[0]);
	close(fd[1]);
	close(var_redirection->fd_output[1]);
}

pid_t	create_process_redirection(t_var_redirect *var_redirection,
					t_var *variables, char **tokens, int *fd)
{
	var_redirection->pid = fork();
	if (!var_redirection->pid)
	{
		mange_fd_redirect(var_redirection, fd);
		if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		{
			if (var_redirection->cmd_result)
				printf("%s", var_redirection-> cmd_result);
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
			printf("%s\n", var_redirection->cmd_result);
		else if (ft_strlen(tokens[0]) == 6
			&& !ft_strncmp(tokens[0], "export", 6))
		{
			if (var_redirection->cmd_result)
				printf("%s", var_redirection->cmd_result);
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
			env_cmd(1, variables);
		else
			execve(var_redirection->path_cmd, tokens, variables->env);
		exit(ft_status(0, 0));
	}
	return (var_redirection->pid);
}

char	*close_pipe(pid_t pid, t_var_redirect *var_redirection,
						int *fd, char *pipe_output)
{
	char	*output_cmd;
	int		status;

	if (pid > 0)
	{
		g_signal_flag = pid;
		waitpid(pid, &status, 0);
		if (!WIFSIGNALED(status))
			ft_status(WEXITSTATUS(status), 1);
		free(var_redirection->join_herdoc);
		free(var_redirection->path_cmd);
		free(pipe_output);
		close(fd[1]);
		close(var_redirection->fd_output[1]);
		g_signal_flag = 0;
	}
	output_cmd = read_fd(var_redirection->fd_output[0]);
	close(fd[0]);
	close(var_redirection->fd_output[0]);
	if (var_redirection->fd_file_input > 0)
		close(var_redirection->fd_file_input);
	return (output_cmd);
}

short	handle_pipe(char *cmd_line, char **tokens,
						int *fd, t_var_redirect *var_redirection)
{
	char	**tokens_redirections;

	tokens_redirections = get_tokens_with_redirection(cmd_line);
	if (!tokens_redirections)
		return (0);
	free_array(tokens_redirections, 1, var_redirection->variables);
	if (!setup_redirections(tokens_redirections, var_redirection))
		return (0);
	var_redirection->cmd_result = builtin_cmd_redirections(tokens,
			var_redirection->fd_pipe,
			var_redirection->variables, var_redirection);
	if (!var_redirection->return_val)
		return (0);
	var_redirection->path_cmd = is_there_cmd(tokens,
			var_redirection->variables);
	if (!var_redirection->path_cmd && !is_buitin_cmd(tokens[0]))
		return (0);
	var_redirection->path_cmd = var_redirection->path_cmd;
	pipe(fd);
	pipe(var_redirection->fd_output);
	return (1);
}

short	init_pipe(char *cmd_line, t_var_redirect *var_redirection,
						int fd_pipe, int *fd)
{
	char	*cmd_args;

	var_redirection->return_val = 1;
	var_redirection->fd_pipe = fd_pipe;
	var_redirection->pipe_output = NULL;
	if (fd_pipe > 0)
		var_redirection->pipe_output = read_fd(fd_pipe);
	cmd_args = join_cmd_args(cmd_line);
	var_redirection->tokens = split_command(cmd_args, 1,
			var_redirection->variables);
	free_array(var_redirection->tokens, 1, var_redirection->variables);
	free(cmd_args);
	if (!handle_pipe(cmd_line, var_redirection->tokens, fd, var_redirection))
		return (0);
	if (var_redirection->join_herdoc)
		ft_putstr_fd(var_redirection->join_herdoc, fd[1]);
	return (1);
}

short	final_setup_pipe(t_var_redirect *var_redirection,
								t_var *variables, int *fd)
{
	pid_t			pid;
	char			*output_cmd;

	close(fd[1]);
	pid = create_process_redirection(var_redirection, variables,
			var_redirection->tokens, fd);
	output_cmd = close_pipe(pid, var_redirection, fd,
			var_redirection->pipe_output);
	if (var_redirection->fd_file_output > 0 && output_cmd)
	{
		write(var_redirection->fd_file_output,
			output_cmd, ft_strlen(output_cmd));
		free(output_cmd);
		close(var_redirection->fd_file_output);
		return (0);
	}
	var_redirection->output_cmd = output_cmd;
	return (1);
}

char	*redirections(char *cmd_line, int fd_pipe, t_var *variables, char *s)
{
	t_var_redirect	*var_redirection;
	int				fd[2];

	var_redirection = malloc(sizeof(t_var_redirect));
	if (!var_redirection)
		return (NULL);
	var_redirection->variables = variables;
	if (!init_pipe(cmd_line, var_redirection, fd_pipe, fd))
		return (NULL);
	else if (fd_pipe > 0)
	{
		if (var_redirection->pipe_output)
			write(fd[1], var_redirection->pipe_output,
				ft_strlen(var_redirection->pipe_output));
		else
			write(fd[1], "", 0);
	}
	if (!final_setup_pipe(var_redirection, variables, fd))
		return (NULL);
	if (var_redirection->fd_file_output > 0)
		close(var_redirection->fd_file_output);
	if (!var_redirection->output_cmd)
		return (free(var_redirection), NULL);
	return (s = ft_strdup(var_redirection->output_cmd), free(var_redirection),
		s);
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
	exit(ft_status(0, 0));
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

void	end_pipe(size_t tokens_count, int *pipe_fd, pid_t pid, t_var *variables)
{
	size_t	i;
	int		status;

	i = 0;
	while (i < 2 * (tokens_count - 1))
		close(pipe_fd[i++]);
	1 && (g_signal_flag = pid, waitpid(pid, &status, 0));
	if (wait(NULL) < 0 && !WIFSIGNALED(status))
		ft_status(WEXITSTATUS(status), 1);
	g_signal_flag = 0;
	tcsetattr(0, 0, variables->ctr);
	i = 0;
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
		varpipe->tokens = split_command(split_pipe[i], 1, variables);
		if (is_exist_redirect_pipe(split_pipe[i], 'r'))
			varpipe->redirections_output = redirections(split_pipe[i],
					in_fd, variables, NULL);
		pid = child_pipe(i, tokens_count, pipe_fd, varpipe);
		free_array(varpipe->tokens, 0, variables);
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
	end_pipe(tokens_count, pipe_fd, pid, variables);
	free(pipe_fd);
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

void	exec_commands(char **tokens, char **copy_env,
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
		if (!WIFSIGNALED(status))
			ft_status(WEXITSTATUS(status), 1);
		g_signal_flag = 0;
	}
	return (tcsetattr(0, 0, ctr), free(path_cmd));
}

short	redirections_pipe(char **tokens, char *cmd_line, t_var *variables)
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
		redirection_output = redirections(cmd_line, -1, variables, NULL);
		if (redirection_output)
			printf("%s", redirection_output);
		return (1);
	}
	return (0);
}

char	*ft_readline(char *pwd, size_t i, char *cmd_line)
{
	char	**path;
	char	*last_dir;
	char	*last_dir_tmp;

	if (ft_strlen(pwd) == 1 && !ft_strncmp(pwd, "/", 1))
		last_dir = ft_strdup("/");
	else
	{
		path = ft_split(pwd, '/');
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
	if (ft_status(0, 0))
		last_dir_tmp = ft_strjoin("\033[31m➥\033[0m ", last_dir);
	free(last_dir);
	last_dir = ft_strjoin(last_dir_tmp, " ");
	free(last_dir_tmp);
	return (cmd_line = readline(last_dir), free(pwd), free(last_dir), cmd_line);
}

void	minishell_loop(t_var *variables, char **copy_env, struct termios *ctr)
{
	t_list	*new_leak;
	char	*command;
	char	**tokens;

	while (1)
	{
		variables->environment = all_env(NULL, NULL, 0, variables);
		free_list(variables, variables->environment, NULL);
		command = ft_readline(pwd_cmd(0), 0, NULL);
		new_leak = ft_lstnew(command);
		ft_lstadd_back(&variables->leaks, new_leak);
		if (!command)
			exit_cmd(copy_env, variables, NULL, 1);
		if (!command[0])
			continue ;
		add_history(command);
		tokens = split_command(command, 1, variables);
		free_array(tokens, 1, variables);
		if (redirections_pipe(tokens, command, variables)
			|| builtin_commands(tokens, copy_env, variables))
			continue ;
		exec_commands(tokens, copy_env, ctr, is_there_cmd(tokens, variables));
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

	if (argc != 1 && argv)
		return (printf(RED "Please do not enter any arguments.\n" DEF), 1);
	status = 0;
	variables = malloc(sizeof(t_var));
	1 && (variables->cd_flag = 0, variables->env = env);
	1 && (variables->environment = NULL, variables->export_list = NULL);
	1 && (variables->leaks = NULL, variables->cd_flag = 0);
	variables->ctr = &ctr;
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
