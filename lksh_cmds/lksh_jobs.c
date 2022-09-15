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
    // flag handling
    int flags[2] = {0}; // -r -s
    if (split_count > 1) {
        for (int i = 1; i < split_count; i++) {
            if (strcmp(splits[i], "-r")) {
                flags[0] = 1;
            }
            if (strcmp(splits[i], "-s")) {
                flags[1] = 1;
            }
        }
    }

    // return if no jobs
    bg_process *cur = bg_process_head;
    if (cur == NULL) {
        return;
    }

    // get number of jobs in array
    bg_process *procs[MAX_LENGTH];
    int num_bg = 0;
    while (cur != NULL) {
        procs[num_bg] = cur;
        num_bg += 1;
        cur -> job_number = num_bg;

        cur = cur -> next;
    }
    
    // sort jobs by process name
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
        sprintf(stat_file_path, "/proc/%d/stat", procs[i] -> id); 
        FILE *stat_file = fopen(stat_file_path, "r");
        char trash[MAX_LENGTH];
        fscanf(stat_file, "%s %s %c", trash, trash, &states[i]);
    }

    // print
    for (int i = 0; i < num_bg; i++) {
        if ((!flags[0] && !flags[1]) || (flags[0] && flags[1])) {
            printf("[%d] %s %s [%d]\n", procs[i] -> job_number, states[i] == 'R' ? "Running" : "Stopped", procs[i] -> process_name, procs[i] -> id);

        } else if (flags[0]) {
            if (states[i] == 'R') {
                printf("[%d] %s %s [%d]\n", procs[i] -> job_number, states[i], procs[i] -> process_name, procs[i] -> id);
            }

        } else if (flags[1]) {
            if (states[i] == 'S') {
                printf("[%d] %s %s [%d]\n", procs[i] -> job_number, states[i], procs[i] -> process_name, procs[i] -> id);
            }
        }
    }
}
