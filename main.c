#include "minishell.h"

int main(int argc, char **argv, char **env)
{
	int fd[2];

	pipe(fd);
	int pid = fork();
	if (pid == 0)
	{
		dup2(fd[1], 1);
		printf("Hello World\n");
	}
	else
	{
		wait(NULL);
		close(fd[1]);
	}
	return 0;
}
