# ctrl + d
- ~~after entering invalid command, ctrl+d does weird shit~~  

# bg
- ~~am i even clearing bg process after its done~~
- invalud command bg proc prints pid: `sleeep 1`

# jobs
~~- push fg proc to bg and jobs list fucks up~~
  ~~- some string bullshit: need to put null char somewhere~~
- `sleep 5&` then `vim&`: do `jobs` after sleep is done and it won't print vim: something wrong with LL
  - if first process finishes then entire LL is gone
- jobs doesn't remove proc if it's a bg proc from ctrl+z: it should probably
- `sleep 5&` then `sleep 4&`, wait for both to end then do `jobs`: takes too long? works if the second command is `sleep 2`
  - also works if `sleep 5& sleep 4&`, linked list problem maybe
- jobs also won't work if you `kill` a proc: probably dont need to handle this because kill is not a custom command
- **jobs print full thing doesn't work for ctrl+z: handle by turning full_cmd to pointer and using malloc?**

# fg bg
- fg sometimes works somtimes doesn't
- `sleep 5` then `ctrl+z` then `bg 1`, proc finishes and sleep just exits
- bg process name is fucked: `sleep 5` then `ctrl+z` then `bg 1`
- ~~fg should bring vim to foreground~~

# other
- perror
- bg processes linked list + add foreground process after ctrl+z
- ctrl+c newline
- put shit in lksh_helpers.c and header files: fix imports
~~- make shit like HIST dynamic by using strlen and shit: **makes those warnings go away**~~
  ~~- suppress sprintf warnings first~~
- where tf am i giving foreground_cmd_name its value
- **redir happening even if cmd name wrong**
- arrow keys: QOL
- **TOOK N SECONDS BROKE**
- autcomplete pressing enter after too many completions breaks shit
- `discover includes/`: not neccessary tbh

CLEAR UP ON FORK
WNOHANG etc
when `strcpy` and `=` for strings
