#include "../inc/uch_client.h"

void *scroll_func() {
    usleep(100000);

    GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(t_main.scrolled_window_right));
    gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), gtk_adjustment_get_upper(GTK_ADJUSTMENT(adj)));
    gtk_scrolled_window_set_vadjustment (GTK_SCROLLED_WINDOW(t_main.scrolled_window_right),GTK_ADJUSTMENT(adj));

    pthread_detach(pthread_self());
    return NULL;
}

    

void *save_scroll_func() {
    usleep(50000);
    
    gtk_widget_hide(t_main.scroll_box_right);
    GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(t_main.scrolled_window_right));

    gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), gtk_adjustment_get_upper(adj) - t_main.adj_pos);
    gtk_scrolled_window_set_vadjustment (GTK_SCROLLED_WINDOW(t_main.scrolled_window_right),GTK_ADJUSTMENT(adj));

    
    gtk_widget_show(t_main.scroll_box_right);


    pthread_detach(pthread_self());
    return NULL;
}
