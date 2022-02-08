#include "../inc/uch_client.h"

void close_connection(SSL *ssl) {
    int socket = SSL_get_fd(ssl);
    SSL_free(ssl);
    ssl = NULL;
    close(socket);
}

int open_ssl_connection() {
    SSL *ssl;
    SSL_library_init();

    open_client_connection(t_main.ip, t_main.port);
    ssl = SSL_new(t_main.context);
    SSL_set_mode(ssl, SSL_MODE_ASYNC);
    if (SSL_set_fd(ssl, cur_client.serv_fd) == 0) {
        perror("ERROR: socket descriptor attachment failed!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    cur_client.ssl = ssl;

    if (SSL_connect(ssl) == -1) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    printf("SSL: Connected to server with chipher: %s\n", SSL_get_cipher(ssl));
    X509 *cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL) {
        printf("SSL: No certificates configured.\n");
    }
    else {
        X509_free(cert);
    }
    t_main.connected = true;
    return 0;
}

int swiftchat_send(SSL *ssl, void *buf, int size) {
    int receive = 0;

    if (t_main.connected)
    while((receive = send_all(ssl, buf, size)) < 0) {
        t_main.connected = false;
        close_connection(cur_client.ssl);
        printf("closed send\n");
        open_ssl_connection();
        ssl = cur_client.ssl;
        char message[32] = {0};
        // Отправка данных для авторизации на сервер
        sprintf(message, "reconnect");
        send_all(cur_client.ssl, message, 32);
        clear_message(message, 32);
        sprintf(message, "%s", cur_client.login);
        send_all(cur_client.ssl, message, 32);
        clear_message(message, 32);
        sprintf(message, "%s", cur_client.passwd);
        send_all(cur_client.ssl, message, 16);

    }

    return receive;
}

int swiftchat_recv(SSL *ssl, void *buf, int size) {
    int receive = 0;
    printf("recv\n");
    if (t_main.connected) {
        while((receive = recv_all(ssl, buf, size)) <= 0) {
            t_main.connected = false;
            printf("closed recv\n");
            close_connection(cur_client.ssl);
            printf("closed recv\n");
            open_ssl_connection();
            ssl = cur_client.ssl;
            char message[32] = {0};
            // Отправка данных для авторизации на сервер
            sprintf(message, "reconnect");
            send_all(cur_client.ssl, message, 32);
            clear_message(message, 32);

            sprintf(message, "%s", cur_client.login);
            send_all(cur_client.ssl, message, 32);
            clear_message(message, 32);

            sprintf(message, "%s", cur_client.passwd);
            send_all(cur_client.ssl, message, 16);

        }
    }
    
    return receive;
}

