/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 04:16:14 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 17:23:08 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	is_empty_token(char *token)
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
		free(path);
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
	i = 0;
	while (i < tokens_count)
	{
		wait(NULL);
		i++;
	}
	if (wait(NULL) < 0 && !WIFSIGNALED(status))
		ft_status(WEXITSTATUS(status), 1);
	tcsetattr(0, 0, variables->ctr);
	g_signal_flag = 0;
	free(pipe_fd);
}
