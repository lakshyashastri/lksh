#include "../includes/libs.h"
#include "../includes/consts.h"

// assumes that history.txt will always exist

#define HIST "lksh_cmds/history.txt"
#define STOP "lksh_cmds/hist_replace.txt" // contains line num to replace
#define BUFFER "lksh_cmds/hist_buffer.txt"

void lksh_history(char *splits[MAX_LENGTH], int split_count) {

}

void lksh_history_write(char *input) {
    // get line number to write to
    FILE *stop_line_file = fopen(STOP, "r");
    int replace_line_num;
    fscanf(stop_line_file, "%d", &replace_line_num);
    fclose(stop_line_file);
    
    // get file stats
    struct stat sb;
    stat(HIST, &sb);

    // open files
    FILE *src = fopen(HIST, "r");
    FILE *new = fopen(BUFFER, "w+");
    
    // copy paste from old to new file to replace a single line
    char line[MAX_LENGTH];
    int line_num = 1;
    while (fgets(line, sizeof(line), src)) {
        if (line_num != replace_line_num) {
            fprintf(new, "%s", line);
        } else {
            fprintf(new, "%s", input); // line replaced
        }
        line_num += 1;
    }

    // for the first time history.txt is filled
    if (replace_line_num == -1) {
        fprintf(new, "%s", input);
    }

    // reset history pointer
    if (line_num == 20) {
        stop_line_file = fopen(STOP, "w");
        fprintf(stop_line_file, "1");
        fclose(stop_line_file);
    };

    // increment replace line
    if (replace_line_num != -1) {
        stop_line_file = fopen(STOP, "w");
        if (replace_line_num == 20) {
            fprintf(stop_line_file, "1");
        } else {
            fprintf(stop_line_file, "%d", replace_line_num + 1);
        }
        fclose(stop_line_file);
    }

    fclose(src);
    fclose(new);

    remove(HIST);
    rename(BUFFER, HIST);
}
