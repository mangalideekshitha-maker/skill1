#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define SIZE (100 * 1024 * 1024)

int main(void)
{
    int i;

    /* =====================================================
       PART 1: DYNAMIC MEMORY ALLOCATION
       malloc(), calloc(), realloc(), free()
       ===================================================== */

    int *malloc_ptr;
    int *calloc_ptr;
    int *temp;

    printf("========================================\n");
    printf("PART 1: DYNAMIC MEMORY ALLOCATION\n");
    printf("========================================\n\n");

    /* malloc() */
    printf("1. malloc() demonstration\n");

    malloc_ptr = malloc(5 * sizeof(int));

    if (malloc_ptr == NULL)
    {
        printf("malloc() failed\n");
        return 1;
    }

    for (i = 0; i < 5; i++)
    {
        malloc_ptr[i] = (i + 1) * 10;
    }

    printf("Memory allocated using malloc():\n");

    for (i = 0; i < 5; i++)
    {
        printf("%d ", malloc_ptr[i]);
    }

    printf("\n\n");

    /* calloc() */
    printf("2. calloc() demonstration\n");

    calloc_ptr = calloc(5, sizeof(int));

    if (calloc_ptr == NULL)
    {
        printf("calloc() failed\n");
        free(malloc_ptr);
        return 1;
    }

    printf("Memory allocated using calloc():\n");

    for (i = 0; i < 5; i++)
    {
        printf("%d ", calloc_ptr[i]);
    }

    printf("\n\n");

    /* realloc() */
    printf("3. realloc() demonstration\n");

    temp = realloc(malloc_ptr, 10 * sizeof(int));

    if (temp == NULL)
    {
        printf("realloc() failed\n");
        free(malloc_ptr);
        free(calloc_ptr);
        return 1;
    }

    malloc_ptr = temp;

    for (i = 5; i < 10; i++)
    {
        malloc_ptr[i] = (i + 1) * 10;
    }

    printf("Memory after realloc():\n");

    for (i = 0; i < 10; i++)
    {
        printf("%d ", malloc_ptr[i]);
    }

    printf("\n\n");

    /* free() */
    printf("4. free() demonstration\n");

    free(malloc_ptr);
    malloc_ptr = NULL;

    free(calloc_ptr);
    calloc_ptr = NULL;

    printf("Allocated memory successfully released.\n\n");


    /* =====================================================
       PART 2: COPY-ON-WRITE AFTER fork()
       ===================================================== */

    printf("========================================\n");
    printf("PART 2: COPY-ON-WRITE (COW)\n");
    printf("========================================\n\n");

    char *memory;

    memory = malloc(SIZE);

    if (memory == NULL)
    {
        printf("Large memory allocation failed\n");
        return 1;
    }

    /* Initialize the memory */
    memset(memory, 'A', SIZE);

    printf("Allocated and initialized 100 MB memory.\n");
    printf("Parent PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        free(memory);
        return 1;
    }

    if (pid == 0)
    {
        /* Child process */
        printf("\nChild process started.\n");
        printf("Child PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());

        printf("Child modifying memory pages...\n");

        for (i = 0; i < SIZE; i += 4096)
        {
            memory[i] = 'B';
        }

        printf("Child modified memory.\n");
        printf("Child sleeping for 20 seconds.\n");
        printf("Check /proc/%d/status or /proc/%d/smaps\n",
               getpid(), getpid());

        sleep(20);

        free(memory);
        return 0;
    }
    else
    {
        /* Parent process */
        printf("\nParent created child process.\n");
        printf("Child PID: %d\n", pid);

        printf("Parent sleeping while child modifies memory.\n");
        printf("Check /proc/%d/status or /proc/%d/smaps\n",
               pid, pid);

        sleep(20);

        wait(NULL);

        printf("Child process completed.\n");

        free(memory);
    }

    printf("\nPractical 8 completed.\n");

    return 0;
}
