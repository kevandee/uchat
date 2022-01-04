#include "../inc/uch_client.h"



void *sender_func(void *param) {
    t_client *cur_client = (t_client *)param;
    char message[512];
    char buf[512 + 32];
    while(1) {
        printf("> ");
        fflush(stdout);
        fgets(message, 512, stdin);
        if (mx_strcmp(message, "users\n") == 0) {
            send(cur_client->cl_socket, message, mx_strlen(message) - 1, 0);
        }
        else if (mx_strcmp(message, "exit\n") == 0) {
            send(cur_client->cl_socket, message, 4, 0);
            break;
        }
        else {
            sprintf(buf, "%s: %s\n", cur_client->name, message);
            send(cur_client->cl_socket, buf, mx_strlen(buf) - 1, 0);
        }
        clear_message(message, 512);
        clear_message(buf, 512 + 32);

    }
    return NULL;
}

void *rec_func(void *param) {
    int fd = *(int *)param;
    char message[512] = {0};
    while (1) {
		int receive = recv(fd, message, 512, 0);
        if (receive > 0) {
            printf("%s", message);
            printf("> ");
            fflush(stdout);
        } else if (receive == 0) {
            break;
        } else {
                // -1
		}
		clear_message(message, 512);
    }
    return NULL;

}



int main(int argc, char *argv[]) {
    if (argc != 3) {
        mx_printerr("usage: ./uchat <server IP> <port>\n");
        return -1;
    }

    char name[32];
    printf("Enter name: ");
    fgets(name, 32, stdin);
    name[mx_strlen(name) - 1] = '\0';
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(mx_atoi(argv[2]));
    connect(fd, (struct sockaddr *)&adr, sizeof(adr));
    inet_pton(AF_INET, argv[1], &adr.sin_addr); //"127.0.0.1"

    
    send(fd, name, mx_strlen(name), 0);
    
    pthread_t sender_th;
    pthread_t rec_th;
    t_client cur_client = {
        .adr = adr,
        .cl_socket = fd,
        .name = name
    };
    pthread_create(&sender_th, NULL, sender_func, &cur_client);
    pthread_create(&rec_th, NULL, rec_func, &fd);
    pthread_join(sender_th, NULL);
    pthread_join(rec_th, NULL);

    close(fd);

    return 0;
}
