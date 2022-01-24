#include "../inc/uch_client.h"

static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

void add_chat_node(t_chat *chat) {
    GtkWidget *child_widget = gtk_button_new ();
    gtk_widget_set_size_request(child_widget, 246, 54);
    GtkWidget *chat_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(chat_info), "scroll_buttons");
    load_css_main(t_screen.provider, chat_info);

    GtkWidget *chat_image = get_circle_widget_from_png_custom("test_circle.png", 57, 57);
    gtk_widget_set_size_request(GTK_WIDGET(chat_image),  57, 0);
    gtk_widget_set_halign(GTK_WIDGET(chat_image), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(chat_image), GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX(chat_info), chat_image);

    GtkWidget *chat_name = gtk_label_new(chat->name);
    gtk_widget_set_size_request(GTK_WIDGET(chat_name), 0, 0);
    gtk_widget_set_halign(GTK_WIDGET(chat_name), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(chat_name), GTK_ALIGN_CENTER);
    
    gtk_box_append (GTK_BOX(chat_info), chat_name);

    gtk_button_set_child(GTK_BUTTON (child_widget), chat_info);
    //переход в историю чатов, в файле мейн скрин

    g_signal_connect(child_widget, "clicked", G_CALLBACK(show_chat_history), chat);
    gtk_box_append(GTK_BOX(t_main.scroll_box_left), child_widget);
}
