#include "../inc/uch_client.h"

static void load_css_headerbar(GtkWidget *headerbar)
{
    GtkStyleContext *context = gtk_widget_get_style_context(headerbar);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(t_screen.provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

static void on_mouse() {
    printf("bebra\n");
}

void chat_decorate_headerbar()
{
    GtkGesture *click_close = gtk_gesture_click_new();
    gtk_gesture_set_state(click_close, GTK_EVENT_SEQUENCE_CLAIMED);
    GtkGesture *click_minimize = gtk_gesture_click_new();
    gtk_gesture_set_state(click_close, GTK_EVENT_SEQUENCE_CLAIMED);

    t_screen.headerbar = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons (GTK_HEADER_BAR( t_screen.headerbar), FALSE);
    GtkWidget *headerbar_button1 = gtk_image_new_from_file("client/media/close.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button1), "close");
    g_signal_connect_swapped(click_close, "pressed", G_CALLBACK(gtk_window_destroy), t_screen.main_window);
    gtk_widget_add_controller(headerbar_button1, GTK_EVENT_CONTROLLER(click_close));
    gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button1);
    GtkWidget *headerbar_button2 = gtk_image_new_from_file("client/media/minimize.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button2), "minimize");
    g_signal_connect_swapped(click_minimize, "pressed", G_CALLBACK(gtk_window_minimize), t_screen.main_window);
    gtk_widget_add_controller(headerbar_button2, GTK_EVENT_CONTROLLER(click_minimize));
    gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button2);
    GtkWidget *headerbar_button3 = gtk_image_new_from_file("client/media/notworking.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button3), "maximize");
    gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button3);
    load_css_headerbar(t_screen.headerbar);
    load_css_headerbar(headerbar_button1);
    load_css_headerbar(headerbar_button2);
    load_css_headerbar(headerbar_button3);
    gtk_window_set_titlebar (GTK_WINDOW(t_screen.main_window), t_screen.headerbar);

    GtkEventController *ev = gtk_event_controller_motion_new();
    g_signal_connect(ev, "enter", G_CALLBACK(on_mouse), NULL); // "leave" когда курсор покидает область виджета
    gtk_widget_add_controller(headerbar_button1,  GTK_EVENT_CONTROLLER(ev));
    gtk_widget_add_controller(headerbar_button2,  GTK_EVENT_CONTROLLER(ev));
}

