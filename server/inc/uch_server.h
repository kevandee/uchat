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

#include <openssl/ssl.h>
#include <openssl/err.h>


extern t_list *users_list;
extern pthread_mutex_t send_mutex;

void send_all_user_data(t_client *client);

void send_message(char *mes, char *sender, t_chat *chat);
void clear_message(char *mes, const int size);
void send_new_chat(t_chat *new_chat);
void change_chat_by_id(int id, t_client *cur);

void free_client(t_client **client, t_list **users_list);
t_client *get_client_by_name(char *name);

char *get_weather(char *city);

void sqlite3_create_db();
void *sqlite3_exec_db(char *query, int type);
int get_user_id(char *login);
char *get_user_login(int id);
int get_chat_id(char *name);
int get_chat_members(int id);
t_list *get_chat_users(int c_id);
t_chat *chat_info (int c_id);
t_client *get_user_info(int id);
t_avatar *parse_avatar_info(char *avatar_info);
char *get_user_avatar(int id);
t_list *db_messages_sender(int c_id);

//SSling
SSL_CTX *CTX_initialize_server();
EVP_PKEY *create_key();
X509 *create_X509(EVP_PKEY *pkey);
void write_certs(EVP_PKEY *pkey, X509 *x509);
void load_certs(SSL_CTX* context, char* cert_name, char* key_name);
int open_server_connection(int port, struct sockaddr_in *adr, socklen_t adrlen);
void certificate_ckeck(SSL *ssl);
void close_connection(SSL *ssl);
//SSLing
#endif
