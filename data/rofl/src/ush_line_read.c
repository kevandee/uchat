#include "../inc/ush.h"

static struct termios term_off(void) {
    struct termios tumbler;
    struct termios tty;

    tcgetattr (0, &tty);
    tumbler = tty;
    tty.c_lflag &= ~(ICANON|ECHO|ISIG|BRKINT|ICRNL
        |INPCK|ISTRIP|IXON|OPOST|IEXTEN);
    tty.c_cflag |= (CS8);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    tcsetattr (0, TCSAFLUSH, &tty);
    return tumbler;
}

static void term_on(struct termios tumbler) {
    printf("\n");
    tcsetattr (0, TCSAFLUSH, &tumbler);
}

/*static void jynx_maze(t_ush *ush, char *line) {
    if (ush->history_count == ush->history_size) {
        ush->history_size += 1000;
        ush->history = (char **)realloc(ush->history, ush->history_size);
    }
    if (strcmp(line, "") != 0) {
        ush->history[ush->history_count] = strdup(line);
        ush->history_count++;
    }
}*/

///////
char *ush_line_reader(t_ush *ush) {
    char *line = mx_strnew(1);
    char *res = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) < 0) {
        if (!isatty(0)) {
            ush->err = 0;
            //mx_clear_all(ush);
            exit(0);
        }
    }
    if(line[0] != '\0'){
        res = mx_strdup(line);
        mx_strdel(&line);
    }
    return res;
}

///////////

char *line_reader(t_ush *ush) {
    char *line;
    struct termios tumbler;
    int stream1 = dup(1);
    int tty = open("/dev/tty", O_WRONLY);

    dup2(tty, 1);
    tumbler = term_off();
    ush->line_len = 1024;
    line = get_keys(ush);
    //jynx_maze(ush, line);
    //ush->history_index = ush->history_count;
    term_on(tumbler);
    dup2(stream1, 1);
    close(stream1);
    close(tty);
    return line;
}
