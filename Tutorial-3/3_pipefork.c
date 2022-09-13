#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    int fildes[2];

    // create a pipe
    if (pipe(fildes) < 0) {
        perror("Could not create pipe.");
        exit(1);
    }

    int pid = fork();

    if (pid < 0) {
        // in parent, fork failed
        perror("Could not create child.");
        exit(1);

    } else if (pid == 0) {
        // in child, will be reading from pipe

        // close unused fds
        close(fildes[1]);

        printf("%d (child): Waiting for content to be written.\n", getpid());

        // read from pipe
        char pipe_output[102];
        read(fildes[0], pipe_output, 100); // blocked until content is written
        
        printf("%d (child): Pipe contents: '%s'\n", getpid(), pipe_output);
        close(fildes[0]);

    } else {
        // in parent, will be writing into the pipe

        // close unused fds
        close(fildes[0]);

        sleep(2);  // to show that the child will wait

        printf("%d (parent): What should be written to the pipe? ", getpid());
        char input[102];
        scanf("%s", input);

        // write to pipe
        write(fildes[1], input, strlen(input) + 1);
        close(fildes[1]);

        sleep(1);  // give child process time to execute
    }
    printf("Process %d will quit\n", getpid());

    return 0;
}
