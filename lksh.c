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
#include "lksh_cmds/lksh_sig.c"
#include "lksh_cmds/lksh_fg.c"
#include "lksh_cmds/lksh_bg.c"

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

// jobs LL
int job_index = 0;

// current cmd
char full_cmd_copy[MAX_LENGTH] = {0};

// add to bg process LL
bg_process *add_process_node(int id, char *process_name, char full_cmd[MAX_LENGTH]) {
    bg_process *node = malloc(sizeof(bg_process));
    node -> id = id;
    node -> job_number = ++job_index;
    strcpy(node -> full_cmd, full_cmd);

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

// autocomplete functions
struct termios orig_termios;

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main() {

    // signals
    signal(SIGCHLD, child_handler);
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, ctrl_z_handler);

    // clear terminal screen at initialization
    cls();

    // list of valid commands
    int NUM_CMDS = 12;
    char *CMDS[12] = {"pwd", "echo", "cd", "ls", "history", "pinfo", "discover", "jobs", "seg", "sig", "fg", "bg"};

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
        // size_t input_length;

        // allocate memory to command line input
        input = malloc(MAX_INPUT_LENGTH * sizeof(char));
        if (input == NULL) {
            printf("Out of memory\n");
            break;
        }
        memset(input, '\0', MAX_INPUT_LENGTH); // random shit happens if you dont do this: mem isn't clear

        // shell prompt
        printf("%s<%s%s@%s%s:%s%s%s>%s ", COLOR_GREEN, username -> pw_name, COLOR_RED, COLOR_CYAN, hostname, COLOR_PURPLE, CWD, TIME_TAKEN_STRING, COLOR_RESET);

        // read char by char
        setbuf(stdout, NULL);
        enableRawMode();
        char c;
        int pt = 0;
        int empty_enter = 0;
        while (read(STDIN_FILENO, &c, 1)) {
            if (iscntrl(c)) {

                // tab
                if (c == 9) {

                    // vars
                    DIR *dir = opendir(".");
                    char *dir_files[MAX_LENGTH];
                    int dir_files_counter = 0;
                    int item_type[MAX_LENGTH] = {0};

                    // get all files in current directory
                    struct dirent *dir_data;
                    while ((dir_data = readdir(dir)) != NULL) {
                        // ignore hidden files and dir pointers
                        if (dir_data -> d_name[0] == '.') {
                            continue;
                        }
                        dir_files[dir_files_counter] = malloc(sizeof(char) * MAX_LENGTH * 10);
                        strcpy(dir_files[dir_files_counter++], dir_data -> d_name);
                    }

                    // get file types of all items in current directory
                    for (int j = 0; j < dir_files_counter; j++) {
                        struct stat sb;
                        if (stat(dir_files[j], &sb) == 0 && sb.st_mode & S_IFDIR) {
                            item_type[j] = 0; // directory

                        } else if (sb.st_mode & S_IXUSR) {
                            item_type[j] = 1; // executable

                        } else if (sb.st_mode & S_IFREG) {
                            item_type[j] = 2; // file
                        }
                    }

                    // display all items ie empty tab
                    printf("\n");
                    char *prompt = NULL;
                    if (!pt) {
                        for (int i = 0; i < dir_files_counter; i++) {
                            printf("%s%c\n", dir_files[i], !item_type[i] ? '/' : ' ');
                        }
                    
                    // something is typed in stdin
                    } else {
                        char *matches[MAX_LENGTH] = {0};
                        int match_count = 0;
                        char *last = strrchr(input, ' ');

                        int print_all = 0;
                        if (last && *(last + 1)) {
                            last += 1;

                        } else {
                            last = input;
                            if (last[pt - 1] == ' ') { // cases like `cat `
                                print_all = 1;
                            }
                        }

                        char common[MAX_LENGTH] = {0};
                        if (!print_all) {
                            for (int i = 0; i < dir_files_counter; i++) {
                                if (strstr(dir_files[i], last) == dir_files[i]) {
                                    printf("%s%c\n", dir_files[i], !item_type[i] ? '/' : ' ');
                                    matches[match_count] = malloc(sizeof(char) * (strlen(dir_files[i]) + 1));
                                    strcpy(matches[match_count++], dir_files[i]);
                                }
                            }

                            if (match_count == 1) {
                                // get match count index in dir_files
                                int match_index = 0;
                                for (int i = 0; i < dir_files_counter; i++) {
                                    if (!strcmp(dir_files[i], matches[0])) {
                                        match_index = i;
                                        break;
                                    }
                                }
                                strcpy(common, matches[0]);
                                strcat(common, !item_type[match_index] ? "/" : " ");

                            } else if (match_count > 1) {
                                // search through all matches for common substring
                                int same = 1;
                                int same_index = 0;
                                while (same) {
                                    for (int i = 0; i < match_count - 1; i++) {
                                        if (matches[i][same_index] != matches[i + 1][same_index]) {
                                            same = 0;
                                            break;
                                        }
                                    }
                                    if (same) {
                                        common[same_index] = matches[0][same_index];
                                    }
                                    same_index += 1;
                                }
                            }

                        } else {
                            for (int i = 0; i < dir_files_counter; i++) {
                                printf("%s%c\n", dir_files[i], !item_type[i] ? '/' : ' ');
                            }
                        }

                        last = strrchr(input, ' '); 
                        if (last == NULL) {
                            strcpy(input, common);
                            pt += strlen(common);
                        
                        // append the difference between last and common to input
                        } else {
                            char *diff = common + strlen(last) - 1;
                            strcat(input, diff);
                            pt += strlen(diff);
                        }

                        // create prompt
                        prompt = malloc(sizeof(char) * (strlen(COLOR_GREEN) + strlen(username -> pw_name) + strlen(COLOR_RED) + strlen(COLOR_CYAN) + strlen(hostname) + strlen(CWD) + strlen(TIME_TAKEN_STRING) + strlen(COLOR_RESET) + 1 + 10 + strlen(input)));
                        sprintf(prompt, "%s<%s%s@%s%s:%s%s%s>%s %s", COLOR_GREEN, username -> pw_name, COLOR_RED, COLOR_CYAN, hostname, COLOR_PURPLE, CWD, TIME_TAKEN_STRING, COLOR_RESET, input);
                    }
                    if (prompt == NULL) {
                        printf("%s<%s%s@%s%s:%s%s%s>%s ", COLOR_GREEN, username -> pw_name, COLOR_RED, COLOR_CYAN, hostname, COLOR_PURPLE, CWD, TIME_TAKEN_STRING, COLOR_RESET);
                    } else {
                        printf("%s", prompt);
                    }
                
                // \n
                } else if (c == 10) {
                    if (!pt) {
                        empty_enter = 1;
                    }
                    break;

                // arrow keys
                } else if (c == 27) {
                    char buf[3];
                    buf[2] = 0;
                    if (read(STDIN_FILENO, buf, 2) == 2) { // length of escape code
                        // printf("\rarrow key: %s", buf);
                    }

                // backspace
                } else if (c == 127) {
                    if (pt > 0) {
                        if (input[pt - 1] == 9) {
                            for (int i = 0; i < 7; i++) {
                                printf("\b");
                            }
                        }
                        input[--pt] = '\0';
                        printf("\b \b");
                    }

                // ctrl d
                } else if (c == 4) {
                    exit(0);

                } else {
                    printf("%c", c);
                }

            } else {
                input[pt++] = c;
                printf("%c", c);
            }
        }
        printf("\n");
        disableRawMode();

        if (empty_enter) {
            continue;
        }

        // handle ctrl + d
        // if (input_length == EOF) {
        //     bg_process *cur = bg_process_head;
        //     while (cur != NULL) {
        //         kill(cur -> id, SIGKILL);
        //         cur = cur -> next;
        //     }
        //     return 0;
        // }

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

            char pipe_copy[MAX_LENGTH];
            strcpy(pipe_copy, input_splitted[ii]);

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

                // putting this somehow fucks bg processes up
                if (strcmp(args_arr[0], "") == 0) {
                    raise(SIGSEGV);
                    continue;
                }

                // check if command is custom or system
                int custom = 0;
                for (int j = 0; j < NUM_CMDS; j++) {
                    if (strcmp(args_arr[0], CMDS[j]) == 0) {
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
                    strcpy(full_cmd_copy, args_arr[0]);
                    strcat(full_cmd_copy, " ");
                    for (int a = 1; a < args_c; a++) {
                        strcat(full_cmd_copy, args_arr[a]);
                        if (args_c - 1 != a) {
                            strcat(full_cmd_copy, " ");
                        }
                    }

                    add_process_node(pid, args_arr[0], full_cmd_copy);

                    bg_process *cur = bg_process_head;
                    int num_bg = 0;
                    while (cur != NULL) {
                        num_bg += 1;
                        cur = cur -> next;
                    }
                    printf("[%d] %d\n", num_bg, pid);
                }
            }
            
            // fg execute
            if (and_sep_count - num_ands == 1 && strlen(and_sepped[and_sep_count - 1]) != 0) {

                // get number of pipes (piping assumes simple pipe string with no bg process shit)
                char pipe_temp[MAX_LENGTH];
                strcpy(pipe_temp, pipe_copy);
                int pipe_count = 0;
                char *tok = strtok(pipe_copy, " ");
                while (tok != NULL) {
                    if (!strcmp(tok, "|")) {
                        pipe_count += 1;
                    }
                    tok = strtok(NULL, " ");
                }
                strcpy(pipe_copy, pipe_temp);

                int new_stdin = STDIN_FILENO;
                tok = strtok(pipe_copy, "|");
                for (int pipe_index = 0; pipe_index <= pipe_count; pipe_index++) {
                    and_sepped[and_sep_count - 1] = tok;
                    tok = strtok(NULL, "|");

                    if (and_sepped[and_sep_count - 1][0] == ' ') {
                        memmove(and_sepped[and_sep_count - 1], and_sepped[and_sep_count - 1] + 1, strlen(and_sepped[and_sep_count - 1]));
                    }

                    int pipes[2];
                    pipe(pipes);

                    int ppid = fork();

                    if (ppid < 0) {
                        printf("Failed to create new process\n");
                        continue;

                    } else if (ppid == 0) {
                        dup2(new_stdin, STDIN_FILENO);
                        if (pipe_index != pipe_count) {
                            dup2(pipes[1], STDOUT_FILENO);
                        }
                        close(pipes[0]);

                        // split into args list
                        tok = strtok(and_sepped[and_sep_count - 1], sep);
                        char *args_arr[MAX_LENGTH];
                        int args_c = 0;
                        while (tok != NULL) {
                            args_arr[args_c++] = tok;
                            tok = strtok(NULL, sep);
                        }

                        // input redirection
                        int backup_stdin = -1;
                        for (int i = 1; i < split_count; i++) {
                            if (!strcmp(splits[i], "<")) {
                                backup_stdin = dup(STDIN_FILENO);
                                int new_stdin = open(splits[i + 1], O_RDONLY);
                                if (new_stdin < 0) {
                                    printf("Input file does not exist");
                                }
                                dup2(new_stdin, STDIN_FILENO);
                                close(new_stdin);
                            }
                        }

                        // output redirection
                        // assuming simple redir input
                        int backup_stdout = -1;
                        for (int i = 1; i < args_c; i++) {
                            if (!strcmp(args_arr[i], ">")) {
                                backup_stdout = dup(STDOUT_FILENO);
                                int old_fd = open(args_arr[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                dup2(old_fd, STDOUT_FILENO);
                                close(old_fd);
                                args_c -= 2;
                                break;

                            } else if (!strcmp(args_arr[i], ">>")) {
                                backup_stdout = dup(STDOUT_FILENO);
                                int old_fd = open(args_arr[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                                dup2(old_fd, STDOUT_FILENO);
                                close(old_fd);
                                args_c -= 2;
                                break;
                            }
                        }

                        // input redirection
                        if (backup_stdin != -1) {
                            args_c -= 2;
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
                        
                        } else if (strcmp(and_sepped[and_sep_count - 1], "sig") == 0) {
                            lksh_sig(args_arr, args_c);

                        } else if (strcmp(and_sepped[and_sep_count - 1], "fg") == 0) {
                            lksh_fg(args_arr, args_c);
                        
                        } else if (strcmp(and_sepped[and_sep_count - 1], "bg") == 0) {
                            lksh_bg(args_arr, args_c);
                        
                        } else if (strcmp(and_sepped[and_sep_count - 1], "seg") == 0) {
                            raise(SIGSEGV);

                        } else {

                            // execute
                            int exe_pid = fork();
                            if (!exe_pid) {

                                // handle ctrl+c
                                signal(SIGINT, ctrl_c_handler);

                                // new execvp array in case of I/O redirection
                                char *trimmed_args_arr[MAX_LENGTH];
                                for (int i = 0; i < args_c; i++) {
                                    trimmed_args_arr[i] = args_arr[i];
                                }
                                trimmed_args_arr[args_c] = NULL;
                                
                                if (execvp(and_sepped[and_sep_count - 1], trimmed_args_arr) == -1) {
                                    printf("lksh: command not found: '%s'\n", and_sepped[and_sep_count - 1]);
                                    _exit(1);
                                }

                            } else {
                                foreground = exe_pid;
                                foreground_cmd_name = malloc(sizeof(char) * (strlen(args_arr[0]) + 1));
                                foreground_cmd_name[strlen(args_arr[0])] = '\0';
                                
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

                        // reset input fd
                        if (backup_stdin != -1) {
                            dup2(backup_stdin, STDIN_FILENO);
                        }

                        // reset output fd
                        if (backup_stdout != -1) {
                            dup2(backup_stdout, STDOUT_FILENO);
                        }
                        
                        _exit(0);
                    } else {
                        waitpid(ppid, NULL, 0);
                        close(pipes[1]);
                        new_stdin = pipes[0];
                    }
                }
            }
        }
    }
}
