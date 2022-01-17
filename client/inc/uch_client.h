#ifndef UCH_CLIENT_H
#define UCH_CLIENT_H

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
#include <pthread.h>

#include "../../libmx/inc/libmx.h"
#include "../../frameworks/SQLite3/inc/sqlite3.h"
#include "../../utils/inc/utils.h"

#include <gtk/gtk.h>

extern t_client cur_client;

void clear_message(char *mes, const int size);

struct
{
    GtkWidget *LOGIN_menu;
    GtkWidget *REGISTRATION_menu;
}   t_auth;

struct
{
    GtkWidget *main_window;
    GtkCssProvider *provider;
} t_screen;


void chat_show_reg_screen();
void load_css_auth(GtkCssProvider *provider, GtkWidget *widget, gint widg);
void chat_show_auth_screen();
void send_login(GtkWidget *widget, gpointer data);

#endif
