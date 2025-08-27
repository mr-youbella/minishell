/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 04:45:56 by youbella          #+#    #+#             */
/*   Updated: 2025/08/27 00:58:42 by youbella         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_status(int status, short is_change)
{
	static int	new_status;

	if (is_change)
		new_status = status;
	return (new_status);
}

static void	handle_sigquit(pid_t pid)
{
	if (pid > 0 && g_signal_flag != -3)
	{
		printf("^\\Quit: 3\n");
		if (g_signal_flag > 0)
			ft_status(131, 1);
	}
	else
	{
		rl_on_new_line();
		rl_redisplay();
	}
}

static void	handle_signal(int sig_num)
{
	pid_t	pid;

	pid = wait(NULL);
	if (g_signal_flag == -3)
	{
		if (sig_num == SIGINT)
			1 && (g_signal_flag = -5, close(0), ft_status(1, 1), printf("\n"));
	}
	else if (sig_num == SIGINT)
	{
		if (pid > 0)
		{
			if (g_signal_flag > 0)
				1 && (printf("^C\n"), ft_status(130, 1));
		}
		else
		{
			1 && (printf("\n"), rl_on_new_line());
			1 && (rl_replace_line("", 0), rl_redisplay(), ft_status(1, 1));
		}
	}
	if (sig_num == SIGQUIT)
		handle_sigquit(pid);
}

static void	setup_terminal(struct termios *ctr)
{
	tcgetattr(0, ctr);
	ctr->c_lflag &= ~ECHOCTL;
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
}

char	*defualt_env(char *str, t_var *variables)
{
	t_list	*new_leak;
	char	*new_env;

	new_env = ft_strdup(str);
	new_leak = ft_lstnew(new_env);
	ft_lstadd_back(&variables->leaks, new_leak);
	return (new_env);
}

char	**add_env(char **env, t_var *variables)
{
	char	*pwd;
	char	**new_env;

	if (!env[0])
	{
		pwd = getcwd(NULL, 0);
		new_env = malloc(6 * sizeof(char *));
		if (!new_env)
			return (NULL);
		new_env[0] = defualt_env("OLDPWD", variables);
		new_env[1] = defualt_env
			("PATH=/usr/gnu/bin:/usr/local/bin:/bin:/usr/bin:.", variables);
		new_env[2] = defualt_env(ft_strjoin("PWD=", pwd), variables);
		new_env[3] = defualt_env("SHLVL=2", variables);
		new_env[4] = defualt_env("_=/usr/bin/env", variables);
		new_env[5] = NULL;
		free(pwd);
		return (new_env);
	}
	return (NULL);
}

int	main(int argc, char **argv, char **env)
{
	t_var			*variables;
	char			**copy_env;
	char			**new_env;
	struct termios	ctr;

	if (argc != 1 && argv)
		return (printf(RED "Please do not enter any arguments.\n" DEF), 1);
	1 && (variables = malloc(sizeof(t_var)));
	if (!variables)
		return (ft_status(1, 1), 1);
	ft_memset(variables, 0, sizeof(t_var));
	new_env = add_env(env, variables);
	if (new_env)
		env = new_env;
	1 && (variables->cd_flag = 0, variables->env = env);
	1 && (variables->environment = NULL, variables->export_list = NULL);
	1 && (variables->leaks = NULL, variables->cd_flag = 0);
	1 && (variables->ctr = &ctr, copy_env = copy_environment(env));
	variables->copy_env = copy_env;
	setup_terminal(&ctr);
	printf(YELLOW "↪ Welcome to our MiniShell 🤪 ↩\n" DEF);
	minishell_loop(variables, copy_env, &ctr);
	return (0);
}
