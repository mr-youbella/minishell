#include "minishell.h"

char *is_there_cmd(char **tokens, int *status);

int main(int argc, char **argv, char **env)
{
	int i = 0;
	char *str = "echo Hello World | grep Hello | wc";
	char **split = ft_split(str, '|');
	int *fd;
	// int *fd1;
	// pipe(fd1);
	pipe(fd);
	while (split[i])
	{
		int pid = fork();
		if (pid == 0)
		{
			dup2(fd[1], 1);
			execve(is_there_cmd(ft_split_first_cmd(split[i], ' ', 137, NULL), &pid), ft_split_first_cmd(split[i], ' ', 137, NULL), env);
		}
		wait(NULL);
		
		i++;
	}
	
}
