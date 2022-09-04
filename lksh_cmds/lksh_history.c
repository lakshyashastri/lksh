#include "../includes/libs.h"
#include "../includes/consts.h"

// assumes that history.txt will always exist

#define HIST "lksh_cmds/history.txt"
#define BUFFER "lksh_cmds/hist_buffer.txt"

void lksh_history(char *splits[MAX_LENGTH], int split_count) {
    FILE *src = fopen(HIST, "r");
    
    int printed = 0;
    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), src)) {
        printf("%s", line);
        
        printed += 1;
        if (printed == 10) {
            break;
        }
    }

    fclose(src);
}

void lksh_history_write(char *input) {

    // open files
    FILE *src = fopen(HIST, "r");
    FILE *new = fopen(BUFFER, "w+");

    // check most recent addition
    char line[MAX_LENGTH];
    fgets(line, sizeof(line), src);
    if (strcmp(line, input) == 0) {
        remove(BUFFER);
        fclose(src);
        fclose(new);
        return;
    }
    fseek(src, 0, SEEK_SET);

    // add most recent to new_file
    fprintf(new, "%s", input);

    // read current history file and write to new file
    int lines_copied = 0;
    while (fgets(line, sizeof(line), src)) {
        fprintf(new, "%s", line);
        lines_copied += 1;

        if (lines_copied == 19) {
            break;
        }
    }

    fclose(src);
    fclose(new);

    remove(HIST);
    rename(BUFFER, HIST);
}
