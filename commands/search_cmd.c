/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 03:45:42 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 03:53:47 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*search_in_path(char *cmd, char **split_env_path)
{
	size_t	i;
	char	*join_cmd_to_path;
	char	*tmp;

	i = 0;
	while (split_env_path && split_env_path[i])
	{
		join_cmd_to_path = ft_strjoin(split_env_path[i], "/");
		tmp = join_cmd_to_path;
		join_cmd_to_path = ft_strjoin(join_cmd_to_path, cmd);
		free(tmp);
		if (!access(join_cmd_to_path, X_OK))
		{
			i = 0;
			while (split_env_path[i])
				free(split_env_path[i++]);
			free(split_env_path);
			return (join_cmd_to_path);
		}
		free(join_cmd_to_path);
		i++;
	}
	return (NULL);
}

char	*search_cmd(char *cmd, t_var *variables)
{
	size_t	i;
	char	*env_path;
	char	**split_env_path;
	char	*join_cmd_to_path;

	i = 0;
	if (!cmd[0])
		return (NULL);
	if (cmd[0] == '/')
		return (ft_strdup(cmd));
	env_path = ft_getenv("PATH", variables);
	split_env_path = ft_split(env_path, ':');
	join_cmd_to_path = search_in_path(cmd, split_env_path);
	if (join_cmd_to_path)
		return (join_cmd_to_path);
	i = 0;
	while (split_env_path && split_env_path[i])
		free(split_env_path[i++]);
	free(split_env_path);
	return (NULL);
}

static short	is_directory(char **tokens)
{
	int	fd;

	if (is_buitin_cmd(tokens[0]))
		return (-1);
	if ((tokens[0][0] == '.' && tokens[0][1] == '/')
		|| (tokens[0][0] == '.' && tokens[0][1] == '.' & tokens[0][2] == '/'))
	{
		fd = open(tokens[0], O_RDONLY);
		if (fd < 0)
		{
			ft_putstr_fd("\033[31mminishell: \033[34m", 2);
			ft_putstr_fd(tokens[0], 2);
			ft_putstr_fd("\033[0m: No such file or directory.\n", 2);
			return (ft_status(127, 1), -1);
		}
		return (1);
	}
	return (0);
}

char	*is_there_cmd(char **tokens, t_var *variables)
{
	char		*path_cmd;
	short		check_return;
	struct stat	file;

	if (stat(tokens[0], &file) == 0 && S_ISDIR(file.st_mode))
	{
		ft_putstr_fd("\033[34mminishell: \033[31m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m is a directory.\n", 2);
		return (ft_status(126, 1), NULL);
	}
	check_return = is_directory(tokens);
	if (check_return == 1)
		return (ft_strdup(tokens[0]));
	if (check_return == -1)
		return (NULL);
	path_cmd = search_cmd(tokens[0], variables);
	if (!path_cmd)
	{
		ft_putstr_fd("\033[34mminishell: \033[31m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m command not found.\n", 2);
		return (ft_status(127, 1), NULL);
	}
	return (path_cmd);
}
