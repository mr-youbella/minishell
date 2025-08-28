/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:14:53 by youbella          #+#    #+#             */
/*   Updated: 2025/08/28 00:40:52 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static short	end_herdoc(char *input_herdoc, char *file_name,
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

static void	join_herdoc(t_var_redirect *var_redirection, char *input_herdoc)
{
	char	*input_herdoc_dollar;
	char	*tmp;

	input_herdoc_dollar = ft_dollar(input_herdoc,
			var_redirection->variables, 0);
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
	int		in_fd;

	if (ft_strlen(type_redirection) == 2
		&& !ft_strncmp(type_redirection, "<<", 2))
	{
		ft_status(0, 1);
		1 && (g_signal_flag = -3, in_fd = dup(0));
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
		1 && (dup2(in_fd, 0), close(in_fd));
	}
}

static short	check_is_ambiguous(t_var_redirect *var_redirection)
{
	if (var_redirection->fd_file_output < 0)
	{
		ft_putstr_fd("\033[34mminishell: ", 2);
		ft_putstr_fd("\033[31mambiguous redirect.\033[0m\n", 2);
		return (ft_status(1, 1), 0);
	}
	return (1);
}

short	redirect_output(char *type_redirection, char *file_name,
							t_var_redirect *var_redirection)
{
	if (ft_strlen(type_redirection) == 1
		&& !ft_strncmp(type_redirection, ">", 1))
	{
		if (var_redirection->fd_file_output > 0)
			close(var_redirection->fd_file_output);
		var_redirection->fd_file_output = open(file_name,
				O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (!check_is_ambiguous(var_redirection))
			return (0);
	}
	else if (ft_strlen(type_redirection) == 2
		&& !ft_strncmp(type_redirection, ">>", 2))
	{
		if (var_redirection->fd_file_output > 0)
			close(var_redirection->fd_file_output);
		var_redirection->fd_file_output = open(file_name,
				O_CREAT | O_APPEND | O_WRONLY, 0644);
		if (!check_is_ambiguous(var_redirection))
			return (0);
	}
	return (1);
}
