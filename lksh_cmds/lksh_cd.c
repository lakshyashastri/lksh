#include "../lksh.h"

#include "../includes/libs.h"
#include "../includes/consts.h"

char *get_pwd_cd() {
    char *path = malloc(sizeof(char) * MAX_LENGTH);
    path[MAX_LENGTH - 1] = '\0';
    getcwd(path, MAX_LENGTH);

    return path;
}

void subtract_strings(char *minuend, int minuend_len, char *subtrahend, int subtrahend_len, char *res) {
    int iter = 0;

    while (minuend[iter] == subtrahend[iter]) {
        iter++;
    }

    int res_len;
    if (subtrahend_len > minuend_len) {
        res_len = subtrahend_len - iter;
        for (int i = 0; i < res_len; i++) {
            res[i] = subtrahend[iter + i];
        }
    } else {
        res_len = minuend_len - iter;
        for (int i = 0; i < res_len; i++) {
            res[i] = minuend[iter + i];
        }
    }
    res[res_len] = '\0';
}

// considers the first argument and ignores the rest if more than one provided
void lksh_cd(char *splits[MAX_LENGTH], int split_count) {
    char *dir;

    // decide directory to cd into
    if (split_count == 2) {
        dir = splits[1];
    } else if (split_count == 1) {
        dir = ROOT; // there is no argument passed to cd so it cds to root
    } else {
        dir = splits[1]; // more than 1 arg so takes the first one
    }

    // handle ~, ~/stuff and -
    if (strcmp(dir, "~") == 0) {
        strcpy(dir, ROOT);
    } else if (dir[0] == '~') {
        char temp[MAX_LENGTH];
        strcpy(temp, ROOT);
        strcat(temp, dir + 1); // eliminate ~
        strcpy(dir, temp);
    } else if (strcmp(dir, "-") == 0) {
        if (strcmp(PREV_WD, "~") == 0) {
            strcpy(dir, ROOT);
        } else {
            strcpy(dir, PREV_WD);
        }
    }
    
    // store current (before change) dir to previous dir variable
    char *current_dir = get_pwd_cd();
    strcpy(PREV_WD, current_dir);

    // change dir and handle error
    int res = chdir(dir);
    if (res != 0) {
        printf("cd: no such file or directory: %s\n", dir);
    }

    // get difference in paths of root and current dir (after change) from actual root
    current_dir = get_pwd_cd();
    char diff[MAX_LENGTH];
    subtract_strings(current_dir, strlen(current_dir), ROOT, strlen(ROOT), diff);

    // update CWD
    if (strcmp(current_dir, ROOT) == 0) {
        strcpy(CWD, "~");
    } else {
        if (!strstr(current_dir, ROOT)) {
            strcpy(CWD, current_dir);
        } else {
            strcpy(CWD, diff);
        }
    }
}
