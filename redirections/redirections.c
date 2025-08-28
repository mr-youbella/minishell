/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:11:00 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 13:04:04 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*close_pipe(pid_t pid, t_var_redirect *var_redirection,
						int *fd, char *pipe_output)
{
	char	*output_cmd;
	int		status;

	1 && (status = 0, output_cmd = NULL);
	if (pid < 0)
		perror("fork");
	if (pid > 0)
	{
		g_signal_flag = pid;
		close(fd[1]);
		close(var_redirection->fd_output[1]);
		close(fd[0]);
		if (var_redirection->fd_file_input > 0)
			close(var_redirection->fd_file_input);
		output_cmd = read_fd(var_redirection->fd_output[0]);
		close(var_redirection->fd_output[0]);
		waitpid(pid, &status, 0);
		if (!WIFSIGNALED(status))
			ft_status(WEXITSTATUS(status), 1);
		free(var_redirection->join_herdoc);
		free(var_redirection->path_cmd);
		free(pipe_output);
		g_signal_flag = 0;
	}
	return (output_cmd);
}

static short	handle_pipe(char *cmd_line, char **tokens,
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
		return (vr_free_close(var_redirection), 0);
	var_redirection->path_cmd = is_there_cmd(tokens,
			var_redirection->variables);
	if (!var_redirection->path_cmd && !is_buitin_cmd(tokens[0]))
		return (vr_free_close(var_redirection), ft_status(0, 1), 0);
	if (pipe(fd) < 0 || pipe(var_redirection->fd_output) < 0)
	{
		perror("pipe");
		free(var_redirection->path_cmd);
		return (vr_free_close(var_redirection), 0);
	}
	return (1);
}

static short	init_pipe(char *cmd_line, t_var_redirect *var_redirection,
						int fd_pipe, int *fd)
{
	char	*cmd_args;

	var_redirection->return_val = 1;
	var_redirection->fd_pipe = fd_pipe;
	var_redirection->pipe_output = NULL;
	if (fd_pipe > 0)
		var_redirection->pipe_output = read_fd(fd_pipe);
	cmd_args = join_cmd_args(cmd_line);
	if (!cmd_args)
		ft_status(0, 1);
	var_redirection->tokens = split_command(cmd_args, 1,
			var_redirection->variables);
	free_array(var_redirection->tokens, 1, var_redirection->variables);
	free(cmd_args);
	if (!handle_pipe(cmd_line, var_redirection->tokens, fd, var_redirection))
		return (free(var_redirection->pipe_output),
			free(var_redirection->join_herdoc), 0);
	if (var_redirection->join_herdoc)
		ft_putstr_fd(var_redirection->join_herdoc, fd[1]);
	return (1);
}

static short	final_setup_pipe(t_var_redirect *var_redirection,
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
	if (var_redirection->fd_file_output > 0)
		close(var_redirection->fd_file_output);
	return (1);
}

char	*redirections(char *cmd_line, int fd_pipe, t_var *variables, char *s)
{
	t_var_redirect	*var_redirection;
	int				fd[2];

	var_redirection = malloc(sizeof(t_var_redirect));
	if (!var_redirection)
		return (ft_status(1, 1), NULL);
	ft_memset(var_redirection, 0, sizeof(t_var_redirect));
	var_redirection->variables = variables;
	if (!init_pipe(cmd_line, var_redirection, fd_pipe, fd))
		return (free(var_redirection), NULL);
	else if (fd_pipe > 0)
	{
		if (var_redirection->pipe_output)
			write(fd[1], var_redirection->pipe_output,
				ft_strlen(var_redirection->pipe_output));
		else
			write(fd[1], "", 0);
	}
	if (!final_setup_pipe(var_redirection, variables, fd))
		return (free(var_redirection), NULL);
	if (!var_redirection->output_cmd)
		return (free(var_redirection), NULL);
	return (s = ft_strdup(var_redirection->output_cmd),
		free(var_redirection->output_cmd), free(var_redirection), s);
}
