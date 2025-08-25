/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 12:23:14 by youbella          #+#    #+#             */
/*   Updated: 2025/08/25 00:18:12 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_cmd(char *cmd, t_var *variables)
{
	int		i;
	char	*env_path;
	char	**split_env_path;
	char	*join_cmd_to_path;
	char	*tmp;

	i = 0;
	if (!cmd[0])
		return (NULL);
	if (cmd[0] == '/')
		return (ft_strdup(cmd));
	env_path = ft_getenv("PATH", variables);
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

char	*is_there_cmd(char **tokens, t_var *variables)
{
	char			*path_cmd;
	int				fd;
	struct stat		file;


	if (stat(tokens[0], &file) == 0 && S_ISDIR(file.st_mode))
	{
		ft_putstr_fd("\033[34mminishell: \033[31m", 2);
		ft_putstr_fd(tokens[0], 2);
		ft_putstr_fd("\033[0m is a directory.\n", 2);
		ft_status(126, 1);
		return (NULL);
	}
	if (is_buitin_cmd(tokens[0]))
		return (NULL);
	if ((tokens[0][0] == '.' && tokens[0][1] == '/')
		|| (tokens[0][0] == '.' && tokens[0][1] == '.' & tokens[0][2] == '/'))
	{
		fd = open(tokens[0], O_RDONLY);
		if (fd < 0)
		{
			write(2, "\033[31mminishell: \033[34m", ft_strlen("\033[31mminishell: \033[34m"));
			write(2, tokens[0], ft_strlen(tokens[0]));
			ft_putstr_fd("\033[0m: No such file or directory.\n", 2);
			ft_status(127, 1);
			return (NULL);
		}
		return (ft_strdup(tokens[0]));
	}
	path_cmd = search_cmd(tokens[0], variables);
	if (!path_cmd)
	{
		write(2, "\033[34mminishell: \033[31m", ft_strlen("\033[34mminishell: \033[34m"));
		write(2, tokens[0], ft_strlen(tokens[0]));
		write(2, "\033[0m command not found.\n", ft_strlen("\033[0m command not found.\n"));
		ft_status(127, 1);
		return (NULL);
	}
	return (path_cmd);
}

short	check_option_echo(char *token)
{
	size_t	i;

	i = 1;
	if (token[0] != '-')
		return (0);
	while (token[i])
	{
		if (token[i] != 'n')
			return (0);
		i++;
	}
	return (1);
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
	while (tokens[j] && check_option_echo(tokens[j]))
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
		if (tokens[j + 1] && ft_strlen(tokens[j]))
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

t_list	*env_cmd(short is_print, t_var *variables)
{
	t_list	*environment;
	t_list	*all_environment;
	t_list	*copy_environment;
	t_list	*new_leak;
	size_t	i;
	short	is_with_value;

	all_environment = all_env(NULL, NULL, 0, 0, variables);
	environment = all_environment;
	while (all_environment)
	{
		new_leak = ft_lstnew(all_environment);
		ft_lstadd_back(&variables->leaks, new_leak);
		all_environment = all_environment->next;
	}
	copy_environment = environment;
	while (is_print && copy_environment)
	{
		is_with_value = 0;
		i = 0;
		while (((char *)copy_environment->content)[i])
		{
			if (((char *)copy_environment->content)[i] == '=')
			{
				is_with_value = 1;
				break ;
			}
			i++;
		}
		if (is_with_value)
			printf("%s\n", (char *)copy_environment->content);
		copy_environment = copy_environment->next;
	}
	return (environment);
}

void	cd_cmd(char *tokens, t_var *variables)
{
	char	*home;
	
	variables->cd_flag = 1;
	home = ft_getenv("HOME", variables);
	if (!tokens || (ft_strlen(tokens) == 1 && !ft_strncmp(tokens, "~", 1)))
	{
		if (!home)
		{
			write(2, "\033[34mminishell: \033[0mcd: \033[31mHOME not set.\033[0m\n", ft_strlen("\033[34mminishell: \033[0mcd: \033[31mHOME not set.\033[0m\n"));
			ft_status(1, 1);
			variables->cd_flag = 0;
		}
		else
			chdir(home);
	}
	else if (chdir(tokens) == -1)
	{
		write(2, "\033[34mminishell:\033[0m cd: no such file or directory: \033[31m", ft_strlen("\033[34mminishell:\033[0m cd: no such file or directory: \033[31m"));
		write(2, tokens, ft_strlen(tokens));
		write(2, "\033[0m\n", ft_strlen("\033[0m\n"));
		ft_status(1, 1);
		variables->cd_flag = 0;
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

static char	*print_sorted_env(t_var *variables, short is_return)
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
	tmp = variables->environment;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	env_array = malloc(sizeof(char *) * count);
	tmp = variables->environment;
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
		ft_lstadd_back(&variables->leaks, new_leak);
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
		ft_lstadd_back(&variables->leaks, new_leak);
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

char	*export_cmd(char **tokens, short is_return, t_var *variables)
{
	size_t	i;
	size_t	j;
	short	is_there_equal;
	t_list	*all_environment;
	t_list	*new_leak;
	char	*export;
	char	*tmp;
	char	*tmp2;

	i = 1;
	if (!tokens[i])
	{
		variables->environment = all_env(NULL, NULL, 1, 1, variables);
		all_environment = variables->environment;
		while (all_environment)
		{
			new_leak = ft_lstnew(all_environment);
			ft_lstadd_back(&variables->leaks, new_leak);
			all_environment = all_environment->next;
		}
		export = print_sorted_env(variables, is_return);
		variables->environment = all_env(NULL, NULL, 1, 2, variables);
		all_environment = variables->environment;
		while (all_environment)
		{
			new_leak = ft_lstnew(all_environment);
			ft_lstadd_back(&variables->leaks, new_leak);
			all_environment = all_environment->next;
		}
		tmp = export;
		tmp2 = print_sorted_env(variables, is_return);
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
			while (tokens[i][j] && tokens[i][j] != '=')
				j++;
			tmp = ft_substr(tokens[i], 0, j);
			j = 0;
			if (is_exist_in_env(tmp, variables->env, -1))
			{
				all_env(tokens[i], NULL, 0, 0, variables);
				i++;
				free(tmp);
				continue;
			}
			free(tmp);
			if (is_exist_var(tokens[i], variables, variables->export_list))
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
					all_env(tokens[i], NULL, 0, 0, variables);
			}
			else
				ft_lstadd_back(&variables->export_list, ft_lstnew(tokens[i]));
		}
		i++;
	}
	return (NULL);
}

void	unset_cmd(char **tokens, t_var *variables)
{
	int	j;

	j = 0;
	while (tokens[j])
	{
		if (check_unset_arg(tokens[j]))
		{
			if (is_exist_var(tokens[j], variables, variables->export_list))
				all_env(NULL, tokens[j], 0, 0, variables);
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

void	exit_cmd(char **copy_env, t_var *variables, char **tokens, short is_print)
{
	int	status;

	status = ft_status(0, 0);
	free(copy_env);
	free_leaks(variables);
	if (is_print)
		printf(RED "exit\n" DEF);
	free(variables);
	if (!tokens || !tokens[1])
		exit(status);
	if (tokens[1] && !tokens[2])
		exit(ft_atoi(tokens[1]));
	else
	{
		if (is_print)
			printf(RED "exit\n" DEF);
		ft_putstr_fd("\033[34mminishell: exit: \033[31mtoo many arguments.\033[0m", 2);
	}
}
