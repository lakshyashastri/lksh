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

void discover(char *dir_path, char *items[MAX_LENGTH], int item_types[MAX_LENGTH], int *counter) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        return;
    }

    struct dirent *item;
    char relative[MAX_LENGTH];
    while ((item = readdir(dir)) != NULL) {
        // ignore hidden folders as well as pointers to the folder itself and the prev folder
        if ((item -> d_name)[0] == '.') {
            continue;
        }
        
        // add item to item array
        items[*counter] = malloc(sizeof(char) * (strlen(item -> d_name) + 1));
        strcpy(items[*counter], item -> d_name);

        strcat(relative, dir_path);
        strcat(relative, "/");
        strcat(relative, item -> d_name);

        printf("%s\n", relative);

        // store item type
        struct stat item_info;
        if (stat(relative, &item_info) == 0 && item_info.st_mode & S_IFDIR) {
            item_types[(*counter)++] = 0;

        } else if (item_info.st_mode & S_IFREG) {
            item_types[(*counter)++] = 1;

        } else {
            item_types[(*counter)++] = -1;
        }

        // printf("%s\n", relative);
        discover(relative, items, item_types, counter);
        memset(relative, 0, MAX_LENGTH);
    }
    
    closedir(dir);
}

void lksh_discover(char *splits[MAX_LENGTH], int split_count) {

    // handle params
    int flags[2] = {0}; // -d -f
    char *path = ".", *file = NULL;
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

        // handle files
        } else if (splits[i][0] == '"') {
            file = malloc(sizeof(char) * (strlen(splits[i]) + 1));
            strcpy(file, splits[i]);

            // remove first and last characters from file
            file += 1;
            file[strlen(file)-1] = '\0';

        // handle path
        } else {
            path = malloc(sizeof(char) * (strlen(splits[i]) + 1));
            strcpy(path, splits[i]);

            if (strcmp(path, "~") == 0) {
                path = malloc(sizeof(char) * (strlen(ROOT) + 1));
                strcpy(path, ROOT);
            }
        }
    }

    // initialise vars
    char *items[MAX_LENGTH];
    int item_types[MAX_LENGTH]; // dir = 0; file = 1
    int item_type_counter = 0;

    // fill in .
    items[item_type_counter] = malloc(sizeof(char) * (strlen(".") + 1));
    item_types[item_type_counter] = 0;
    strcpy(items[item_type_counter++], ".");

    discover(path, items, item_types, &item_type_counter);

    for (int i = 0; i < item_type_counter; i++) {
        printf("%s: %d\n", items[i], item_types[i]);
    }

    // simply discover or discover -d -f
    // if (split_count == 1 || (flags[0] && flags[1])) {
    //     return;
    // }

    // printf("%s\n", path == NULL ? "no path" : path);
    // printf("%s %s\n", flags[0] ? "-d" : "no d", flags[1] ? "-f" : "no f");
    // printf("%s\n", file == NULL ? "no file" : file);

}
