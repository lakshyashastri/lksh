#include "lksh.h"

#include "includes/libs.h"
#include "includes/consts.h"

#include "lksh_cmds/lksh_pwd.c"
#include "lksh_cmds/lksh_echo.c"
#include "lksh_cmds/lksh_cd.c"
#include "lksh_cmds/lksh_ls.c"
#include "lksh_cmds/lksh_history.c"
#include "lksh_cmds/lksh_pinfo.c"
#include "lksh_cmds/lksh_discover.c"
#include "lksh_cmds/lksh_handlers.c"
#include "lksh_cmds/lksh_jobs.c"

// initial working directory ie root
char ROOT[MAX_LENGTH];

// variable to keep track of current working directory (relative from ~)
char CWD[MAX_LENGTH] = "~";

// variable to keep track of previous working directory (absolute from actual root)
char PREV_WD[MAX_LENGTH] = "~";

// time taken by foreground processes
long long int TIME_TAKEN = 0;
char TIME_TAKEN_STRING[MAX_LENGTH];

// get username
struct passwd *username;

// get hostname
char hostname[MAX_LENGTH];

// ctrl c fired
int ctrl_c_fired = 0;

// foreground pid running
int foreground = -1;
char *foreground_cmd_name = NULL;

// add to bg process LL
bg_process *add_process_node(int id, char *process_name) {
    bg_process *node = malloc(sizeof(bg_process));
    node -> id = id;
    node -> job_number = -1;

    if (process_name == NULL) {
        node -> process_name = NULL;
    } else {
        node -> process_name = malloc(sizeof(char) * (strlen(process_name) + 1));
        strcpy(node -> process_name, process_name);
    }

    // cur now points to last node of current LL
    bg_process *cur = bg_process_head;
    if (cur != NULL) {
        while (cur -> next != NULL) {
            cur = cur -> next;
        }
    }

    node -> prev = cur;
    node -> next = NULL;
    if (cur != NULL) {
        cur -> next = node;
    }

    if (bg_process_head == NULL) {
        bg_process_head = node;
    }

    return node;
}

int main() {

    // signals
    signal(SIGCHLD, child_handler);
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, ctrl_z_handler);

    // clear terminal screen at initialization
    cls();

    // list of valid commands
    int NUM_CMDS = 9;
    char *CMDS[9] = {"pwd", "echo", "cd", "ls", "history", "pinfo", "discover", "jobs", "seg"};

    // get username
    username = getpwuid(getuid());
    
    // get system name
    hostname[MAX_LENGTH - 1] = '\0';
    gethostname(hostname, MAX_LENGTH);

    // get initial working directory ie root
    ROOT[MAX_LENGTH - 1] = '\0';
    getcwd(ROOT, MAX_LENGTH);

    // fill bg process head node
    // bg_process_head -> id = -1;
    // bg_process_head -> process_name = NULL;
    // bg_process_head -> prev = bg_process_head -> next = NULL;
    bg_process_head = NULL;

    while (1) {

        // command line input
        char *input;
        size_t MAX_INPUT_LENGTH = MAX_LENGTH;
        size_t input_length;

        // allocate memory to command line input
        input = malloc(MAX_INPUT_LENGTH * sizeof(char));
        if (input == NULL) {
            printf("Out of memory\n");
            break;
        }

        // shell prompt
        printf("%s<%s%s@%s%s:%s%s%s>%s ", COLOR_GREEN, username -> pw_name, COLOR_RED, COLOR_CYAN, hostname, COLOR_PURPLE, CWD, TIME_TAKEN_STRING, COLOR_RESET);
        input_length = getline(&input, &MAX_INPUT_LENGTH, stdin);

        // handle ctrl + d
        if (input_length == EOF) {
            bg_process *cur = bg_process_head;
            while (cur != NULL) {
                kill(cur -> id, SIGKILL);
                cur = cur -> next;
            }
            return 0;
        }

        // clear time string
        memset(TIME_TAKEN_STRING, 0, sizeof(TIME_TAKEN));

        // log start time
        time_t start_time = time(NULL);

        // call this only when input is not empty
        lksh_history_write(input);

        // parse ;
        char *semi = ";";
        char *cmds;
        cmds = strtok(input, semi);

        // convert input to input.split(';') array
        char *input_splitted[MAX_LENGTH];
        int input_split_count = 0;
        while (cmds != NULL) {
            input_splitted[input_split_count++] = cmds;
            cmds = strtok(NULL, semi);
        }

        for (int ii = 0; ii < input_split_count; ii++) {

            char input_copy[MAX_LENGTH];
            strcpy(input_copy, input_splitted[ii]);

            // parse input for spaces and tabs
            char *sep = " \t";
            char *token;
            token = strtok(input_splitted[ii], sep);

            // convert tokenized string to array
            char *splits[MAX_LENGTH];
            int split_count = 0;
            while (token != NULL) {
                splits[split_count++] = token;
                token = strtok(NULL, sep);
            }

            // remove trailing "\n" from last element in splits array
            strtok(splits[split_count - 1], "\n");

            // empty enter
            if (strcmp(splits[0], "\n") == 0) {
                continue;
            }

            // get number of &s in string
            int num_ands = 0;
            for (int i = 0; i <= strlen(input_copy); i++) {
                if (input_copy[i] == '&') {
                    num_ands += 1;
                }
            }

            // get &-separated string
            char *and_sepped[MAX_LENGTH];
            int and_sep_count = 0;
            char *and_sep = "&";
            token = strtok(input_copy, and_sep);
            while (token != NULL) {
                and_sepped[and_sep_count++] = token;
                token = strtok(NULL, and_sep);
            }

            // strip ' ' and '\n'
            for (int i = 0; i < and_sep_count; i++) {
                if (and_sepped[i][0] == ' ' || and_sepped[i][0] == '\n') {
                    memmove(and_sepped[i], and_sepped[i] + 1, strlen(and_sepped[i]));
                }
                if (and_sepped[i][strlen(and_sepped[i]) - 1] == ' ' || and_sepped[i][strlen(and_sepped[i]) - 1] == '\n') {
                    and_sepped[i][strlen(and_sepped[i]) - 1] = '\0';
                }
            }

            // bg execute
            for (int i = 0; i < num_ands; i++) {
                // split into args list
                char *tok;
                tok = strtok(and_sepped[i], sep);
                char *args_arr[MAX_LENGTH];
                int args_c = 0;
                while (tok != NULL) {
                    args_arr[args_c++] = tok;
                    tok = strtok(NULL, sep);
                }

                // check if command is custom or system
                int custom = 0;
                for (int i = 0; i < NUM_CMDS; i++) {
                    if (strcmp(args_arr[0], CMDS[i]) == 0) {
                        custom = 1;
                        break;
                    }
                }
                if (custom) {
                    printf("lksh: Background processes are not compatible with built-in commands: %s\n", args_arr[0]);
                    continue;
                }

                int pid = fork();
                if (pid < 0) {
                    printf("lksh: failed to create process: %s\n", args_arr[0]);
                    continue;

                } else if (pid == 0) {
                    setpgid(0, 0);
                    
                    args_arr[args_c] = NULL;
                    if (execvp(args_arr[0], args_arr) == -1) {
                        printf("lksh: command not found: %s\n", args_arr[0]);
                    }

                } else {
                    add_process_node(pid, args_arr[0]);

                    bg_process *cur = bg_process_head;
                    int num_bg = 0;
                    while (cur != NULL) {
                        num_bg += 1;
                        cur = cur -> next;
                    }
                    printf("[%d] %d\n", num_bg, pid);
                    // bg_ids[num_bg] = pid;
                    // bg_names[num_bg] = malloc(sizeof(char) * (strlen(args_arr[0]) + 1));
                    // strcpy(bg_names[num_bg++], args_arr[0]);
                }
            }
            
            // fg execute
            if (and_sep_count - num_ands == 1 && strlen(and_sepped[and_sep_count - 1]) != 0) {

                // split into args list
                char *tok;
                tok = strtok(and_sepped[and_sep_count - 1], sep);
                char *args_arr[MAX_LENGTH];
                int args_c = 0;
                while (tok != NULL) {
                    args_arr[args_c++] = tok;
                    tok = strtok(NULL, sep);
                }

                // fg execute
                if (strcmp(and_sepped[and_sep_count - 1], "pwd") == 0) {
                    lksh_pwd();

                } else if (strcmp(and_sepped[and_sep_count - 1], "echo") == 0) {
                    lksh_echo(args_arr, args_c);

                } else if (strcmp(and_sepped[and_sep_count - 1], "cd") == 0) {
                    lksh_cd(args_arr, args_c);

                } else if (strcmp(and_sepped[and_sep_count - 1], "ls") == 0) {
                    lksh_ls(args_arr, args_c);

                } else if (strcmp(and_sepped[and_sep_count - 1], "history") == 0) {
                    lksh_history(args_arr, args_c);
                
                } else if (strcmp(and_sepped[and_sep_count - 1], "pinfo") == 0) {
                    lksh_pinfo(args_arr, args_c);
                
                } else if (strcmp(and_sepped[and_sep_count - 1], "discover") == 0) {
                    lksh_discover(args_arr, args_c);
                
                } else if (strcmp(and_sepped[and_sep_count - 1], "jobs") == 0) {
                    lksh_jobs(args_arr, args_c);
                
                } else if (strcmp(and_sepped[and_sep_count - 1], "seg") == 0) {
                    raise(SIGSEGV);

                } else {

                    // execute
                    int exe_pid = fork();
                    if (!exe_pid) {

                        // handle ctrl+c
                        signal(SIGINT, ctrl_c_handler);

                        if (execvp(and_sepped[and_sep_count - 1], args_arr) == -1) {
                            printf("lksh: command not found: %s\n", and_sepped[and_sep_count - 1]);
                            exit(1);
                        }

                    } else {
                        foreground = exe_pid;
                        foreground_cmd_name = malloc(sizeof(char) * (strlen(args_arr[0]) + 1));
                        
                        // wait for execvp to end
                        waitpid(exe_pid, NULL, WUNTRACED);
                        // printf("\n");
                    }
                }

                // end time
                time_t end_time = time(NULL);

                // display time
                TIME_TAKEN = end_time - start_time;
                if (TIME_TAKEN >= 1) {
                    sprintf(TIME_TAKEN_STRING, ": took %llds", TIME_TAKEN);
                }

                // reset foreground
                foreground = -1;
            }
        }
    }
}
