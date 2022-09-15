#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

int cmp_func(const void *v1, const void *v2) {
    const bg_process *p1 = v1;
    const bg_process *p2 = v2;

    if (strcmp(p1 -> process_name, p2 -> process_name) < 0) {
        return -1;
    } else if (strcmp(p1 -> process_name, p2 -> process_name) > 0) {
        return 1;
    } else {
        return 0;
    }
}

void lksh_jobs(char *splits[MAX_LENGTH], int split_count) {
    bg_process *cur = bg_process_head;
    if (cur == NULL) {
        return;
    }

    bg_process *procs[MAX_LENGTH];
    int num_bg = 0;
    while (cur != NULL) {
        procs[num_bg] = cur;
        num_bg += 1;
        cur -> job_number = num_bg;

        cur = cur -> next;
    }
    
    struct bg_process *temp;
    for (int i = 0; i < num_bg; i++) {
        for (int j = i + 1; j < num_bg; j++) {
            if (strcmp(procs[i] -> process_name, procs[j] -> process_name) > 0) {
                temp = procs[i];
                procs[i] = procs[j];
                procs[j] = temp;
            }
        }
    }
    
    // get process state
    char stat_file_path[MAX_LENGTH];
    char states[MAX_LENGTH];
    for (int i = 0; i < num_bg; i++) {
        sprintf(stat_file_path, "/proc/%d", procs[i] -> id); 
        FILE *stat_file = fopen(stat_file_path, "r");
        char trash[MAX_LENGTH];
        fscanf(stat_file, "%s %s %c", trash, trash, &states[i]);
    }

    for (int i = 0; i < num_bg; i++) {
        printf("[%d] %s %s [%d]\n", procs[i] -> job_number, states[i] == 'R' ? "Running" : "Stopped", procs[i] -> process_name, procs[i] -> id);
    }
}
