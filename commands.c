/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 12:23:14 by youbella          #+#    #+#             */
/*   Updated: 2025/08/20 09:39:30 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_cmd(char *cmd, t_list *environment, t_list **leaks)
{
	int		i;
	char	*env_path;
	char	**split_env_path;
	char	*join_cmd_to_path;
	char	*tmp;

	i = 0;
	if (!cmd[0])
		return (NULL);
	if ((cmd[0] == '.' && cmd[1] == '/')
		|| (cmd[0] == '.' && cmd[1] == '.' & cmd[2] == '/'))
		return (ft_strdup(cmd));
	if (cmd[0] == '/')
		return (ft_strdup(cmd));
	env_path = ft_getenv("PATH", environment, leaks);
	split_env_path = ft_split(env_path, ':');
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
	i = 0;
	while (split_env_path && split_env_path[i])
		free(split_env_path[i++]);
	free(split_env_path);
	return (NULL);
}

char	*is_there_cmd(char **tokens, t_list *environment, t_list **leaks)
{
	char	*path_cmd;
	char	*join_error;
	char	*tmp;

	join_error = NULL;
	if ((ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		|| (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2))
		|| (ft_strlen(tokens[0]) == 3 && (!ft_strncmp(tokens[0], "pwd", 3) || !ft_strncmp(tokens[0], "PWD", 3)))
		|| (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6))
		|| (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		|| (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
		|| (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4)))
		return (ft_strdup(tokens[0]));
	path_cmd = search_cmd(tokens[0], environment, leaks);
	if (!path_cmd)
	{
		join_error = ft_strjoin(join_error, RED);
		tmp = join_error;
		join_error = ft_strjoin(join_error, "minishell: ");
		free(tmp);
		tmp = join_error;
		join_error = ft_strjoin(join_error, BLUE);
		free(tmp);
		tmp = join_error;
		join_error = ft_strjoin(join_error, tokens[0]);
		free(tmp);
		tmp = join_error;
		join_error = ft_strjoin(join_error, " command not found.\n");
		free(tmp);
		tmp = join_error;
		join_error = ft_strjoin(join_error, DEF);
		write(2, join_error, ft_strlen(join_error));
		free(tmp);
		free(join_error);
		ft_status(32512, 1);
		return (NULL);
	}
	return (path_cmd);
}

char	*echo_cmd(char **tokens, short is_return)
{
	int		j;
	short	is_op_echo;
	char	*echo;
	char	*tmp;

	j = 1;
	is_op_echo = 0;
	echo = NULL;
	while (tokens[j] && !ft_strncmp(tokens[j], "-n", 2)
		&& ft_strlen(tokens[j]) == 2)
	{
		is_op_echo = 1;
		j++;
	}
	while (tokens[j])
	{
		if (is_return)
		{
			tmp = echo;
			echo = ft_strjoin(echo, tokens[j]);
			free(tmp);
		}
		else
			printf("%s", tokens[j]);
		if (tokens[j + 1])
		{
			if (is_return)
			{
				tmp = echo;
				echo = ft_strjoin(echo, " ");
				free(tmp);
			}
			else
				printf(" ");
		}
		j++;
	}
	if (!is_op_echo)
	{
		if (is_return)
		{
			tmp = echo;
			echo = ft_strjoin(echo, "\n");
			free(tmp);
		}
		else
			printf("\n");
	}
	ft_status(0, 1);
	return (echo);
}

t_list	*env_cmd(char **env, t_list *export_list, short is_print, t_list **leaks)
{
	t_list	*environment;
	t_list	*all_environment;
	t_list	*copy_environment;
	t_list	*new_leak;

	all_environment = all_env(NULL, NULL, env, export_list, 0, 0, NULL, NULL, leaks);
	environment = all_environment;
	while (all_environment)
	{
		new_leak = ft_lstnew(all_environment);
		ft_lstadd_back(leaks, new_leak);
		all_environment = all_environment->next;
	}
	copy_environment = environment;
	while (is_print && copy_environment)
	{
		printf("%s\n", (char *)copy_environment->content);
		copy_environment = copy_environment->next;
	}
	return (environment);
}

void	cd_cmd(char *tokens, short *cd_flag)
{
	*cd_flag = 1;
	if (!tokens)
		chdir(getenv("HOME"));
	else if (chdir(tokens) == -1)
	{
		printf(BLUE "minishell%s: cd: no such file or directory: %s%s%s\n",
			DEF, RED, tokens, DEF);
		ft_status(256, 1);
		*cd_flag = 0;
	}
}

void	sort_env(char **env_arr, int count)
{
	int		i;
	int		j;
	char	*temp;
	size_t	len;

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

static char	*print_sorted_env(t_list *environment, t_list **leaks, short is_return)
{
	int		count;
	int		i;
	t_list	*tmp;
	size_t	len_var;
	char	*var_name;
	char	*var_value;
	char	**env_array;
	t_list	*new_leak;
	char	*export;
	char	*tmp_export;

	count = 0;
	i = 0;
	export = NULL;
	tmp = environment;
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
		new_leak = ft_lstnew(var_name);
		ft_lstadd_back(leaks, new_leak);
		if (is_return)
		{
			tmp_export = export;
			export = ft_strjoin(export, "declare -x ");
			free(tmp_export);
			tmp_export = export;
			export = ft_strjoin(export, var_name);
			free(tmp_export);
		}
		else
			printf("declare -x %s%s%s", GREEN, var_name, DEF);
		var_value = ft_substr(env_array[i], len_var, ft_strlen(env_array[i]) - len_var);
		new_leak = ft_lstnew(var_value);
		ft_lstadd_back(leaks, new_leak);
		if ((!var_value[0] && env_array[i][len_var - 1] != '=') || (!var_value))
		{
			if (is_return)
			{
				tmp_export = export;
				export = ft_strjoin(export, var_value);
				free(tmp_export);
				tmp_export = export;
				export = ft_strjoin(export, "\n");
				free(tmp_export);
			}
			else
				printf("%s%s%s\n", BLUE, var_value, DEF);
		}
		else
		{
			if (is_return)
			{
				tmp_export = export;
				export = ft_strjoin(export, "\"");
				free(tmp_export);
				tmp_export = export;
				export = ft_strjoin(export, var_value);
				free(tmp_export);
				tmp_export = export;
				export = ft_strjoin(export, "\"");
				free(tmp_export);
				tmp_export = export;
				export = ft_strjoin(export, "\n");
				free(tmp_export);
			}
			else
				printf("%s\"%s\"%s\n", BLUE, var_value, DEF);
		}
		i++;
	}
	free(env_array);
	return (export);
}

char	*export_cmd(char **env, char **tokens, t_list **export_list, t_list **leaks, short is_return)
{
	size_t	i;
	size_t	j;
	short	is_there_equal;
	t_list	*environment;
	t_list	*all_environment;
	t_list	*new_leak;
	char	*export;
	char	*tmp;
	char	*tmp2;

	i = 1;
	if (!tokens[i])
	{
		all_environment = all_env(NULL, NULL, env, *export_list, 1, 1, NULL, NULL, leaks);
		environment = all_environment;
		while (all_environment)
		{
			new_leak = ft_lstnew(all_environment);
			ft_lstadd_back(leaks, new_leak);
			all_environment = all_environment->next;
		}
		export = print_sorted_env(environment, leaks, is_return);
		all_environment = all_env(NULL, NULL, env, *export_list, 1, 2, NULL, NULL, leaks);
		environment = all_environment;
		while (all_environment)
		{
			new_leak = ft_lstnew(all_environment);
			ft_lstadd_back(leaks, new_leak);
			all_environment = all_environment->next;
		}
		tmp = export;
		tmp2 = print_sorted_env(environment, leaks, is_return);
		export = ft_strjoin(export, tmp2);
		free(tmp);
		free(tmp2);
		return (export);
	}
	while (tokens[i])
	{
		j = 0;
		is_there_equal = 0;
		if (check_export_arg(tokens[i]))
		{
			if (is_exist_var(tokens[i], env, *export_list, leaks))
			{
				while (tokens[i][j])
				{
					if (tokens[i][j] == '=')
					{
						is_there_equal = 1;
						break ;
					}
					j++;
				}
				if (is_there_equal)
					all_env(tokens[i], NULL, env, *export_list, 0, 0, NULL, NULL, leaks);
			}
			else
				ft_lstadd_back(export_list, ft_lstnew(tokens[i]));
		}
		i++;
	}
	return (NULL);
}

void	unset_cmd(char **tokens, char **env, t_list **export_list, t_list **leaks)
{
	int	j;

	j = 0;
	while (tokens[j])
	{
		if (check_unset_arg(tokens[j]))
		{
			if (is_exist_var(tokens[j], env, *export_list, leaks))
				all_env(NULL, tokens[j], env, *export_list, 0, 0, NULL, NULL, leaks);
		}
		j++;
	}
}

char	*pwd_cmd(short is_print)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		pwd = ft_strdup("minishell/unknown");
	if (is_print)
		printf("%s\n", pwd);
	return (pwd);
}

void	exit_cmd(void)
{
	printf(RED "exit\n" DEF);
	exit(ft_status(0, 0));
}
