#include "../inc/uch_client.h"

static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

void chat_show_main_screen(GtkWidget *window) 
{
    GtkWidget *main_box;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(main_box), 323, 600);
    gtk_widget_set_halign(GTK_WIDGET(main_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(main_box), GTK_ALIGN_END);
    gtk_widget_set_name(GTK_WIDGET(main_box), "left_panel");
    load_css_main(t_screen.provider, main_box);
    GtkWidget *Logo, *SearchBox, *SearchField, *Home, *Settings;
//-----------------------------------------------SearcPanel---------------------------------------------------------------
    t_main.search_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(GTK_WIDGET(t_main.search_panel), 40);
    SearchBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);

    Logo = get_circle_widget_from_png("test_circle.png");
    gtk_widget_set_size_request(GTK_WIDGET(Logo), 10, 10);
    SearchField = gtk_entry_new();
    gtk_widget_set_name(GTK_WIDGET(SearchField), "search_field");
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER (gtk_entry_get_buffer(GTK_ENTRY(SearchField))), 20);
    gtk_widget_set_size_request(SearchField, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(SearchField), "Search");
    load_css_main(t_screen.provider, SearchField);
    gtk_box_append(GTK_BOX(SearchBox), GTK_WIDGET(SearchField));
    GFile *path = g_file_new_for_path("client/media/search_ico.png"); 
    GIcon *icon = g_file_icon_new(path);
    gtk_entry_set_icon_from_gicon(GTK_ENTRY(SearchField),GTK_ENTRY_ICON_SECONDARY, icon);
    Home = gtk_image_new_from_file("client/media/home.png");
    gtk_widget_set_size_request(GTK_WIDGET(Home), 10, 10);
    Settings = gtk_image_new_from_file("client/media/settings.png");
    gtk_widget_set_size_request(GTK_WIDGET(Settings), 10, 10);
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Logo));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(SearchBox));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Home));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Settings));
    gtk_box_set_spacing (GTK_BOX(t_main.search_panel), 8);
//------------------------------------------------------------------------------------------------------------------------
    
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

    gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(t_main.search_panel));
    gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(scrolled_window));
    gtk_box_set_spacing (GTK_BOX(main_box), 0);

    gtk_window_set_child(GTK_WINDOW(window), main_box);

    gtk_widget_show(window);
}
