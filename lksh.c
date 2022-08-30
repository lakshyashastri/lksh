#include "lksh.h"

#include "includes/libs.h"
#include "includes/consts.h"

#include "lksh_cmds/lksh_pwd.c"

int main() {
    // clear terminal screen at initialization
    cls();

    // list of valid commands
    char *CMDS = {"pwd"};
    int num_commands = sizeof(CMDS[0]) / sizeof(char);

    // get OS name
    struct utsname unameInfo;
    uname(&unameInfo);
    
    // get system name
    char hostname[MAX_LENGTH];
    hostname[MAX_LENGTH - 1] = '\0';
    gethostname(hostname, MAX_LENGTH);

    // get initial working directory ie root
    char ROOT[MAX_LENGTH];
    ROOT[MAX_LENGTH - 1] = '\0';
    getcwd(ROOT, MAX_LENGTH);

    // variable to keep tra"ck of relative working directory
    char CWD[MAX_LENGTH] = "~";

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
        printf("<%s@%s:%s> ", hostname, unameInfo.sysname, CWD);
        input_length = getline(&input, &MAX_INPUT_LENGTH, stdin);

        // parse input
        char *sep = " \t";
        char *token;
        token = strtok(input, sep);

        // convert tokenized string to array
        char *splits[MAX_LENGTH];
        int split_count = 0;
        while (token != NULL) {
            splits[split_count++] = token;
            token = strtok(NULL, sep);
        }

        // remove trailing "\n" from last element in splits array
        strtok(splits[split_count - 1], "\n");

        // check if command is valid
        if (split_count == 0) {
            continue;
        }
        int valid = 0;
        for (int i = 0; i < split_count; i++) {
            if (strcmp(splits[0], &CMDS[i]) == 0) {
                valid = 1;
                break;
            }
        }
        
        // if not valid
        if (!valid) {
            printf("%s is not a valid command\n", splits[0]);
            continue;
        }

        // check commands
        if (strcmp(splits[0], "pwd") == 0) {
            lksh_pwd();
        }
    }
}
