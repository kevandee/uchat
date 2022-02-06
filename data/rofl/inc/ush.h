#ifndef USH_H
#define USH_H

#include "../libmx/inc/libmx.h"

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/acl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/xattr.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>

#define MX_PATH (getenv("PATH"))

#define MX_INPUT_SIZE  1024
#define MX_K_LEFT      4479771  // Edit keys
#define MX_K_RIGHT     4414235
#define MX_K_HOME      4741915
#define MX_K_END       4610843
#define MX_K_UP        4283163  // History keys
#define MX_K_DOWN      4348699
#define MX_P_UP        2117425947
#define MX_P_DOWN      2117491483
#define MX_C_PROMPT    42946
#define MX_CTRL_D      4
#define MX_CTRL_C      3
#define MX_CTRL_R      18
#define MX_CTRL_Z      26
#define MX_BACKSCAPE   127
#define MX_TAB         9
#define MX_ENTER       10


typedef struct s_process {
    char    *fullpath;     // For execve
    char    **argv;        // Gets in create_job.c
    char    *command;
    char    *arg_command;
    int     id;
    pid_t   pid;
    pid_t   pgid;
    int     exit_code;
    char    *path;
    char    **env;
    int     status;         // Status RUNNING DONE SUSPENDED CONTINUED TERMINATED
    int     foregrd;
    
    int     type;  // COMMAND_BUILTIN = index in m_s->builtin_list; default = 0
    //struct s_process *next;  // Next process in pipeline
}             t_process;

typedef struct s_job {
    int     job_id;             // Number in jobs control
    char    *command;          // Command line, used for messages
    t_process *first_pr;    // List of processes in this job
    pid_t   pgid;             // Process group ID
    char    *path;
    char    **env;
    int     flag;
    int     exit_code;
    int     foregrd;            // Foregrd = 1 or background execution = 0
    struct termios tmodes;  // Saved terminal modes
    struct s_job *next;     // Next job separated by ";" "&&" "||"
}             t_job;


typedef struct s_ush {
    t_job *jobs[10000];
    pid_t pid;
    pid_t ppid;
    pid_t pgid;
    sigset_t mask;

    t_process *foreground;
    t_list *background;
    int count_bg;
    char *pwd;
    char *prev_pwd;
    char **argv_arr;
    int cmd_num;
    bool quote;
    t_list *argv;
    int err;
    int line_len;
    int count_job;
    struct termios tmodes;
    t_list *cmd_queue;
    int redirect;
}               t_ush;

typedef struct s_opt_ush {
    bool u;
    bool p;
    bool i;
}              t_opt_ush;

void init(t_ush **ush);
char *ush_input();
int ush_parser(char *src, t_ush *ush);
int check_input(char const *s);
void ush_execute(t_ush *ush);
void ush_separator(char *s, t_ush *ush);
char **ush_strsplit(char const *s, t_ush *ush);
char *del_quot(char *src);
char *get_keys(t_ush *ush);
int bg_new_id(t_list *background);

char *line_reader(t_ush *ush);
char *ush_line_reader(t_ush *ush);
void signal_detected(int keycode, char **line, int *position, t_ush *ush);
void edit_cmd(int keycode, int *position, char **line);


int launch_proc(char **argv,t_ush *ush);
char *get_prog_path(char *name, const char *PATH);
void detecter();
void free_sh(t_ush **ush);

void ush_export(t_ush *ush);
void ush_unset(t_ush *ush);
void ush_exit(t_ush *ush);
void ush_fg(t_ush *ush);
void ush_env(t_ush *ush);
void ush_cd(t_ush *ush);
void ush_echo(t_ush *ush);
void ush_pwd(t_ush *ush);
int ush_which(t_ush *ush);

void mx_disable_canon(void);
void mx_enable_canon(void);
struct termios *mx_get_tty(void);

void list_to_arr(t_list *list, char ***arr);

t_process *new_process(pid_t pid);
void add_bg(t_process *proc, t_ush *ush);

#endif
