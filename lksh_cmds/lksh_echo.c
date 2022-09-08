#include "../includes/libs.h"
#include "../includes/consts.h"

void lksh_echo(char *splits[MAX_LENGTH], int split_count) {
    for (int i = 1; i < split_count; i++) {
        printf("%s ", splits[i]);
    }
    printf("\n");
}
