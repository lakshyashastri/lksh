#include "lksh.h"

#include "includes/libs.h"
#include "includes/consts.h"

#include "lksh_cmds/lksh_pwd.c"
#include "lksh_cmds/lksh_echo.c"
#include "lksh_cmds/lksh_cd.c"
#include "lksh_cmds/lksh_ls.c"

// initial working directory ie root
char ROOT[MAX_LENGTH];

// variable to keep track of current working directory (relative from ~)
char CWD[MAX_LENGTH] = "~";

// variable to keep track of previous working directory (absolute from actual root)
char PREV_WD[MAX_LENGTH] = "~";

int main() {
    // clear terminal screen at initialization
    cls();

    // list of valid commands
    int NUM_CMDS = 4;
    char *CMDS[4] = {"pwd", "echo", "cd", "ls"};

    // get username
    struct passwd *username;
    username = getpwuid(getuid());
    
    // get system name
    char hostname[MAX_LENGTH];
    hostname[MAX_LENGTH - 1] = '\0';
    gethostname(hostname, MAX_LENGTH);

    // get initial working directory ie root
    ROOT[MAX_LENGTH - 1] = '\0';
    getcwd(ROOT, MAX_LENGTH);

    while (1) {
        // command line input
        char *input;
        size_t MAX_INPUT_LENGTH = MAX_LENGTH;
        size_t input_length;

        // allocate memory to command line input
        input = malloc(MAX_INPUT_LENGTH * sizeof(char));
        if (input == NULL) {
            printf("Out of memory\n");
            break;
        }

        // shell prompt
        printf("%s<%s%s@%s%s:%s%s>%s ", COLOR_GREEN, username -> pw_name, COLOR_RED, COLOR_CYAN, hostname, COLOR_PURPLE, CWD, COLOR_RESET);
        input_length = getline(&input, &MAX_INPUT_LENGTH, stdin);

        // parse ;
        char *semi = ";";
        char *cmds;
        cmds = strtok(input, semi);

        // convert input to input.split(';')
        char *input_splitted[MAX_LENGTH];
        int input_split_count = 0;
        while (cmds != NULL) {
            input_splitted[input_split_count++] = cmds;
            cmds = strtok(NULL, semi);
        }

        for (int ii = 0; ii < input_split_count; ii++) {

            // parse input for spaces and tabs
            char *sep = " \t";
            char *token;
            token = strtok(input_splitted[ii], sep);

            // convert tokenized string to array
            char *splits[MAX_LENGTH];
            int split_count = 0;
            while (token != NULL) {
                splits[split_count++] = token;
                token = strtok(NULL, sep);
            }

            // remove trailing "\n" from last element in splits array
            strtok(splits[split_count - 1], "\n");

            // empty enter
            if (strcmp(splits[0], "\n") == 0) {
                continue;
            }

            // check if command is valid
            int valid = 0;
            for (int i = 0; i < NUM_CMDS; i++) {
                if (strcmp(splits[0], CMDS[i]) == 0) {
                    valid = 1;
                    break;
                }
            }
            
            // if not valid
            if (!valid) {
                printf("%s is not a valid command\n", splits[0]);
                continue;
            }

            // execute commands
            if (strcmp(splits[0], "pwd") == 0) {
                lksh_pwd();
            } else if (strcmp(splits[0], "echo") == 0) {
                lksh_echo(splits, split_count);
            } else if (strcmp(splits[0], "cd") == 0) {
                lksh_cd(splits, split_count);
            } else if (strcmp(splits[0], "ls") == 0) {
                lksh_ls(splits, split_count);
            }
        }
    }
}
