#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

void lksh_pwd() {
    char path[MAX_LENGTH];
    path[MAX_LENGTH - 1] = '\0';
    getcwd(path, MAX_LENGTH);

    printf("%s\n", path);
}
