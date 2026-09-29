#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define SERVER_FIFO "/tmp/server_fifo"
#define MAX_MSG 256

typedef struct
{
    pid_t client_pid;
    char message[MAX_MSG];
} Request;

typedef struct
{
    char response[MAX_MSG];
} Response;

volatile sig_atomic_t sigint_received = 0;
volatile sig_atomic_t sigterm_received = 0;
volatile sig_atomic_t sigusr1_received = 0;

/* Signal Handler */
void signal_handler(int signo)
{
    if (signo == SIGINT)
    {
        sigint_received = 1;
    }
    else if (signo == SIGTERM)
    {
        sigterm_received = 1;
    }
    else if (signo == SIGUSR1)
    {
        sigusr1_received = 1;
    }
}

/* Signal Handling Demonstration */
void signal_demo()
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction SIGTERM");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    printf("\n===== POSIX SIGNAL HANDLING =====\n");
    printf("Signal handling program started.\n");
    printf("Process PID = %d\n", getpid());

    printf("\nSend signals from another terminal:\n");
    printf("SIGINT  : kill -SIGINT %d\n", getpid());
    printf("SIGTERM : kill -SIGTERM %d\n", getpid());
    printf("SIGUSR1 : kill -SIGUSR1 %d\n", getpid());

    while (1)
    {
        pause();

        if (sigint_received)
        {
            printf("\nSIGINT received!\n");
            printf("Interrupt signal handled.\n");
            sigint_received = 0;
        }

        if (sigusr1_received)
        {
            printf("SIGUSR1 received!\n");
            printf("User-defined event handled.\n");
            sigusr1_received = 0;
        }

        if (sigterm_received)
        {
            printf("SIGTERM received!\n");
            printf("Termination requested.\n");
            break;
        }
    }

    printf("Program terminating gracefully...\n");
}

/* FIFO Client */
void fifo_client()
{
    int server_fd, client_fd;
    char client_fifo[100];
    char message[MAX_MSG];

    Request request;
    Response response;

    pid_t pid = getpid();

    snprintf(client_fifo,
             sizeof(client_fifo),
             "/tmp/client_%d_fifo",
             pid);

    if (mkfifo(client_fifo, 0666) == -1)
    {
        perror("mkfifo client");
        exit(EXIT_FAILURE);
    }

    printf("Client PID: %d\n", pid);
    printf("Enter message: ");

    fgets(message, sizeof(message), stdin);

    message[strcspn(message, "\n")] = '\0';

    request.client_pid = pid;
    strcpy(request.message, message);

    server_fd = open(SERVER_FIFO, O_WRONLY);

    if (server_fd == -1)
    {
        perror("Unable to open server FIFO");
        unlink(client_fifo);
        exit(EXIT_FAILURE);
    }

    write(server_fd, &request, sizeof(request));

    close(server_fd);

    printf("Message sent to server.\n");

    client_fd = open(client_fifo, O_RDONLY);

    if (client_fd == -1)
    {
        perror("Unable to open client FIFO");
        unlink(client_fifo);
        exit(EXIT_FAILURE);
    }

    read(client_fd, &response, sizeof(response));

    printf("Server Response: %s\n", response.response);

    close(client_fd);

    unlink(client_fifo);
}

/* FIFO Server */
void fifo_server()
{
    int server_fd;
    Request request;
    char client_fifo[100];

    if (mkfifo(SERVER_FIFO, 0666) == -1 && errno != EEXIST)
    {
        perror("mkfifo server");
        exit(EXIT_FAILURE);
    }

    printf("\n===== CLIENT-SERVER USING NAMED PIPE =====\n");
    printf("Server started...\n");
    printf("Waiting for clients...\n");

    server_fd = open(SERVER_FIFO, O_RDWR);

    if (server_fd == -1)
    {
        perror("open server FIFO");
        unlink(SERVER_FIFO);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        ssize_t n = read(server_fd, &request, sizeof(Request));

        if (n <= 0)
        {
            continue;
        }

        printf("\nReceived from Client PID %d: %s\n",
               request.client_pid,
               request.message);

        snprintf(client_fifo,
                 sizeof(client_fifo),
                 "/tmp/client_%d_fifo",
                 request.client_pid);

        int client_fd = open(client_fifo, O_WRONLY);

        if (client_fd == -1)
        {
            perror("open client FIFO");
            continue;
        }

        Response response;

        snprintf(response.response,
                 sizeof(response.response),
                 "Server processed: %s",
                 request.message);

        write(client_fd, &response, sizeof(response));

        close(client_fd);

        printf("Response sent to Client PID %d\n",
               request.client_pid);
    }

    close(server_fd);
    unlink(SERVER_FIFO);
}

int main()
{
    int choice;

    printf("========================================\n");
    printf("       PRACTICAL SESSION 6\n");
    printf("========================================\n");

    printf("\n1. FIFO Server\n");
    printf("2. FIFO Client\n");
    printf("3. POSIX Signal Handling\n");

    printf("\nEnter choice: ");
    scanf("%d", &choice);
    getchar();

    if (choice == 1)
    {
        fifo_server();
    }
    else if (choice == 2)
    {
        fifo_client();
    }
    else if (choice == 3)
    {
        signal_demo();
    }
    else
    {
        printf("Invalid choice.\n");
    }

    return 0;
}
