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

        int found_index = -1;
        for (int i = 0; i < num_bg; i++) {
            if (bg_ids[i] == pid) {
                found_index = i;
            }
        }

        if (found_index >= 0) {
            fprintf(stderr, "\n%s with pid = %d exited %s\n", bg_names[found_index], pid, WIFEXITED(status) ? "normally" : "abnormally");
            printf("%s<%s%s@%s%s:%s%s%s>%s ", COLOR_GREEN, username -> pw_name, COLOR_RED, COLOR_CYAN, hostname, COLOR_PURPLE, CWD, TIME_TAKEN_STRING, COLOR_RESET);
            fflush(stdout);
            fflush(stderr);
        }
    }
}

void ctrl_c_handler() {
    signal(SIGINT, SIG_DFL);
    fflush(stdout);
}

void ctrl_z_handler() {
    if (foreground != -1) {
        bg_ids[num_bg] = foreground;
        bg_names[num_bg] = malloc(sizeof(char) * (strlen(foreground_cmd_name) + 1));
        strcpy(bg_names[num_bg++], foreground_cmd_name);
    }
}
