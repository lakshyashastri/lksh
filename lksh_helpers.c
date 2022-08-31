#include <stdio.h>
#include <string.h>

#include "lksh_helpers.h"
#include "lksh.h"

// subtracts from left to right
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

int main() {
    char res[1000];

    char *s1 = "/Users/lakshyashastri/Desktop/iiith/assignments/sem-3/osn/2/lksh";
    char *s2 = "/Users/lakshyashastri/Desktop/iiith/assignments/sem-3/osn/2/lksh/includes";
    
    subtract_strings(s1, strlen(s1), s2, strlen(s2), res);
    printf("%s\n", res);
}
