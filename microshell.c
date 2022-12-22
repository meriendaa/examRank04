#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int ft_strlen(char *str)
{
    int i = 0;
    while(str[i])
        i++;
    return (i);
}
int ft_put(char *str)
{
    write(2, str, ft_strlen(str));
    return(1);
}
int ft_ex(char *argv[], int i, int tmp_fd, char *envp[])
{
    argv[i] = NULL;
    dup2(tmp_fd, 0);
    close(tmp_fd);
    execve(argv[0], argv, envp);
    ft_put("error: cannot execute ");
    ft_put(argv[0]);
    ft_put("\n");
    return(1);
}
int main(int argc, char *argv[], char *envp[])
{
    int i;
    int pid;
    int fd[2];
    int tmp_fd;
    (void)argc;
    i = 0;
    tmp_fd = dup(0);
    while(argv[i] && argv[i + 1])
    {
        argv = &argv[i + 1];
        i = 0;
        while(argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            i++;
        if(strcmp(argv[0], "cd") == 0)
        {
            if (i != 2)
                ft_put("error: cd: bad arguments\n");
            else if(chdir(argv[1]) == -1)
            {
                ft_put("error: cd: cannot change directory to ");
                ft_put(argv[1]);
                ft_put("\n");
            }
        }
        else if(i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
        {
            pid = fork();
            if(pid == -1)
                return (1);
            if (pid == 0)
            {
                if(ft_ex(argv, i, tmp_fd, envp))
                    return(1);
            }
            else
            {
                close(tmp_fd);
                waitpid(pid, NULL, WUNTRACED);
                tmp_fd = dup(0);
            }
        }
        else if(i != 0 && strcmp(argv[i], "|") == 0)
        {
            pipe(fd);
            pid = fork();
            if(pid == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                if(ft_ex(argv, i, tmp_fd, envp))
                    return(1);
            }
            else
            {
                close(tmp_fd);
                close(fd[1]);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
    return(0);
}
