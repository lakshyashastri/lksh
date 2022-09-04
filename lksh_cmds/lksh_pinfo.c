#include "../includes/libs.h"
#include "../includes/consts.h"

void lksh_pinfo(char *splits[MAX_LENGTH], int split_count) {
    int pid;
    
    // get pid
    if (split_count == 1) {
        pid = getpid();
    } else {
        pid = atoi(splits[1]);
    }
    
    printf("pid: %d\n", pid);

    // create path to stat file for current pid
    char stat_file_path[MAX_LENGTH];
    sprintf(stat_file_path, "/proc/%d/stat", pid);
    
    // get process state
    FILE *stat_file = fopen(stat_file_path, "r");
    char state;
    fscanf(stat_file, "%*s %*[^)]) %c", &state);

    // background foreground
    fseek(stat_file, 0, SEEK_SET);
    char trash[MAX_LENGTH], five[MAX_LENGTH], eight[MAX_LENGTH];
    fscanf(stat_file, "%s %s %s %s %s %s %s %s",
    trash, trash, trash, trash, five, trash, trash, eight);

    char plus = ' ';
    if (atoi(five) == atoi(eight)) {
        char plus = '+';
    }
    printf("process status: %c%c\n", state, plus);

    // create path to statm file for current pid
    char statm_file_path[MAX_LENGTH];
    sprintf(statm_file_path, "/proc/%d/statm", pid);

    // get memory
    FILE *statm_file = fopen(statm_file_path, "r");
    int memory;
    fscanf(statm_file, "%d", &memory);
    printf("memory: %d\n", memory);

    // exec path
    char exe_file_path[MAX_LENGTH];
    sprintf(exe_file_path, "/proc/%d/exe", pid);
    char exec_path[MAX_LENGTH] = {0};
    readlink(exe_file_path, exec_path, sizeof(exec_path));
    printf("executable path: %s\n", exec_path);

    fclose(stat_file);
    fclose(statm_file);
}
