#include "../inc/libmx.h"

void *mx_realloc(void *ptr, size_t size) {
    if (size <= 0) {
        free(ptr);
        ptr = NULL;
        return NULL;
    }

    if (!ptr) {
        return malloc(size);
    }

    size_t used_size = malloc_size(ptr);
    
    if (used_size >= size) {
        return ptr;
    }

    void *new = malloc(size);
    mx_memcpy(new, ptr, used_size);
    free(ptr);
    ptr = NULL;

    return new;
}
