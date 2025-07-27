/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 19:15:34 by youbella          #+#    #+#             */
/*   Updated: 2025/07/27 08:50:33 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *search_cmd(char *cmd)
{
	int i;
	char *env_path;
	char **split_env_path;
	char *join_cmd_to_path;

	i = 0;
	if (cmd[0] == '.' && cmd[1] == '/')
		return (cmd);
	env_path = getenv("PATH");
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

void del(void *content)
{
	free(content);
}

void handle_signal(int sig)
{
	(void)sig;
	printf("\n");
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

char *ft_getenv(char *var, t_list *export_list)
{
	char *var_env;
	char *env;
	int i;

	env = getenv(var);
	if (env)
		return (ft_strdup(env));
	while (export_list)
	{
		i = 0;
		while (((char *)export_list->content)[i])
		{
			if (((char *)export_list->content)[i] == '=')
				break;
			i++;
		}
		var_env = ft_substr((char *)export_list->content, 0, i);
		env = ft_substr((char *)export_list->content, i + 1, ft_strlen((char *)export_list->content) - i - 1);
		if (ft_strlen(var) == ft_strlen(var_env) && !ft_strncmp(var, var_env, ft_strlen(var)))
			return (env);
		export_list = export_list->next;
	}
	return (NULL);
}

void remove_node(t_list **list, t_list *remove, void (*del)(void *))
{
	t_list *prev;
	t_list *temp;

	prev = NULL;
	temp = *list;
	if (!list || !remove || !del)
		return;
	while (temp != remove)
	{
		prev = temp;
		temp = temp->next;
	}
	if (prev)
		prev->next = temp->next;
	else
		*list = temp->next;
	ft_lstdelone(temp, del);
}

char *echo_cmd(char **tokens, short is_return, int *status)
{
	int j;
	short is_op_echo;
	char *str;

	j = 1;
	is_op_echo = 0;
	str = NULL;
	if (tokens[j] && !ft_strncmp(tokens[j], "-n", 2) && ft_strlen(tokens[j]) == 2)
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
			printf("%s ", tokens[j]);
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

char *is_there_cmd(char **tokens, int *status)
{
	char *path_cmd;

	path_cmd = search_cmd(tokens[0]);
	if (!path_cmd)
	{
		printf(RED "minishell: %s%s%s command not found.\n", BLUE, tokens[0], DEF);
		*status = 32512;
		return (NULL);
	}
	return (path_cmd);
}

char *env_cmd(char **env, t_list *export_list, short is_return)
{
	t_list *copy_export_list;
	char *environment;
	int j;

	j = 1;
	environment = NULL;
	while (env[j])
	{
		if (is_return)
		{
			environment = ft_strjoin(environment, env[j]);
			environment = ft_strjoin(environment, "\n");
		}
		else
			printf("%s\n", env[j]);
		j++;
	}
	copy_export_list = export_list;
	while (copy_export_list)
	{
		if (is_return)
		{
			environment = ft_strjoin(environment, (char *)copy_export_list->content);
			environment = ft_strjoin(environment, "\n");
		}
		else
			printf("%s\n", (char *)copy_export_list->content);
		copy_export_list = copy_export_list->next;
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

void export_cmd(char **tokens, t_list **export_list)
{
	int j;
	char *args_export;
	char **split_export;

	j = 1;
	args_export = NULL;
	split_export = NULL;
	while (tokens[j])
	{
		if (check_export_arg(tokens[j]))
		{
			args_export = ft_strjoin(args_export, tokens[j]);
			args_export = ft_strjoin(args_export, " ");
		}
		j++;
	}
	split_export = ft_split(args_export, ' ');
	if (!split_export)
		return;
	j = 0;
	while (split_export[j])
	{
		ft_lstadd_back(export_list, ft_lstnew(ft_strdup(split_export[j])));
		j++;
	}
}

void unset_cmd(char **tokens, t_list **export_list)
{
	t_list *s;
	int j;
	char *args_unset;
	char **split_unset;

	args_unset = NULL;
	split_unset = NULL;
	j = 1;
	while (tokens[j])
	{
		if (check_unset_arg(tokens[j]))
		{
			args_unset = ft_strjoin(args_unset, tokens[j]);
			args_unset = ft_strjoin(args_unset, " ");
		}
		j++;
	}
	split_unset = ft_split(args_unset, ' ');
	if (!split_unset)
		return;
	j = 0;
	while (split_unset[j])
	{
		s = search_in_list(split_unset[j], *export_list);
		if (s)
			remove_node(export_list, s, del);
		j++;
	}
}

char *herdoc(char *cmd_line, char **env, int *status, t_list **export_list, char *pwd, short is_return)
{
	t_redirections *her;
	char *join_herdoc;
	char *input;
	char *output;
	char *join_output;
	// char *cmd_line;
	char *path_cmd;
	char *cmd_line_until_redirect;
	char **tokens_until_redirect;
	int fd[2];
	int fd_out[2];
	pid_t pid;
	int fd_file = 0;
	join_output = NULL;
	// cmd_line = join_tokens(tokens);
	her = add_redirections_list(cmd_line);
	if (!her)
		return (NULL);
	while (her)
	{
		join_herdoc = NULL;
		fd_file = 0;
		if (!ft_strncmp(her->type_redirection, "<<", 2))
		{
			while (1)
			{
				input = readline(CYAN "heredoc > " DEF);
				if (!input)
					break;
				add_history(input);
				if (ft_strlen(her->file_name) == ft_strlen(input) && !ft_strncmp(her->file_name, input, ft_strlen(her->file_name)))
				{
					input = NULL;
					break;
				}
				join_herdoc = ft_strjoin(join_herdoc, input);
				if (join_herdoc)
					join_herdoc = ft_strjoin(join_herdoc, "\n");
			}
		}
		else
		{
			int i = 0;
			char **split_files = ft_split(her->file_name, ' ');
			while (split_files[i])
			{
				fd_file = open(split_files[i], O_RDONLY);
				if (fd_file < 0)
				{
					printf(RED "minishell: %s%s%s: No such file or directory.\n", BLUE, split_files[i], DEF);
					return (NULL);
				}
				i++;
			}
		}
		her = her->next;
	}
	cmd_line_until_redirect = malloc(strlen_until_redirections(cmd_line) + 1);
	strcpy_until_redirections(cmd_line_until_redirect, cmd_line, ft_strlen(cmd_line) + 1);
	tokens_until_redirect = ft_split_first_cmd(cmd_line_until_redirect, ' ', *status, *export_list);
	if (!ft_strncmp(tokens_until_redirect[0], "echo", 4) && ft_strlen(tokens_until_redirect[0]) == 4)
	{
		output = echo_cmd(tokens_until_redirect, 1, status);
		if (!is_return)
		{
			printf("%s", output);
			return (NULL);
		}
		else
			return (output);
	}
	else if (!ft_strncmp(tokens_until_redirect[0], "env", 3) && ft_strlen(tokens_until_redirect[0]) == 3)
	{
		output = env_cmd(env, *export_list, 1);
		if (!is_return)
			printf("%s", output);
		else
			return (output);
	}
	else if (ft_strlen(tokens_until_redirect[0]) == 6 && !ft_strncmp(tokens_until_redirect[0], "export", 6))
	{
		export_cmd(tokens_until_redirect, export_list);
		return ("export");
	}
	else if (ft_strlen(tokens_until_redirect[0]) == 5 && !ft_strncmp(tokens_until_redirect[0], "unset", 5))
	{
		unset_cmd(tokens_until_redirect, export_list);
		return ("unset");
	}
	else if (!ft_strncmp(tokens_until_redirect[0], "pwd", 3) && ft_strlen(tokens_until_redirect[0]) == 3)
	{
		if (!is_return)
		{
			printf("%s", ft_strjoin(pwd, "\n"));
			return (NULL);
		}
		else
			return (ft_strjoin(pwd, "\n"));
	}
	else if (!ft_strncmp(tokens_until_redirect[0], "cd", 2) && ft_strlen(tokens_until_redirect[0]) == 2)
	{
		if (is_return)
		{
			if (!tokens_until_redirect[1])
				return ("NULL");
			return (tokens_until_redirect[1]);
		}
		cd_cmd(tokens_until_redirect[1]);
		return (NULL);
	}
	path_cmd = is_there_cmd(tokens_until_redirect, status);
	if (!path_cmd)
		return (NULL);
	pipe(fd);
	pipe(fd_out);
	if (join_herdoc)
		write(fd[1], join_herdoc, ft_strlen(join_herdoc));
	close(fd[1]);
	pid = fork();
	if (pid == 0)
	{
		dup2(fd[0], 0);
		if (fd_file > 0)
			dup2(fd_file, 0);
		if (is_return)
			dup2(fd_out[1], 1);
		close(fd[0]);
		close(fd_out[0]);
		execve(path_cmd, tokens_until_redirect, env);
		exit(1);
	}
	else
	{
		waitpid(pid, status, 0);
		close(fd[0]);
		close(fd_out[1]);
	}
	output = get_next_line(fd_out[0]);
	while (output)
	{
		join_output = ft_strjoin(join_output, output);
		output = get_next_line(fd_out[0]);
	}
	return (join_output);
}

char *redirect_output(char *cmd_line, char *pwd, char **env, int *status, t_list **export_list, char *herdoc_output)
{
	t_redirections *list_redirections_output;
	pid_t pid;
	char *cmd_line_until_redirect;
	char **tokens_until_redirect;
	// char *cmd_line;
	int fd[2];
	int fd_file;
	char *output_line;
	char *all_output;
	char *output;
	char *path_cmd;

	// cmd_line = join_tokens(tokens);
	pipe(fd);
	list_redirections_output = add_redirections_out_in_list(cmd_line);
	all_output = NULL;
	while (list_redirections_output)
	{
		if (list_redirections_output->next)
		{
			open(list_redirections_output->file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			list_redirections_output = list_redirections_output->next;
			continue;
		}
		cmd_line_until_redirect = malloc(1000);
		strcpy_until_redirections(cmd_line_until_redirect, cmd_line, ft_strlen(cmd_line) + 1);
		tokens_until_redirect = ft_split_first_cmd(cmd_line_until_redirect, ' ', *status, *export_list);
		if (!ft_strncmp(list_redirections_output->type_redirection, ">>", 2))
			fd_file = open(list_redirections_output->file_name, O_CREAT | O_APPEND | O_WRONLY, 0644);
		else
			fd_file = open(list_redirections_output->file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (herdoc_output)
		{
			if ((ft_strlen(tokens_until_redirect[0]) == 6 && !ft_strncmp(tokens_until_redirect[0], "export", 6) && !ft_strncmp(herdoc_output, "export", 6)) ||
				(ft_strlen(tokens_until_redirect[0]) == 5 && !ft_strncmp(tokens_until_redirect[0], "unset", 5) && !ft_strncmp(herdoc_output, "unset", 5)))
				return (NULL);
			if (!ft_strncmp(tokens_until_redirect[0], "cd", 2) && ft_strlen(tokens_until_redirect[0]) == 2)
			{
				if (!ft_strncmp(herdoc_output, "NULL", 4))
					herdoc_output = NULL;
				cd_cmd(herdoc_output);
				return (NULL);
			}
			write(fd_file, herdoc_output, ft_strlen(herdoc_output));
			return (NULL);
		}
		if (!ft_strncmp(tokens_until_redirect[0], "echo", 4) && ft_strlen(tokens_until_redirect[0]) == 4)
		{
			output = echo_cmd(tokens_until_redirect, 1, status);
			write(fd_file, output, ft_strlen(output));
			break;
		}
		else if (!ft_strncmp(tokens_until_redirect[0], "env", 3) && ft_strlen(tokens_until_redirect[0]) == 3)
		{
			output = env_cmd(env, *export_list, 1);
			write(fd_file, output, ft_strlen(output));
			break;
		}
		else if (ft_strlen(tokens_until_redirect[0]) == 6 && !ft_strncmp(tokens_until_redirect[0], "export", 6))
		{
			export_cmd(tokens_until_redirect, export_list);
			break;
		}
		else if (ft_strlen(tokens_until_redirect[0]) == 5 && !ft_strncmp(tokens_until_redirect[0], "unset", 5))
		{
			unset_cmd(tokens_until_redirect, export_list);
			break;
		}
		else if (!ft_strncmp(tokens_until_redirect[0], "pwd", 3) && ft_strlen(tokens_until_redirect[0]) == 3)
		{
			write(fd_file, ft_strjoin(pwd, "\n"), ft_strlen(pwd) + 1);
			break;
		}
		else if (!ft_strncmp(tokens_until_redirect[0], "cd", 2) && ft_strlen(tokens_until_redirect[0]) == 2)
		{
			cd_cmd(tokens_until_redirect[1]);
			return (NULL);
		}
		path_cmd = is_there_cmd(tokens_until_redirect, status);
		if (!path_cmd)
			break;
		pid = fork();
		if (!pid)
		{
			close(fd[0]);
			dup2(fd[1], 1);
			execve(path_cmd, tokens_until_redirect, env);
		}
		else
		{
			wait(status);
			close(fd[1]);
			output_line = get_next_line(fd[0]);
			while (output_line)
			{
				all_output = ft_strjoin(all_output, output_line);
				output_line = get_next_line(fd[0]);
			}
			if (all_output)
				write(fd_file, all_output, ft_strlen(all_output));
		}
		list_redirections_output = list_redirections_output->next;
	}
	return (NULL);
}

void ft_pipe(char *cmd_line, int status, t_list *export_list, char **env, char *pwd)
{
	char **split_pipe = ft_split_first_cmd(cmd_line, '|', status, export_list);
	char *herdoc_output = NULL;
	char **tokens;
	int i = 0;
	int fd[2];
	int in_fd = 0;

	while (split_pipe[i])
	{
		int exits_redirect = 0;
		pipe(fd);
		tokens = ft_split_first_cmd(split_pipe[i], ' ', status, export_list);
		if ((is_there_redirect(split_pipe[i], '<') || is_there_redirect(split_pipe[i], 'h')) && !is_there_redirect(split_pipe[i], '>'))
			herdoc_output = herdoc(cmd_line, env, &status, &export_list, pwd, 0);
		else if (is_there_redirect(split_pipe[i], '>') || is_there_redirect(split_pipe[i], 'h'))
		{
			if (!is_there_redirect(split_pipe[i], '>'))
				herdoc_output = herdoc(cmd_line, env, &status, &export_list, pwd, 1);
			else
			{
				herdoc_output = herdoc(cmd_line, env, &status, &export_list, pwd, 1);
				redirect_output(cmd_line, pwd, env, &status, &export_list, herdoc_output);
			}
			exits_redirect = 1;
		}
		int pid = fork();
		if (pid == 0)
		{
			dup2(in_fd, 0);
			if (split_pipe[i + 1])
				dup2(fd[1], 1);
			if (herdoc_output)
				write(fd[1], herdoc_output, ft_strlen(herdoc_output));
			close(fd[0]);
			if (!exits_redirect)
				execve(is_there_cmd(tokens, &i), tokens, env);
			exit(1);
		}
		else
		{
			wait(NULL);
			close(fd[1]);
			in_fd = fd[0];
		}
		i++;
	}
}

int main(int argc, char **argv, char **env)
{
	struct sigaction sig;
	struct termios ctr;
	t_list *export_list;
	int i;
	int status;
	pid_t pid;
	char *cmd_line;
	char **tokens;
	char *pwd;
	char **path;
	char *this_dir;
	char *path_cmd;
	char *herdoc_output;

	if (argc != 1)
		return (printf(RED "Please do not enter any arguments.\n" DEF), 1);
	(void)argc;
	(void)argv;
	tcgetattr(0, &ctr);
	ctr.c_lflag &= ~ECHOCTL;
	tcsetattr(0, 0, &ctr);
	sig.sa_handler = handle_signal;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sig, NULL);
	signal(SIGQUIT, SIG_IGN);
	export_list = NULL;
	herdoc_output = NULL;
	status = 0;
	pid = fork();
	if (!pid)
		execve("/usr/bin/clear", (char *[]){"clear", NULL}, env);
	wait(NULL);
	printf(YELLOW "↪ Welcome to our MiniSheel 🤪 ↩\n" DEF);
	while (1)
	{
		i = 0;
		pwd = getcwd(NULL, 0);
		path = ft_split(pwd, '/');
		while (path[i])
			this_dir = path[i++];
		this_dir = ft_strjoin("\033[1;94m", this_dir);
		this_dir = ft_strjoin(this_dir, "\033[0m");
		this_dir = ft_strjoin("\033[32m➥\033[0m ", this_dir);
		this_dir = ft_strjoin(this_dir, " ");
		cmd_line = readline(this_dir);
		if (!cmd_line)
			break;
		if (!cmd_line[0])
			continue;
		add_history(cmd_line);
		tokens = ft_split_first_cmd(cmd_line, ' ', WEXITSTATUS(status), export_list);
		if (!tokens || !tokens[0])
			continue;
		if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "exit", 4))
			break;
		else if (0)
		{
			ft_pipe(cmd_line, status, export_list, env, pwd);
			continue;
		}
		if ((is_there_redirect(cmd_line, '<') || is_there_redirect(cmd_line, 'h')) && !is_there_redirect(cmd_line, '>'))
			herdoc(cmd_line, env, &status, &export_list, pwd, 0);
		else if (is_there_redirect(cmd_line, '>'))
		{
			herdoc_output = herdoc(cmd_line, env, &status, &export_list, pwd, 1);
			redirect_output(cmd_line, pwd, env, &status, &export_list, herdoc_output);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 6 && !ft_strncmp(tokens[0], "export", 6))
		{
			export_cmd(tokens, &export_list);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 5 && !ft_strncmp(tokens[0], "unset", 5))
		{
			unset_cmd(tokens, &export_list);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "env", 3))
		{
			env_cmd(env, export_list, 0);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 3 && !ft_strncmp(tokens[0], "pwd", 3))
		{
			printf("%s\n", pwd);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 4 && !ft_strncmp(tokens[0], "echo", 4))
		{
			echo_cmd(tokens, 0, &status);
			continue;
		}
		else if (ft_strlen(tokens[0]) == 2 && !ft_strncmp(tokens[0], "cd", 2))
		{
			cd_cmd(tokens[1]);
			continue;
		}
		path_cmd = is_there_cmd(tokens, &status);
		if (!path_cmd)
			continue;
		pid = fork();
		if (pid == 0)
			execve(path_cmd, tokens, env);
		else
			wait(&status);
		free(cmd_line);
	}
	return (0);
}
