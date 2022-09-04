#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // dynamically calculate space for filesize in ls -l

#include <sys/utsname.h> // uname
#include <unistd.h> // gethostname, getcwd, chdir
#include <pwd.h> // getpwuid
#include <dirent.h> // ls
#include <sys/stat.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
