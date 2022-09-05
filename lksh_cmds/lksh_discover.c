#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

// check if flag is valid
int flag_is_valid_discover(char *flag) {
    char *valid_flags[2] = {"-d", "-f"};
    for (int i = 0; i < 2; i++) {
        if (strcmp(flag, valid_flags[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void discover(char *path, char items[MAX_LENGTH]) {

}

void lksh_discover(char *splits[MAX_LENGTH], int split_count) {
    // simply discover
    if (split_count == 1) {
        return;
    }

    // handle params
    int flags[2] = {0}; // -d -f
    char *path = NULL, *file = NULL;
    for (int i = 1; i < split_count; i++) {
        
        // handle flags
        if (splits[i][0] == '-') {
            if (strcmp(splits[i], "-d") == 0) {
                flags[0] = 1;
            } else if (strcmp(splits[i], "-f") == 0) {
                flags[1] = 1;
            } else {
                printf("discover: invalid option -- %s\n", splits[i]);
                return;
            }

        } else if (splits[i][0] == '"') {
            file = malloc(sizeof(char) * (strlen(splits[i]) + 1));
            strcpy(file, splits[i]);

            // remove first and last characters from file
            file += 1;
            file[strlen(file)-1] = '\0';

        } else {
            path = malloc(sizeof(char) * (strlen(splits[i]) + 1));
            strcpy(path, splits[i]);

            if (strcmp(path, "~") == 0) {
                path = malloc(sizeof(char) * (strlen(ROOT) + 1));
                strcpy(path, ROOT);
            }
        }
    }

    // printf("%s\n", path == NULL ? "no path" : path);
    // printf("%s %s\n", flags[0] ? "-d" : "no d", flags[1] ? "-f" : "no f");
    // printf("%s\n", file == NULL ? "no file" : file);

}
