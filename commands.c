/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 12:23:14 by youbella          #+#    #+#             */
/*   Updated: 2025/08/09 23:13:51 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *search_cmd(char *cmd, t_list *environment)
{
	int i;
	char *env_path;
	char **split_env_path;
	char *join_cmd_to_path;

	i = 0;
	if ((cmd[0] == '.' && cmd[1] == '/') || (cmd[0] == '.' && cmd[1] == '.' & cmd[2] == '/'))
		return (cmd);
	else if (cmd[0] == '/')
		return (cmd);
	if (!access(cmd, X_OK))
		return (cmd);
	env_path = ft_getenv("PATH", environment);
	if (!env_path)
		return (NULL);
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

char *is_there_cmd(char **tokens, t_list *environment, int *status)
{
	char *path_cmd;

	path_cmd = search_cmd(tokens[0], environment);
	if (!path_cmd)
	{
		printf(RED "minishell: %s%s%s command not found.\n", BLUE, tokens[0], DEF);
		*status = 32512;
		return (NULL);
	}
	return (path_cmd);
}

char *echo_cmd(char **tokens, int *status, short is_return)
{
	int j;
	short is_op_echo;
	char *str;

	j = 1;
	is_op_echo = 0;
	str = NULL;
	while (tokens[j] && !ft_strncmp(tokens[j], "-n", 2) && ft_strlen(tokens[j]) == 2)
	{
		is_op_echo = 1;
		j++;
	}
	while (tokens[j])
	{
		if (is_return)
		{
			str = ft_strjoin(str, tokens[j]);
			str = ft_strjoin(str, " ");
		}
		else
		{
			printf("%s", tokens[j]);
			if (tokens[j + 1])
				printf(" ");
		}
		j++;
	}
	if (!is_op_echo)
	{
		if (is_return)
			str = ft_strjoin(str, "\n");
		else
			printf("\n");
	}
	*status = 0;
	return (str);
}

t_list *env_cmd(char **env, t_list *export_list, short is_return)
{
	t_list *environment;
	t_list *copy_environment;

	environment = all_env(NULL, NULL, env, export_list, 1);
	copy_environment = environment;
	while (!is_return && copy_environment)
	{
		printf("%s\n", (char *)copy_environment->content);
		copy_environment = copy_environment->next;
	}
	return (environment);
}

void cd_cmd(char *tokens)
{
	if (!tokens)
		chdir(getenv("HOME"));
	else if (chdir(tokens) == -1)
		printf(BLUE "minishell%s: cd: no such file or directory: %s%s%s\n", DEF, RED, tokens, DEF);
}

void export_cmd(char **env, char **tokens, t_list **export_list)
{
	int i;

	while (tokens[i])
	{
		if (check_export_arg(tokens[i]))
		{
			if (is_exist_var(tokens[i], env, *export_list))
				all_env(tokens[i], NULL, env, *export_list, 0);
			else
				ft_lstadd_back(export_list, ft_lstnew(ft_strdup(tokens[i])));
		}
		i++;
	}
}

void unset_cmd(char **tokens, char **env, t_list **export_list)
{
	int j;

	while (tokens[j])
	{
		if (check_unset_arg(tokens[j]))
		{
			if (is_exist_var(tokens[j], env, *export_list))
				all_env(NULL, tokens[j], env, *export_list, 0);
		}
		j++;
	}
}
