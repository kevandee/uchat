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
    GtkWidget *scroll_box;
    GtkWidget *search_panel;
}   t_main;

struct
{
    GtkWidget *LOGIN_menu;
    GtkWidget *REGISTRATION_menu;
    GtkWidget *ErrorMessageLogin;
    GtkWidget *ErrorMessageRegister;
}   t_auth;

struct
{
    GtkWidget *main_window;
    GtkWidget *headerbar;
    GtkCssProvider *provider;
} t_screen;

void chat_show_reg_screen();
void chat_show_auth_screen();
void chat_decorate_headerbar();
void send_login(GtkWidget *widget, gpointer data);

void chat_show_main_screen(GtkWidget *window);
void add_chat_node(t_chat *chat);

int check_auth_input(const char *str);

cairo_surface_t *get_surface_from_jpg(const char *filename);
cairo_surface_t *scale_to_half(cairo_surface_t *s, int orig_width, int orig_height, int scaled_width, int scaled_height);

GtkWidget *get_circle_widget_from_png(const char *filename);

#endif
