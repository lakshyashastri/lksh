#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

void lksh_sig(char *splits[MAX_LENGTH], int split_count) {
    if (split_count < 3) {
        printf("Missing arguments\n");
        return;
    }

    int job_number = atoi(splits[1]);
    int sig_number = atoi(splits[2]);

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
        perror("Job number does not exist");
        return;
    }

    if (sig_number >= 1 && sig_number <= 31) {
        kill(cur -> id, sig_number);
    }
}
