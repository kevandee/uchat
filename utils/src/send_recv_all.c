#include "../inc/utils.h"

int send_all(SSL *sockfd,  char *buf, int len) {
    ssize_t n;

    while (len > 0) {
        n = SSL_write(sockfd, buf, len);
        if (n <= 0)
            return -1;
        buf += n;
        len -= n;
    }

    return 1;
}


int recv_all(SSL *sockfd, char * buf, int len) {

    ssize_t n;

    while (len > 0) {
        n = SSL_read(sockfd, buf, len);
        if (n <= 0)
            return -1;
        buf += n;
        len -= n;
    }

    return 1;
}
