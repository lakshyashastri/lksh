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
    int flags[4] = {0};

    int dirs_count = 0;
    int flag_count = 0;

    // get all dirs, files, and flag names from command input
    for (int i = 1; i < split_count; i++) {
        if (splits[i][0] != '-') {
            if (splits[i][0] == '~') {
                dirs[dirs_count++] = ROOT;
            } else {
                dirs[dirs_count++] = splits[i];
            }

        } else {
            if (flag_is_valid(splits[i])) {
                if (strcmp(splits[i], "-a") == 0) {
                    flags[0] = 1;
                } else if (strcmp(splits[i], "-l") == 0) {
                    flags[1] = 1;
                } else {
                    flags[2] = 1;
                }

            } else {
                printf("ls: invalid option -- %s\n", splits[i]);
                return;
            }
        }
    }

    // straight up "ls"
    if (!dirs_count) {
        dirs[dirs_count] = ".";
        dirs_count += 1;
    }


    for (int i = 0; i < dirs_count; i++) {
        DIR *dir = opendir(dirs[i]);
        if (dir == NULL) {
            printf("ls: %s: No such file or directory\n", splits[1]);
            
            // legibility
            if (i != dirs_count - 1) {
                printf("\n");
            }
            
            continue;
        }

        // only print dir name if there are more than one dirs
        if (dirs_count > 1) {
            printf("%s:\n", dirs[i]);
        }

        // stores names of all files in directory
        char *dir_files[MAX_LENGTH];
        int dir_files_counter = 0;

        // stores types of all items in a directory
        int item_type[MAX_LENGTH];

        // get all files in directory
        struct dirent *dir_data;
        while ((dir_data = readdir(dir)) != NULL) {
            dir_files[dir_files_counter++] = dir_data -> d_name;
        }

        // sort in alphabetical order
        char temp[MAX_LENGTH];
        for (int x = 0; x < dir_files_counter; x++) {
            for (int y = x + 1; y < dir_files_counter; y++) {
                if (strcasecmp(dir_files[x], dir_files[y]) > 0) {
                    strcpy(temp, dir_files[x]);
                    strcpy(dir_files[x], dir_files[y]);
                    strcpy(dir_files[y], temp);
                }
            }
        }

        // get item type info for all items in directory
        for (int j = 0; j < dir_files_counter; j++) {
            // get item type
            struct stat sb;
            if (stat(dir_files[j], &sb) == 0 && sb.st_mode & S_IFDIR) {
                item_type[j] = 0; // directory
            } else if (sb.st_mode & S_IXUSR) {
                item_type[j] = 1; // executable
            } else if (sb.st_mode & S_IFREG) {
                item_type[j] = 2; // file
            }
        }

        // output file names
        for (int j = 0; j < dir_files_counter; j++) {
            // ignore dot files; handle -a cases
            if (dir_files[j][0] == '.') {
                if (!flags[0] && !flags[2]) {
                    continue;
                }
            }

            // no -l flags
            if (!flags[1] && !flags[2]) {

                // print item and colour code
                if (item_type[j] == 0) {
                    printf("%s%s%s\n", COLOR_BLUE, dir_files[j], COLOR_RESET);
                } else if (item_type[j] == 1) {
                    printf("%s%s%s\n", COLOR_WHITE, dir_files[j], COLOR_RESET);
                } else if (item_type[j] == 2) {
                    printf("%s%s%s\n", COLOR_RED, dir_files[j], COLOR_RESET);
                }

            } else {
                printf("-l hai bhai, karta huon\n");
            }
        }
        
        if (i != dirs_count - 1) {
            printf("\n");
        }
    }
}
