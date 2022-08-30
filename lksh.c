#include "includes.h"
#include "consts.h"

int main() {
    // clear terminal screen at initialization
    cls();

    // list of valid commands
    char *COMMANDS = {"pwd"};

    // get OS name
    struct utsname unameInfo;
    uname(&unameInfo);
    
    // get system name
    char hostname[MAX_LENGTH];
    hostname[MAX_LENGTH - 1] = '\0';
    gethostname(hostname, MAX_LENGTH);

    // get current working directory
    char init_cwd[MAX_LENGTH];
    init_cwd[MAX_LENGTH - 1] = '\0';
    getcwd(init_cwd, MAX_LENGTH);
    
    // var to keep track of current working directory
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

        // check for command
        if (split_count == 0) {
            continue;
        }

        
        

    }
}
