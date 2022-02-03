#include "../inc/uch_client.h"

int check_auth_input(const char *str) {
    const char *forbidden_symbols = "$&=+<>,_`- ";
    for (int i = 0; i < mx_strlen(str); i++) {
        for (int j = 0; j < 9; j++) {
            if (str[i] == forbidden_symbols[j]) {
                return (int)forbidden_symbols[j];
            }
        }
    }

    if (mx_strstr(str, "..")) {
        return -1;
    }

    if (str[0] == '.' || str[mx_strlen(str) - 1] == '.') {
        return -2;
    }

    return 0;
}
