/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youbella <youbella@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 04:45:56 by youbella          #+#    #+#             */
/*   Updated: 2025/08/26 16:27:10 by youbella         ###   ########.fr       */
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
			1 && (close(0), ft_status(1, 1), printf("\n"));
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

int	main(int argc, char **argv, char **env)
{
	t_var			*variables;
	int				status;
	pid_t			pid;
	char			**copy_env;
	struct termios	ctr;

	if (argc != 1 && argv)
		return (printf(RED "Please do not enter any arguments.\n" DEF), 1);
	1 && (status = 0, variables = malloc(sizeof(t_var)));
	if (!variables)
		return (ft_status(1, 1), 1);
	ft_memset(variables, 0, sizeof(t_var));
	1 && (variables->cd_flag = 0, variables->env = env);
	1 && (variables->environment = NULL, variables->export_list = NULL);
	1 && (variables->leaks = NULL, variables->cd_flag = 0);
	1 && (variables->ctr = &ctr, copy_env = copy_environment(env));
	variables->copy_env = copy_env;
	setup_terminal(&ctr);
	pid = fork();
	if (!pid)
		execve("/usr/bin/clear", (char *[]){"clear", NULL}, env);
	1 && (waitpid(pid, &status, 0), ft_status(WEXITSTATUS(status), 1));
	printf(YELLOW "↪ Welcome to our MiniShell 🤪 ↩\n" DEF);
	minishell_loop(variables, copy_env, &ctr);
	return (0);
}
