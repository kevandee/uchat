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
extern pthread_mutex_t cl_mutex;

void clear_message(char *mes, const int size);

struct
{
    GtkWidget *grid;
    GtkWidget *left_panel;
    GtkWidget *search_panel;
    GtkWidget *logo;
    GtkWidget *scroll_box_left;
    GtkWidget *scrolled_window_left;
    GtkWidget *right_panel;
    GtkWidget *scroll_box_right;
    GtkWidget *scrolled_window_right;
    double adj_pos;
    GtkWidget *sticker_panel;
    GtkWidget *sticker_scroll_box;
    GtkWidget *sticker_scroll_window;

    t_list *search_users_list;
    bool loaded;
    bool drawed;
    bool scroll_mes;
    t_list *check_buttons_user_list;
    t_avatar default_avatar;
    t_avatar default_group_avatar;
    t_avatar *loaded_avatar;
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

struct
{ 
    GtkWidget *home;
    GtkWidget *settings;
} t_actives;

void chat_show_reg_screen();
void chat_show_auth_screen();
void chat_decorate_headerbar();
void show_settings();
void show_chat_history(GtkWidget *widget, gpointer data);
void return_to_chatlist(GtkWidget *widget, gpointer data);
void add_chat_dialog(GtkWidget *widget, gpointer data);

void *scroll_func();
void *save_scroll_func();

void send_login(GtkWidget *widget, gpointer data);
void chat_show_main_screen(GtkWidget *window);
gboolean add_chat_node(gpointer data);
int check_auth_input(const char *str);

void text_changed_main_screen(GObject *object, GParamSpec *pspec, gpointer data);
void text_changed_add_chat(GObject *object, GParamSpec *pspec, gpointer data);

cairo_surface_t *get_surface_from_jpg(const char *filename);
cairo_surface_t *scale_to_half(cairo_surface_t *s, int orig_width, int orig_height, int scaled_width, int scaled_height);
GtkWidget *get_circle_widget_from_png(const char *filename);
GtkWidget *get_circle_widget_from_png_custom(const char *filename, gint width, gint height);
GtkWidget *get_circle_widget_current_user_avatar();

GtkWidget *get_circle_widget_from_png_avatar(t_avatar *avatar, gint width, gint height, bool must_update);
t_list *get_chat_users_avatars(t_chat *chat);


void get_all_user_data();
void get_messages_from_server(int c_id, int mes_id);
t_avatar *get_avatar(t_avatar *avatar);

void create_user_db(t_client cur_client);
void insert_user_db(t_client cur_client);
void *user_exec_db(char *login, char *query, int type);
char *get_db_name(char *login);

#endif
