#include "../inc/uch_client.h"

SSL_CTX* CTX_initialize_client() { 
    //describes internall ssl library methods (needed for SSL_CTX)
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = SSLv23_client_method();
    ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

void open_client_connection(char* server_IP, int port) {
    struct sockaddr_in adr = {0};
    cur_client.serv_fd = socket(AF_INET, SOCK_STREAM, 0);

    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    if(inet_pton(AF_INET, server_IP, &adr.sin_addr)<=0)
    {
        perror("\n inet_pton error occured\n");
        return;
    } 

    while (connect(cur_client.serv_fd, (struct sockaddr *)&adr, sizeof(adr)) != 0) {
        close(cur_client.serv_fd);
        sleep(1);
        cur_client.serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    }

    //inet_pton(AF_INET, server_IP, &adr.sin_addr); //"127.0.0.1"
    cur_client.adr = adr;
}
