#include "../inc/ush.h"

int main() {
    t_ush *ush = NULL;
    init(&ush);
    char* line = NULL;

    while (1) {;
        mx_printstr("u$h> ");
        detecter();

        isatty(0) ? (line = line_reader(ush)) : (line = ush_line_reader(ush));
        if (line[0] == '\0') {
            free(line);
            continue;
        }

        ush_separator(line, ush);
        while(ush->cmd_queue) {
            int check = ush_parser(ush->cmd_queue->data, ush);
            if (check == -1) {
                continue;
            }

            ush_execute(ush);

            if (ush->cmd_num == 8) {
                ush_exit(ush);
            }

            mx_clear_ldata(&ush -> argv);
            mx_clear_list(&ush -> argv);
            ush -> argv = NULL;
            mx_del_strarr(&ush->argv_arr);
            ush->argv_arr = NULL;

            mx_pop_front(&ush->cmd_queue);
        }
        mx_strdel(&line);
        line = NULL;
    }
    free(ush);
    return 0;
}
