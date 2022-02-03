#include "../inc/libmx.h"

int mx_quicksort(char** arr, int left, int right) {
    if (!arr) {
        return -1;
    }

    int i = left;
    int j = right;
    char *temp; 
    char *pivot = arr[left];
    int counter = 0;

    while (i <= j) {
        while (mx_strlen(arr[i]) < mx_strlen(pivot)) {
            i++;
        }
        while (mx_strlen(arr[j]) > mx_strlen(pivot)) {
            j--;
        }

        if (i <= j) {
            if (mx_strlen(arr[i]) > mx_strlen(arr[j])) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
                counter++;
            }

            i++; 
            j--;
        }
    }

    if (left < j) {
        counter += mx_quicksort(arr, left, j);
    }
    if (i < right) {
        counter += mx_quicksort(arr, i, right);
    }

    return counter;
}
