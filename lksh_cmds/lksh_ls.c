#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

int flag_is_valid(char *flag) {
    char *valid_flags[4] = {"-a", "-l", "-al", "-la"};
    for (int i = 0; i < 4; i++) {
        if (strcmp(flag, valid_flags[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void lksh_ls(char *splits[MAX_LENGTH], int split_count) {
    char *dirs[MAX_LENGTH]; // directory names or files
    char *flags[4];

    int dirs_count = 0;
    int flag_count = 0;
    for (int i = 1; i < split_count; i++) {
        if (splits[i][0] != '-') {
            if (splits[i][0] == '~') {
                dirs[dirs_count++] = ROOT;
            } else {
                dirs[dirs_count++] = splits[i];
            }
        } else {
            if (flag_is_valid(splits[i])) {
                flags[flag_count++] = splits[i];
            } else {
                printf("ls: invalid option -- %s\n", splits[i]);
                return;
            }
        }
    }

    // if (!dirs_count && !flag_count) {

    // }


    for (int i = 0; i < dirs_count; i++) {
        DIR *dir = opendir(dirs[i]);
        if (dir == NULL) {
            printf("ls: %s: No such file or directory\n", splits[1]);
        }

        struct dirent *dir_data;
        while ((dir_data = readdir(dir)) != NULL) {
            printf("%s\n", dir_data -> d_name);
        }
        printf("\n");
    }
}
