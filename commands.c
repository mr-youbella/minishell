/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 12:23:14 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 00:40:39 by youbella         ###   ########.fr       */
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

	all_environment = all_env(NULL, NULL, 0, variables);
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

char	*print_var(short is_return, char *export, char *var_name)
{
	char	*tmp_export;

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
	return (export);
}

void	print_val_2(short is_return, t_var *variables,
					char	*tmp_export, char *var_value)
{
	if (is_return)
	{
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, "\"");
		free(tmp_export);
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, var_value);
		free(tmp_export);
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, "\"");
		free(tmp_export);
		tmp_export = variables->export;
		variables->export = ft_strjoin(variables->export, "\n");
		free(tmp_export);
	}
	else
		printf("%s\"%s\"%s\n", BLUE, var_value, DEF);
}

void	print_val(short is_return, char	**env_array,
					size_t len_var, t_var *var)
{
	char	*tmp_export;
	char	*var_value;
	t_list	*new_leak;

	tmp_export = NULL;
	var_value = ft_substr(env_array[var->i],
			len_var, ft_strlen(env_array[var->i]) - len_var);
	new_leak = ft_lstnew(var_value);
	ft_lstadd_back(&var->leaks, new_leak);
	if ((!var_value[0] && env_array[var->i][len_var - 1] != '=') || !var_value)
	{
		if (is_return)
		{
			tmp_export = var->export;
			var->export = ft_strjoin(var->export, var_value);
			free(tmp_export);
			tmp_export = var->export;
			var->export = ft_strjoin(var->export, "\n");
			free(tmp_export);
		}
		else
			printf("%s%s%s\n", BLUE, var_value, DEF);
	}
	else
		print_val_2(is_return, var, tmp_export, var_value);
}

void	loop_print_env(char **env_array, t_var *variables,
						short is_return, size_t count)
{
	size_t	len_var;
	char	*var_name;
	t_list	*new_leak;

	while (variables->i < count)
	{
		len_var = 0;
		while (env_array[variables->i][len_var]
			&& env_array[variables->i][len_var] != '=')
			len_var++;
		if (env_array[variables->i][len_var] == '=')
			len_var++;
		var_name = ft_substr(env_array[variables->i], 0, len_var);
		new_leak = ft_lstnew(var_name);
		ft_lstadd_back(&variables->leaks, new_leak);
		variables->export = print_var(is_return, variables->export, var_name);
		print_val(is_return, env_array, len_var, variables);
		variables->i++;
	}
	free(env_array);
}

static char	*print_sorted_env(t_var *variables, short is_return)
{
	size_t	count;
	t_list	*tmp;
	char	**env_array;

	count = 0;
	variables->i = 0;
	variables->export = NULL;
	tmp = variables->environment;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	env_array = malloc(sizeof(char *) * count);
	tmp = variables->environment;
	while (variables->i < count)
	{
		env_array[variables->i] = (char *)tmp->content;
		tmp = tmp->next;
		variables->i++;
	}
	sort_env(env_array, count);
	variables->i = 0;
	loop_print_env(env_array, variables, is_return, count);
	return (variables->export);
}

char	*print_export(t_var *variables, short is_return, char *tmp, char *tmp2)
{
	t_list	*all_environment;
	t_list	*new_leak;
	char	*export;

	variables->is_export_cmd = 1;
	variables->environment = all_env(NULL, NULL, 1, variables);
	all_environment = variables->environment;
	while (all_environment)
	{
		new_leak = ft_lstnew(all_environment);
		ft_lstadd_back(&variables->leaks, new_leak);
		all_environment = all_environment->next;
	}
	export = print_sorted_env(variables, is_return);
	variables->is_export_cmd = 1;
	variables->environment = all_env(NULL, NULL, 2, variables);
	all_environment = variables->environment;
	while (all_environment)
	{
		new_leak = ft_lstnew(all_environment);
		ft_lstadd_back(&variables->leaks, new_leak);
		all_environment = all_environment->next;
	}
	1 && (tmp = export, tmp2 = print_sorted_env(variables, is_return));
	return (export = ft_strjoin(export, tmp2), free(tmp), free(tmp2), export);
}

void	add_export_in_list(char **tokens, size_t i,
							short is_there_equal, t_var *variables)
{
	size_t	j;

	j = 0;
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
			all_env(tokens[i], NULL, 0, variables);
	}
	else
		ft_lstadd_back(&variables->export_list, ft_lstnew(tokens[i]));
}

short	add_again_env(char **tokens, size_t *i, size_t j, t_var *variables)
{
	char	*tmp;

	tmp = ft_substr(tokens[*i], 0, j);
	if (is_exist_in_env(tmp, variables->env, -1))
	{
		1 && (all_env(tokens[*i], NULL, 0, variables), (*i)++);
		free(tmp);
		return (0);
	}
	free(tmp);
	return (1);
}

char	*export_cmd(char **tokens, short is_return, t_var *variables)
{
	size_t	i;
	size_t	j;
	short	is_there_equal;

	i = 1;
	if (!tokens[i])
		return (print_export(variables, is_return, NULL, NULL));
	while (tokens[i])
	{
		1 && (j = 0, is_there_equal = 0);
		if (check_export_arg(tokens[i]))
		{
			while (tokens[i][j] && tokens[i][j] != '=')
				j++;
			if (!add_again_env(tokens, &i, j, variables))
				continue ;
			add_export_in_list(tokens, i, is_there_equal, variables);
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
				all_env(NULL, tokens[j], 0, variables);
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

void	check_exit_args(char **tokens, int *status)
{
	if (*status == -3)
	{
		ft_putstr_fd("\033[34minishell: \033[0mexit: ", 2);
		ft_putstr_fd("\033[31mnumeric argument required.\033[0m\n", 2);
		*status = ft_status(255, 1);
	}
	else if (tokens[2])
	{
		*status = -2;
		ft_putstr_fd("\033[34mminishell:\033[0m exit: ", 2);
		ft_putstr_fd("\033[31mtoo many arguments.\033[0m\n", 2);
		ft_status(1, 1);
	}
}

void	exit_cmd(char **copy_env, t_var *variables,
				char **tokens, short is_print)
{
	int	status;

	status = -2;
	if (is_print)
		printf(RED "exit\n" DEF);
	if (!tokens || !tokens[1])
		status = ft_status(0, 0);
	else if (tokens[1])
	{
		status = ft_atoi(tokens[1]);
		check_exit_args(tokens, &status);
	}
	if (status != -2)
	{
		free(copy_env);
		free_leaks(variables);
		free(variables);
		exit(status);
	}
}
