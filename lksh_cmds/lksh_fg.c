#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

void lksh_fg(char *splits[MAX_LENGTH], int split_count) {
    if (split_count < 2) {
        printf("Missing arguments\n");
        return;
    }

    int job_number = atoi(splits[1]);

    int exists = 0;
    bg_process *cur = bg_process_head;
    while (cur != NULL) {
        if (cur -> job_number == job_number) {
            exists = 1;
            break;
        }
        cur = cur -> next;
    }

    if (!exists) {
        printf("Job number does not exist");
        return;
    }

    kill(cur -> id, SIGCONT);
    waitpid(cur -> id, NULL, WUNTRACED);
}
