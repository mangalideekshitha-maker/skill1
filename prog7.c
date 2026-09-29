#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Global variables */
int global_var = 10;
int global_uninit;

/* Static variable */
static int static_var = 20;

/* Function to display code address */
void display_code_address(void)
{
    printf("Code address   : %p\n", (void *)display_code_address);
}

int main(void)
{
    int stack_var = 30;
    int *heap_var = (int *)malloc(sizeof(int));

    if (heap_var == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    *heap_var = 40;

    printf("========================================\n");
    printf("   PRACTICAL SESSION 7\n");
    printf("   Linux Process Address Space\n");
    printf("========================================\n\n");

    /* Part 1: Address Space */
    printf("----- Process Memory Addresses -----\n");

    display_code_address();

    printf("Global address : %p\n", (void *)&global_var);
    printf("Static address : %p\n", (void *)&static_var);
    printf("BSS address    : %p\n", (void *)&global_uninit);
    printf("Heap address   : %p\n", (void *)heap_var);
    printf("Stack address  : %p\n", (void *)&stack_var);

    printf("\nProcess ID (PID): %d\n", getpid());

    printf("\n----- Process is running -----\n");
    printf("Use another terminal to examine:\n");
    printf("cat /proc/%d/maps\n", getpid());
    printf("\nPress Ctrl+C to stop the process.\n");

    /* Keep process running for /proc/<PID>/maps analysis */
    while (1)
    {
        sleep(10);
    }

    free(heap_var);

    return 0;
}
