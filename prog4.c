#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILDREN 3

int main()
{
    pid_t child_pid[NUM_CHILDREN];
    pid_t pid;
    int status;

    printf("Parent Process: PID = %d\n\n", getpid());

    /* Create multiple child processes */
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            printf("Child %d: PID = %d, PPID = %d\n",
                   i + 1, getpid(), getppid());

            sleep(i + 1);

            printf("Child %d: Completed\n", i + 1);
            exit((i + 1) * 10);
        }

        child_pid[i] = pid;
    }

    printf("\n--- Using wait() ---\n");

    pid = wait(&status);

    if (pid > 0 && WIFEXITED(status))
    {
        printf("wait(): Child PID %d terminated with exit status %d\n",
               pid, WEXITSTATUS(status));
    }

    printf("\n--- Using waitpid() ---\n");

    pid = waitpid(child_pid[1], &status, 0);

    if (pid > 0 && WIFEXITED(status))
    {
        printf("waitpid(): Child PID %d terminated with exit status %d\n",
               pid, WEXITSTATUS(status));
    }

    /* Reap remaining children */
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        if (child_pid[i] == pid)
            continue;

        pid = waitpid(child_pid[i], &status, 0);

        if (pid > 0 && WIFEXITED(status))
        {
            printf("waitpid(): Child PID %d terminated with exit status %d\n",
                   pid, WEXITSTATUS(status));
        }
    }

    printf("\nParent: All children have completed.\n");

    printf("\n--- Zombie Process Demonstration ---\n");

    pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        printf("Zombie Child: PID = %d\n", getpid());
        printf("Zombie Child: Exiting...\n");
        exit(10);
    }
    else
    {
        printf("Zombie Parent: Child PID = %d\n", pid);
        printf("Child has terminated.\n");
        printf("Parent has NOT called wait() yet.\n");
        printf("Therefore, the child becomes a ZOMBIE.\n");
        printf(">>> Check the process table NOW using: ps -el\n");
        printf(">>> You have 20 seconds.\n\n");

        sleep(20);

        printf("Parent: Calling wait() now...\n");

        if (wait(&status) == pid)
        {
            if (WIFEXITED(status))
            {
                printf("Parent: Child exited normally.\n");
                printf("Parent: Child exit status = %d\n",
                       WEXITSTATUS(status));
            }

            printf("Parent: Child has been reaped.\n");
            printf("Parent: Zombie process eliminated.\n");
        }
    }

    return 0;
}
