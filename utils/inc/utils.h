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

#include <gtk/gtk.h>
#include <openssl/ssl.h>

typedef struct s_avatar {
    double d_width;

    double orig_w;
    double orig_h;
    cairo_surface_t *image;
    double scaled_w;
    double scaled_h;
    double prev_w;
    double prev_h;
    
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

    int last_mes_id;
    bool is_enter;
    
    bool is_new; // для сервера
    bool mute;
    bool notify;
    bool is_avatar;
    t_avatar avatar;
}              t_chat;

typedef struct s_message {
    int id;
    int c_id;
    bool prev;

    bool edited;
    char time[32];
    char sender[32];
    char type[32];
    char data[4096];
}              t_message;

typedef struct s_client {
    int serv_fd;

    SSL *ssl;// client ssl structure with coneection to server
    
    struct sockaddr_in adr;
    int cl_socket;

    int id;
    char *login;
    char *passwd;

    t_avatar avatar;
    cairo_surface_t *scaled_for_chat;
    char name[32];
    char surname[32];
    char bio[256];

    int theme;

    int chat_count;
    t_list *chats;

    int id_cur_chat; // id чата в бд
    t_chat cur_chat;
    bool sender_new_chat;
}              t_client;

void send_image(SSL *socket, char *file);
int recv_image(SSL *socket, char *path);

int send_all(SSL *sockfd, char *buf, int len);
int recv_all(SSL *sockfd, char * buf, int len);

void clear_message(char *mes, const int size);

void send_file(SSL *socket, char *file, const char *mode);
int recv_file(SSL *socket, char *path, const char *mode) ;

#endif
