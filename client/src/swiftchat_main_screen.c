#include "../inc/uch_client.h"


void chat_show_main_screen(GtkWidget *window) {
    GtkWidget *main_box = NULL;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(main_box), 323, 600);
    gtk_widget_set_halign(GTK_WIDGET(main_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(main_box), GTK_ALIGN_END);
    
    GtkWidget *scrolled_window = gtk_scrolled_window_new ();
    gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 323, 600);
    t_main.scroll_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box), 323, 0);
    
    t_chat pop_smoke;
    mx_strcpy(pop_smoke.name, "Pop Smoke");
    t_list *chats = cur_client.chats;
    while (chats) {
        add_chat_node(chats->data);
        chats = chats->next;
    }
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window),t_main.scroll_box);

    gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(scrolled_window));
    gtk_box_set_spacing (GTK_BOX(main_box), 0);

    gtk_window_set_child(GTK_WINDOW(window), main_box);

    gtk_widget_show(window);
}
