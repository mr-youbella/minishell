#include "minishell.h"

char *is_there_cmd(char **tokens, int *status);

int main(int argc, char **argv, char **env)
{
    char *str = "echo Hello World | grep gf | wc";
    char **split = ft_split(str, '|'); 
    int i = 0;
    int fd[2];
    int in_fd = 0; 

    while (split[i])
    {
        pipe(fd); 
        int pid = fork();
        if (pid == 0)
        {
            dup2(in_fd, 0);
            if (split[i + 1])
                dup2(fd[1], 1);
            close(fd[0]);
            char **cmd = ft_split_first_cmd(split[i], ' ', 137, NULL);
            execve(is_there_cmd(cmd, &i), cmd, env);
        }
        else
        {
            wait(NULL);
            close(fd[1]);
            in_fd = fd[0];
        }
        i++;
    }
    return 0;
}
