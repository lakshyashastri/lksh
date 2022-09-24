#include "includes/libs.h"
#include "includes/consts.h"

#ifndef LKSH
#define LKSH
// bg processes
typedef struct bg_process {
    int id;
    char *process_name;
    struct bg_process *next;
    struct bg_process *prev;

    int job_number;
} bg_process;

// bg process head
bg_process *bg_process_head;

// add to bg process LL
bg_process *add_process_node(int id, char *process_name);

#endif
extern char CWD[MAX_LENGTH];
extern char ROOT[MAX_LENGTH];
extern char PREV_WD[MAX_LENGTH];

extern struct passwd *username;
char hostname[MAX_LENGTH];
char TIME_TAKEN_STRING[MAX_LENGTH];
extern int ctrl_c_fired;
extern int foreground;
extern char *foreground_cmd_name;
extern int job_index;
