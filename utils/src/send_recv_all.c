#include "../inc/utils.h"

int send_all(int sockfd, const char *buf, int len) {
    ssize_t n;

    while (len > 0) {
        n = send(sockfd, buf, len, 0);
        if (n < 0)
            return -1;
        buf += n;
        len -= n;
    }

    return 0;
}


int recv_all(int sockfd, char * buf, int len) {
    ssize_t n;

    while (len > 0) {
        n = recv(sockfd, buf, len, 0);
        if (n <= 0)
            return n;
        buf += n;
        len -= n;
    }

    return 1;
}
