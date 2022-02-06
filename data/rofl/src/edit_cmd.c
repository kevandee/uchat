#include "../inc/ush.h"

static void backscape(int *position, char *line) {
    if (*position > 0) {
        for (int i = *position - 1; i < mx_strlen(line); i++) {
            line[i] = line[i + 1];
        }
        (*position)--;
    }
}

void edit_cmd(int keycode, int *position, char **line) {
    if (keycode == MX_BACKSCAPE)
        backscape(position, *line);
}
