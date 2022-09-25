#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

// check if flag is valid
int flag_is_valid(char *flag) {
    char *valid_flags[4] = {"-a", "-l", "-al", "-la"};
    for (int i = 0; i < 4; i++) {
        if (strcmp(flag, valid_flags[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// construct the permission string
void create_perm_string(struct stat file_info, char ret[11]) {
    // directory
    ret[0] = (file_info.st_mode & S_IFDIR) ? 'd' : '-';

    // user perms
    ret[1] = (file_info.st_mode & S_IRUSR) ? 'r' : '-';
    ret[2] = (file_info.st_mode & S_IWUSR) ? 'w' : '-';
    ret[3] = (file_info.st_mode & S_IXUSR) ? 'x' : '-';

    // group perms
    ret[4] = (file_info.st_mode & S_IRGRP) ? 'r' : '-';
    ret[5] = (file_info.st_mode & S_IWGRP) ? 'w' : '-';
    ret[6] = (file_info.st_mode & S_IXGRP) ? 'x' : '-';

    // other perms
    ret[7] = (file_info.st_mode & S_IROTH) ? 'r' : '-';
    ret[8] = (file_info.st_mode & S_IWOTH) ? 'w' : '-';
    ret[9] = (file_info.st_mode & S_IXOTH) ? 'x' : '-';

    ret[10] = '\0';
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

    int file_sep = 0;
    for (int i = 0; i < dirs_count; i++) {
        DIR *dir = opendir(dirs[i]);
        if (dir == NULL) {
            
            // if its a file
            if (access(dirs[i], F_OK) == 0) {
                printf("%s\n", dirs[i]);
                file_sep = 1;
                continue;
            }

            // dirs[i] is not a file or a directory
            printf("ls: %s: No such file or directory\n", dirs[i]);
            
            // legibility
            if (i != dirs_count - 1) {
                printf("\n");
            }
            
            continue;
        }

        // legibility
        if (file_sep && i != dirs_count - 1) {
            printf("\n");
        }

        // only print dir name if there are more than one dirs
        if (dirs_count > 1) {
            printf("%s:\n", dirs[i]);
        }

        // stores relative names of all files in directory
        char *dir_files[MAX_LENGTH];
        int dir_files_counter = 0;

        // stores names of all files in directory
        char *dir_files_names[MAX_LENGTH];

        // stores types of all items in a directory
        int item_type[MAX_LENGTH];

        // get all files in directory
        struct dirent *dir_data;
        while ((dir_data = readdir(dir)) != NULL) {
            char prev_temp[MAX_LENGTH];
            sprintf(prev_temp, "%s/%s", dirs[i], dir_data -> d_name);

            dir_files[dir_files_counter] = malloc(sizeof(char) * MAX_LENGTH * 10);
            strcpy(dir_files[dir_files_counter], prev_temp);
            prev_temp[0] = '\0';

            dir_files_names[dir_files_counter++] = dir_data -> d_name;
        }

        // sort in alphabetical order
        char alpha_temp[MAX_LENGTH];
        for (int x = 0; x < dir_files_counter; x++) {
            for (int y = x + 1; y < dir_files_counter; y++) {
                if (strcmp(dir_files[x], dir_files[y]) > 0) { // strcasecmp for actual sort
                    strcpy(alpha_temp, dir_files[x]);
                    strcpy(dir_files[x], dir_files[y]);
                    strcpy(dir_files[y], alpha_temp);
                }

                if (strcmp(dir_files_names[x], dir_files_names[y]) > 0) {
                    strcpy(alpha_temp, dir_files_names[x]);
                    strcpy(dir_files_names[x], dir_files_names[y]);
                    strcpy(dir_files_names[y], alpha_temp);
                }
                
            }
        }

        // block sizes for -l
        int block_size = 0;
        int hidden_block_size = 0;

        // get item type info for all items in directory
        int max_file_size = 0;
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

            // if "-l" or "-al" is there
            if (flags[1] || flags[2]) {
                
                // add block size
                hidden_block_size += sb.st_blocks;
                if (dir_files_names[j][0] != '.') {
                    block_size += sb.st_blocks;
                }

                // space for file size in ls -l
                if (sb.st_size > max_file_size) {
                    max_file_size = sb.st_size;
                }
            }
        }

        // default int file size width
        int file_size_width = 5;

        // if "-l" or "-al" is there
        if (flags[1] || flags[2]) {

            // total block size
            printf("total ");
            if (flags[1]) {
                printf("%d", block_size);
            } else if (flags[2]) {
                printf("%d", hidden_block_size);
            }
            printf("\n");

            // space for file size in ls -l
            file_size_width = 7;
        }

        // output file names
        for (int j = 0; j < dir_files_counter; j++) {

            // ignore dot files; handle -a cases
            if (dir_files_names[j][0] == '.') {
                if (!flags[0] && !flags[2]) {
                    continue;
                }
            }

            // no -l flags
            if (!flags[1] && !flags[2]) {
                
                // print item and colour code
                if (item_type[j] == 0) {
                    printf("%s%s%s\n", COLOR_BLUE, dir_files_names[j], COLOR_RESET);
                } else if (item_type[j] == 1) {
                    printf("%s%s%s\n", COLOR_WHITE, dir_files_names[j], COLOR_RESET);
                } else if (item_type[j] == 2) {
                    printf("%s%s%s\n", COLOR_RED, dir_files_names[j], COLOR_RESET);
                }

            } else {
                // get file info
                struct stat file_info;
                stat(dir_files[j], &file_info);

                // get perm string
                char perm_string[11];
                create_perm_string(file_info, perm_string);

                // hard links
                int hard_links = file_info.st_nlink;
                
                // owner
                struct passwd *user_id = getpwuid(file_info.st_uid);
                
                // group owner
                struct group *gr = getgrgid(file_info.st_gid);

                // content size
                int size = file_info.st_size;

                // time modified
                char time[MAX_LENGTH];
                strftime(time, sizeof(time), "%b %e %H:%M", localtime(&file_info.st_mtime));

                // print
                printf("%s  %3d %s  %s  %*d %s %s\n",
                    perm_string, hard_links, user_id -> pw_name,
                    gr -> gr_name, file_size_width, size, time, dir_files[j]
                    );
            }
        }
        
        if (i != dirs_count - 1) {
            printf("\n");
        }
    }
}
