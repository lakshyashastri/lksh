#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

// called when child dies before parent kills it
// REMOVE BG PROCESS
void child_handler() {
    int status;
    int pid = waitpid(-1, &status, WNOHANG);

    if (pid > 0) {
        if (WIFSTOPPED(status)) {
            return;
        }

        int found = 0;
        struct bg_process *cur = bg_process_head;
        while (cur != NULL) {
            if (cur -> id == pid) {
                found = 1;
                break;
            }
            cur = cur -> next;
        }

        if (found) {
            fprintf(stderr, "\n%s with pid = %d exited %s\n", cur -> process_name, pid, WIFEXITED(status) ? "normally" : "abnormally");
            printf("%s<%s%s@%s%s:%s%s%s>%s ", COLOR_GREEN, username -> pw_name, COLOR_RED, COLOR_CYAN, hostname, COLOR_PURPLE, CWD, TIME_TAKEN_STRING, COLOR_RESET);
            fflush(stdout);
            fflush(stderr);

            // free bg process node: turn to func which takes pid and returns -1 if node not found
            // last node
            if (cur -> next == NULL) {
                // head node
                if (cur -> prev == NULL) {
                    bg_process_head = NULL;
                } else {
                    cur -> prev -> next = NULL;
                }

            // middle node
            } else {
                cur -> next -> prev = cur -> prev;
                cur -> prev -> next = cur -> next;
            }
            free(cur);
        }
    }
}

void ctrl_c_handler() {
    signal(SIGINT, SIG_DFL);
    printf("\n");
    fflush(stdout);
}

void ctrl_z_handler() {
    if (foreground != -1) {
        bg_process *node = add_process_node(foreground, foreground_cmd_name);
    }
}
