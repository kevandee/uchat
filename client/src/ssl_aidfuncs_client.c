#include "../inc/uch_client.h"

SSL_CTX* CTX_initialize_client() { 
    //describes internall ssl library methods (needed for SSL_CTX)
    const SSL_METHOD *ssl_method;
    
    //created by a server or client once per program life-time
    // and holds mainly default values for  
    //SSL structures which are later created for the connections
    SSL_CTX *ssl_context;

    OpenSSL_add_all_algorithms(); // loads all ssl diget and cipher algos
    SSL_load_error_strings();

    ssl_method = TLS_client_method();
    //ssl_method = TLSv1_2_client_method();
    ssl_context = SSL_CTX_new(ssl_method);
    if (ssl_context == NULL) {
        // vvv needs crypto err vvv
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ssl_context;
}

void open_client_connection(char* server_IP, int port) {
    struct sockaddr_in adr = {0};
    cur_client.serv_fd = socket(AF_INET, SOCK_STREAM, 0);

    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    if (connect(cur_client.serv_fd, (struct sockaddr *)&adr, sizeof(adr)) != 0) {
        close(cur_client.serv_fd);
        perror(server_IP);
        abort();
    }

    inet_pton(AF_INET, server_IP, &adr.sin_addr); //"127.0.0.1"
    cur_client.adr = adr;
}
