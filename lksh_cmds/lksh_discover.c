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

    // check if path is given and also check for flags
    char *path = NULL;
    int flags[2] = {0}; // -d -f
    if (splits[1][0] != '-') {
        path = malloc(sizeof(char) * (strlen(splits[1]) + 1));
        strcpy(path, splits[1]);

    } else {
        if (strcmp(splits[1], "-d") == 0) {
            flags[0] = 1;
        } else if (strcmp(splits[1], "-f") == 0) {
            flags[1] = 1;
        }

        if (split_count >= 3) {
            if (strcmp(splits[2], "-d") == 0) {
                flags[0] = 1;
            } else if (strcmp(splits[2], "-f") == 0) {
                flags[1] = 1;
            }
        }
    }

    printf("%s\n", path == NULL ? "no path" : path);
    printf("%s %s\n", flags[0] ? "-d" : "no d", flags[1] ? "-f" : "no f");
    
    // // check for flags
    // int flags[2] = {0}; // -d -f
    // for (int i = 0; i < split_count; i++) {
    //     if (splits[i][0] == '-') {
    //         if (flag_is_valid_discover(splits[i])) {
    //             if (strcmp(splits[i], "-d") == 0) {
    //                 flags[0] = 1;
    //             } else {
    //                 flags[1] = 1;
    //             }
    //         } else {
    //             printf("discover: invalid option -- %s\n", splits[i]);
    //         }
    //     }
    // }


}
