#include "lksh_helpers.h"
#include "lksh.h"

#include "includes/libs.h"
#include "includes/consts.h"

// subtracts from left to right
// void subtract_strings(char *minuend, int minuend_len, char *subtrahend, int subtrahend_len, char *res) {
//     int iter = 0;

//     while (minuend[iter] == subtrahend[iter]) {
//         iter++;
//     }

//     int res_len;
//     if (subtrahend_len > minuend_len) {
//         res_len = subtrahend_len - iter;
//         for (int i = 0; i < res_len; i++) {
//             res[i] = subtrahend[iter + i];
//         }
//     } else {
//         res_len = minuend_len - iter;
//         for (int i = 0; i < res_len; i++) {
//             res[i] = minuend[iter + i];
//         }
//     }
//     res[res_len] = '\0';
// }

double current_timestamp() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec) * 1000 + (t.tv_usec) / 1000;
}

// int main() {
//     char res[1000];

//     char *s1 = "/Users/lakshyashastri/Desktop/iiith/assignments/sem-3/osn/2/lksh";
//     char *s2 = "/Users/lakshyashastri/Desktop/iiith/assignments/sem-3/osn/2/lksh/includes";
    
//     subtract_strings(s1, strlen(s1), s2, strlen(s2), res);
//     printf("%s\n", res);
    // printf("%lf", current_timestamp());
// }
