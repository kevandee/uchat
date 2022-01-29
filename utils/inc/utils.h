#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include "../../libmx/inc/libmx.h"

typedef struct s_avatar {
    double d_width;

    double orig_w;
    double orig_h;

    double scaled_w;
    double scaled_h;
    
    double x;
    double y;

    char *name;
    char *path;
} t_avatar;

typedef struct s_chat {
    int id;
    char name[256];
    int count_users;

    t_list *users;
    t_list *users_avatars;
    t_list *messages;
}              t_chat;

typedef struct s_message {
    int id;
    int c_id;
    
    char sender[32];
    char type[32];
    char data[4096];
}              t_message;

typedef struct s_client {
    int serv_fd;
    
    struct sockaddr_in adr;
    int cl_socket;

    int id;
    char *login;
    char *passwd;

    t_avatar avatar;
    char name[32];
    char surname[32];
    char bio[256];

    int chat_count;
    t_list *chats;

    int id_cur_chat; // id чата в бд
    t_chat cur_chat;
    bool sender_new_chat;
}              t_client;

void send_image(int socket, char *file);
int recv_image(int socket, char *path);

int send_all(int sockfd, const char *buf, int len);
int recv_all(int sockfd, char * buf, int len);

void clear_message(char *mes, const int size);

#endif
