#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

void lksh_jobs(char *splits[MAX_LENGTH], int split_count) {
    bg_process *cur = bg_process_head;
    if (cur == NULL) {
        return;
    }

    int num_bg = 0;
    while (cur != NULL) {
        num_bg += 1;
        cur = cur -> next;
    }

    printf("%d\n", num_bg);
}
