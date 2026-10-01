#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 8192

void low_level_copy(const char *source, const char *destination)
{
    int src_fd, dest_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_written;

    printf("\n========================================\n");
    printf("PART 1A: LOW-LEVEL FILE COPY\n");
    printf("========================================\n");

    src_fd = open(source, O_RDONLY);

    if (src_fd == -1)
    {
        perror("open source");
        return;
    }

    dest_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (dest_fd == -1)
    {
        perror("open destination");
        close(src_fd);
        return;
    }

    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0)
    {
        char *ptr = buffer;
        ssize_t remaining = bytes_read;

        while (remaining > 0)
        {
            bytes_written = write(dest_fd, ptr, remaining);

            if (bytes_written == -1)
            {
                if (errno == EINTR)
                    continue;

                perror("write");
                close(src_fd);
                close(dest_fd);
                return;
            }

            ptr += bytes_written;
            remaining -= bytes_written;
        }
    }

    if (bytes_read == -1)
    {
        perror("read");
        close(src_fd);
        close(dest_fd);
        return;
    }

    off_t file_size = lseek(src_fd, 0, SEEK_END);

    if (file_size == (off_t)-1)
    {
        perror("lseek");
    }
    else
    {
        printf("Source file size: %lld bytes\n",
               (long long)file_size);
    }

    close(src_fd);
    close(dest_fd);

    printf("Low-level file copy completed.\n");
}

void stdio_copy(const char *source, const char *destination)
{
    FILE *src;
    FILE *dest;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    printf("\n========================================\n");
    printf("PART 1B: STANDARD LIBRARY FILE COPY\n");
    printf("========================================\n");

    src = fopen(source, "rb");

    if (src == NULL)
    {
        perror("fopen source");
        return;
    }

    dest = fopen(destination, "wb");

    if (dest == NULL)
    {
        perror("fopen destination");
        fclose(src);
        return;
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src)) > 0)
    {
        size_t total_written = 0;

        while (total_written < bytes_read)
        {
            size_t bytes_written;

            bytes_written = fwrite(buffer + total_written,
                                   1,
                                   bytes_read - total_written,
                                   dest);

            if (bytes_written == 0)
            {
                if (ferror(dest))
                {
                    perror("fwrite");
                    fclose(src);
                    fclose(dest);
                    return;
                }
            }

            total_written += bytes_written;
        }
    }

    if (ferror(src))
    {
        perror("fread");
        fclose(src);
        fclose(dest);
        return;
    }

    if (fseek(src, 0, SEEK_END) == 0)
    {
        long file_size = ftell(src);

        if (file_size >= 0)
        {
            printf("Source file size: %ld bytes\n", file_size);
        }
    }

    fclose(src);
    fclose(dest);

    printf("Standard library file copy completed.\n");
}

void redirect_output(void)
{
    int fd;

    printf("\n========================================\n");
    printf("PART 2A: OUTPUT REDIRECTION USING dup2()\n");
    printf("========================================\n");

    fd = open("output.txt",
              O_WRONLY | O_CREAT | O_TRUNC,
              0644);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    printf("Before redirection\n");

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        return;
    }

    close(fd);

    printf("Hello from redirected standard output!\n");
    printf("This message is stored in output.txt\n");
}

void redirect_input(void)
{
    int fd;
    char buffer[100];

    printf("\n========================================\n");
    printf("PART 2B: INPUT REDIRECTION USING dup2()\n");
    printf("========================================\n");

    fd = open("input.txt", O_RDONLY);

    if (fd == -1)
    {
        perror("open input.txt");
        return;
    }

    if (dup2(fd, STDIN_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        return;
    }

    close(fd);

    printf("Reading from redirected standard input:\n");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        printf("%s", buffer);
    }
}

int main(void)
{
    printf("========================================\n");
    printf("PRACTICAL 9: FILE I/O AND dup2()\n");
    printf("========================================\n");

    /*
       PART 1:
       File copy using low-level I/O
       and standard library I/O
    */

    low_level_copy("input.dat", "output_low.dat");

    stdio_copy("input.dat", "output_stdio.dat");

    /*
       PART 2:
       dup2() output and input redirection
    */

    redirect_output();

    /*
       stdout is now redirected to output.txt.
       Restore stdout to the terminal.
    */

    int terminal_fd = open("/dev/tty", O_WRONLY);

    if (terminal_fd != -1)
    {
        dup2(terminal_fd, STDOUT_FILENO);
        close(terminal_fd);
    }

    redirect_input();

    printf("\nPractical 9 completed.\n");

    return 0;
}
