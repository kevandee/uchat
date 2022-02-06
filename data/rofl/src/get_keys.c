#include "../inc/ush.h"

static void reader(int *max_len, int *keycode, char *line) {
    *max_len = mx_strlen(line);
    *keycode = 0;
    read(0, keycode, 4);
}

static void add_char(int *position, char *line, int keycode, t_ush *ush) {
    if (mx_strlen(line) >= ush->line_len) {
        ush->line_len += 1024;
        line = realloc(line, ush->line_len);
    }
    for (int i = mx_strlen(line); i > *position; i--) {
        line[i] = line[i - 1];
    }
    line[*position] = keycode;
    (*position)++;
}

static void printer(char *line, int position, int max_len){
    for (int i = position; i <= mx_strlen(line); i++) {
        printf (" ");
    }
    for (int i = 0; i <= max_len + 2; i++) {
        printf ("\b\x1b[2K");
    }
    printf ("\r");
    printf("u$h> ");
    printf ("%s", line);
    /*for (int i = 0; i < mx_strlen(line) - position; i++) {
        printf ("%c[1D", 27);
    }*/
    fflush (NULL);
}

static void swither (int *position, char **line, int keycode, t_ush *ush) {
    if (keycode >= 127) {
        edit_cmd(keycode, position, line);
    }
    else if (keycode < 32)
        signal_detected(keycode, line, position, ush);
    else
        add_char(position, *line, keycode, ush);
}

char *get_keys(t_ush *ush) {
    char *line = mx_strnew(1024);
    int keycode = 0;
    int max_len = 0;
    int position = 0;

    for (;keycode != MX_ENTER && keycode != MX_CTRL_C;) {
        reader(&max_len, &keycode, line);
        swither (&position, &line, keycode, ush);
        if(keycode != MX_CTRL_C)
            printer(line, position, max_len);
    }
    return line;
}
