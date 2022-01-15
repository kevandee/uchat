#include "../inc/utils.h"

void clear_message(char *mes, const int size) {
    for (int i = 0; i < size; i++) {
        if(mes[i] == '\0') {
            break;
        }
        mes[i] = '\0';
    }
}
