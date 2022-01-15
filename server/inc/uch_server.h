#ifndef UCH_SERVER_H
#define UCH_SERVER_H

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
#include "../../frameworks/SQLite3/inc/sqlite3.h"
#include "../../utils/inc/utils.h"

#include <pthread.h>
#include <curl/curl.h>


extern t_list *users_list;
extern pthread_mutex_t send_mutex;

void send_message(char *mes, char *sender, t_chat *chat);
void clear_message(char *mes, const int size);
void send_new_chat(t_chat **new_chat);

void free_client(t_client **client, t_list **users_list);
t_client *get_client_by_name(char *name);

char *get_weather(char *city);

void sqlite3_create_db();
void *sqlite3_exec_db(char *query, int type);

#endif
