#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s filename\n", argv[0]);
        exit(1);
    }

    // open the file to replace stdout
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0)
    {
        perror("Failed to open file.");
        exit(1);
    }

    printf("You should see this on your screen.\n");

    int original;
    dup2(STDOUT_FILENO, original);

    // int newfd, oldfd;
    // newfd = dup(oldfd);

    // dup2(oldfd, newfd);

    // duplicate the open file into stdout, closing stdout atomically
    if (dup2(fd, STDOUT_FILENO) < 0)
    {
        perror("Unable to duplicate file descriptor.");
        exit(1);
    }

    // whether or not the original fd is closed, the duplicated fd remains accessible
    close(fd);
    printf("This will not end up on your screen.\n");

    dup2(original, STDOUT_FILENO);
    printf("You should see this on your screen.\n");

    return 0;
}
