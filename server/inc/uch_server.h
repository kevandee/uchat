#ifndef UCH_SERVER_H
#define UCH_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
//#include <sqlite3.h>

#include "../../libmx/inc/libmx.h"
#include "../../frameworks/SQLite3/inc/sqlite3.h"

#include <pthread.h>
#include <curl/curl.h>

typedef struct s_client {
    struct sockaddr_in adr;
    int cl_socket;

    int id;
    char *login;
    char *passwd;
}              t_client;

void clear_message(char *mes, const int size);

extern t_list *users_list;
extern pthread_mutex_t send_mutex;

void send_message(char *mes, char *sender);



char *get_weather(char *city);

#endif
