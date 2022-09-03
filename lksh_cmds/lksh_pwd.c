#include "../includes/libs.h"
#include "../includes/consts.h"

char *get_pwd() {
    char *path = malloc(sizeof(char) * MAX_LENGTH);
    path[MAX_LENGTH - 1] = '\0';
    getcwd(path, MAX_LENGTH);

    return path;
}

void lksh_pwd() {
    printf("%s\n", get_pwd());
}
