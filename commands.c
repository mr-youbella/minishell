/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 12:23:14 by youbella          #+#    #+#             */
/*   Updated: 2025/08/16 16:51:12 by youbella         ###   ########.fr       */
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
	if (!cmd[0])
		return (NULL);
	if ((cmd[0] == '.' && cmd[1] == '/') || (cmd[0] == '.' && cmd[1] == '.' & cmd[2] == '/'))
		return (cmd);
	if (cmd[0] == '/')
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

	if ((ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4)) || (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2)) || (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3)) || (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6)) || (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5)) || (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3)) || (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4)))
		return (ft_strdup(tokens[0]));
	path_cmd = search_cmd(tokens[0], environment);
	if (!path_cmd)
	{
		printf(RED "minishell: %s%s%s command not found.\n", BLUE, tokens[0], DEF);
		*status = 32512;
		return (NULL);
	}
	return (path_cmd);
}

short echo_cmd(char **tokens)
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
		printf("%s", tokens[j]);
		if (tokens[j + 1])
			printf(" ");
		j++;
	}
	if (!is_op_echo)
		printf("\n");
	return (0);
}

t_list *env_cmd(char **env, t_list *export_list, short is_print)
{
	t_list *environment;
	t_list *copy_environment;

	environment = all_env(NULL, NULL, env, export_list, 0, 0, NULL, NULL);
	copy_environment = environment;
	while (is_print && copy_environment)
	{
		printf("%s\n", (char *)copy_environment->content);
		copy_environment = copy_environment->next;
	}
	return (environment);
}

void cd_cmd(char *tokens, short *cd_flag)
{
	*cd_flag = 1;

	if (!tokens)
		chdir(getenv("HOME"));
	else if (chdir(tokens) == -1)
	{
		printf(BLUE "minishell%s: cd: no such file or directory: %s%s%s\n", DEF, RED, tokens, DEF);
		*cd_flag = 0;
	}
}

void sort_env(char **env_arr, int count)
{
	int i, j;
	char *temp;
	size_t len;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strlen(env_arr[j]) > ft_strlen(env_arr[j + 1]))
				len = ft_strlen(env_arr[j]);
			else
				len = ft_strlen(env_arr[j + 1]);
			if (ft_strncmp(env_arr[j], env_arr[j + 1], len) > 0)
			{
				temp = env_arr[j];
				env_arr[j] = env_arr[j + 1];
				env_arr[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

void print_sorted_env(t_list *environment)
{
	int count = 0;
	int i = 0;
	t_list *tmp = environment;
	size_t len_var;
	char *var_name;
	char *var_value;
	char **env_array;

	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	env_array = malloc(sizeof(char *) * count);
	tmp = environment;
	while (i < count)
	{
		env_array[i] = (char *)tmp->content;
		tmp = tmp->next;
		i++;
	}
	sort_env(env_array, count);
	i = 0;
	while (i < count)
	{
		len_var = 0;
		while (env_array[i][len_var] && env_array[i][len_var] != '=')
			len_var++;
		if (env_array[i][len_var] == '=')
			len_var++;
		var_name = ft_substr(env_array[i], 0, len_var);
		printf("declare -x %s%s%s", GREEN, var_name, DEF);
		var_value = ft_substr(env_array[i], len_var, ft_strlen(env_array[i]) - len_var);
		if ((!var_value[0] && env_array[i][len_var - 1] != '=') || (!var_value))
			printf("%s%s%s\n", BLUE, var_value, DEF);
		else
			printf("%s\"%s\"%s\n", BLUE, var_value, DEF);
		i++;
	}
	free(env_array);
}

void export_cmd(char **env, char **tokens, t_list **export_list)
{
	size_t i;
	size_t j;
	size_t len_var;
	short is_there_equal;
	char *var_name;
	char *var_value;
	t_list *environment;

	i = 1;
	if (!tokens[i])
	{
		environment = all_env(NULL, NULL, env, *export_list, 1, 1, NULL, NULL);
		print_sorted_env(environment);
		environment = all_env(NULL, NULL, env, *export_list, 1, 2, NULL, NULL);
		print_sorted_env(environment);
		return;
	}
	while (tokens[i])
	{
		j = 0;
		is_there_equal = 0;
		if (check_export_arg(tokens[i]))
		{
			if (is_exist_var(tokens[i], env, *export_list))
			{
				while (tokens[i][j])
				{
					if (tokens[i][j] == '=')
					{
						is_there_equal = 1;
						break;
					}
					j++;
				}
				if (is_there_equal)
					all_env(tokens[i], NULL, env, *export_list, 0, 0, NULL, NULL);
			}
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
				all_env(NULL, tokens[j], env, *export_list, 0, 0, NULL, NULL);
		}
		j++;
	}
}

char *pwd_cmd(short is_print)
{
	char *pwd;
	pwd = getcwd(NULL, 0);
	if (!pwd)
		pwd = ft_strdup("minishell/unknown");
	if (is_print)
		printf("%s\n", pwd);
	return (pwd);
}

void exit_cmd(int status)
{
	printf(RED "exit\n" DEF);
	exit(status);
}
