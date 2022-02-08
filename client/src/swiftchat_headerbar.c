#include "../inc/uch_client.h"

static void load_css_headerbar(GtkWidget *headerbar)
{
    GtkStyleContext *context = gtk_widget_get_style_context(headerbar);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(t_screen.provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

static void enter_mouse(GtkWidget *widget, gpointer data);

static void leave_mouse(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget **button_box =  (data);

    gtk_widget_hide(button_box[1]);
    gtk_widget_show(button_box[0]);

    GtkEventController *ev_enter = gtk_event_controller_motion_new();
    g_signal_connect_after(ev_enter, "enter", G_CALLBACK(enter_mouse), data); // "leave" когда курсор покидает область виджета
    gtk_widget_add_controller(button_box[0],  GTK_EVENT_CONTROLLER(ev_enter));
}

static void enter_mouse(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget **button_box =  (data);

    gtk_widget_hide(button_box[0]);
    gtk_widget_show(button_box[1]);
}
static void close_app() {
    t_main.is_run = false;
    gtk_window_destroy(GTK_WINDOW (t_screen.main_window));
    
}

void chat_decorate_headerbar()
{
    GtkGesture *click_close = gtk_gesture_click_new();
    gtk_gesture_set_state(click_close, GTK_EVENT_SEQUENCE_CLAIMED);
    GtkGesture *click_minimize = gtk_gesture_click_new();
    gtk_gesture_set_state(click_close, GTK_EVENT_SEQUENCE_CLAIMED);

    t_screen.headerbar = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons (GTK_HEADER_BAR( t_screen.headerbar), FALSE);

    GtkWidget *buttons_box_enter = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 7);

    GtkWidget *headerbar_button1 = gtk_image_new_from_file("client/media/close.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button1), "close");
    //gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button1);
    gtk_box_append(GTK_BOX(buttons_box_enter), headerbar_button1);
    GtkWidget *headerbar_button2 = gtk_image_new_from_file("client/media/minimize.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button2), "minimize");
   
    //gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button2);
    gtk_box_append(GTK_BOX(buttons_box_enter), headerbar_button2);
    
    GtkWidget *headerbar_button3 = gtk_image_new_from_file("client/media/inactive.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button3), "maximize");
    //gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button3);
    gtk_box_append(GTK_BOX(buttons_box_enter), headerbar_button3);

    GtkWidget *buttons_box_leave = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 7);

    GtkWidget *headerbar_button_leave1 = gtk_image_new_from_file("client/media/close_hover.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button_leave1), "close");
    g_signal_connect_swapped(click_close, "pressed", G_CALLBACK(close_app), NULL);
    gtk_widget_add_controller(headerbar_button_leave1, GTK_EVENT_CONTROLLER(click_close));
    //gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button1);
    gtk_box_append(GTK_BOX(buttons_box_leave), headerbar_button_leave1);
    GtkWidget *headerbar_button_leave2 = gtk_image_new_from_file("client/media/minimize_hover.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button_leave2), "minimize");
    g_signal_connect_swapped(click_minimize, "pressed", G_CALLBACK(gtk_window_minimize), t_screen.main_window);
    gtk_widget_add_controller(headerbar_button_leave2, GTK_EVENT_CONTROLLER(click_minimize));
    //gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button2);
    gtk_box_append(GTK_BOX(buttons_box_leave), headerbar_button_leave2);
    GtkWidget *headerbar_button_leave3 = gtk_image_new_from_file("client/media/inactive.png");
    gtk_widget_set_name(GTK_WIDGET(headerbar_button_leave3), "maximize");
    //gtk_header_bar_pack_start (GTK_HEADER_BAR( t_screen.headerbar), headerbar_button3);
    gtk_box_append(GTK_BOX(buttons_box_leave), headerbar_button_leave3);

    gtk_header_bar_pack_start(GTK_HEADER_BAR( t_screen.headerbar), buttons_box_enter);
    gtk_header_bar_pack_start(GTK_HEADER_BAR( t_screen.headerbar), buttons_box_leave);
    gtk_widget_hide(buttons_box_leave);    
    load_css_headerbar(t_screen.headerbar);
    load_css_headerbar(headerbar_button1);
    load_css_headerbar(headerbar_button2);
    load_css_headerbar(headerbar_button3);
    load_css_headerbar(headerbar_button_leave1);
    load_css_headerbar(headerbar_button_leave2);
    load_css_headerbar(headerbar_button_leave3);
    gtk_window_set_titlebar (GTK_WINDOW(t_screen.main_window), t_screen.headerbar);

    GtkWidget **box_arr = (GtkWidget **)malloc(2 * sizeof(GtkWidget *));
    box_arr[0] = buttons_box_enter;
    box_arr[1] = buttons_box_leave;

    GtkEventController *ev_enter = gtk_event_controller_motion_new();
    GtkEventController *ev_leave = gtk_event_controller_motion_new();
    g_signal_connect_after(ev_enter, "enter", G_CALLBACK(enter_mouse), box_arr); // "leave" когда курсор покидает область виджета
    
    g_signal_connect_after(ev_leave, "leave", G_CALLBACK(leave_mouse), box_arr); // "leave" когда курсор покидает область виджета
    gtk_widget_add_controller(buttons_box_enter,  GTK_EVENT_CONTROLLER(ev_enter));
    gtk_widget_add_controller(buttons_box_leave,  GTK_EVENT_CONTROLLER(ev_leave));

    
}

