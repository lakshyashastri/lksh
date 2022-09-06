#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *and_sepped[1024];
    int and_sep_count = 0;
    char *sep = "&";
    char *token;
    char input[] = "sleep 5& sleep 3 2 &sleep 1&sleep 3";
    
    token = strtok(input, sep);

    while (token != NULL) {
        and_sepped[and_sep_count++] = token;
        token = strtok(NULL, sep);
    }

    for (int i = 0; i < and_sep_count; i++) {
        if (and_sepped[i][0] == ' ' || and_sepped[i][0] == '\n') {
            memmove(and_sepped[i], and_sepped[i] + 1, strlen(and_sepped[i]));
        }
        if (and_sepped[i][strlen(and_sepped[i]) - 1] == ' ' || and_sepped[i][strlen(and_sepped[i]) - 1] == '\n') {
            and_sepped[i][strlen(and_sepped[i]) - 1] = '\0';
        }
    }

    // for (int i = 0; i < and_sep_count; i++) {
    //     printf("'%s'\n", and_sepped[i]);
    // }

    char *tok;
    tok = strtok(and_sepped[and_sep_count - 1], " ");
    char *args_arr[1024];
    int args_c = 0;
    while (tok != NULL) {
        args_arr[args_c++] = tok;
        tok = strtok(NULL, " ");
    }

    for (int i = 0; i < args_c; i++) {
        printf("'%s' ", args_arr[i]);
    }

    return 0;
}
