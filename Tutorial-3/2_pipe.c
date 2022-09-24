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

    char input[102];
    scanf("%s", input);
    printf("'%s' will be written to fildes[1]\n", input);

    // write to the write end of the pipe
    write(fildes[1], input, strlen(input) + 1);
    close(fildes[1]);

    // read from the other end of the pipe
    char pipe_content[102];
    read(fildes[0], pipe_content, 100);
    
    printf("Contents of fildes[0]: '%s'\n", pipe_content);
    close(fildes[0]);

    return 0;
}
