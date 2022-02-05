#include "../inc/uch_client.h"

void on_dark_theme() {
    cur_client.theme = DARK_THEME;
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/dark_chat.css");
    if(t_main.search_panel)
    {
        gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.home);
        t_actives.home = gtk_image_new_from_file("client/media/home.png");
        gtk_widget_set_name(GTK_WIDGET(t_actives.home), "home_icon");
        load_css_main(t_screen.provider, t_actives.home);
        GtkGesture *click_home = gtk_gesture_click_new();
        gtk_gesture_set_state(click_home, GTK_EVENT_SEQUENCE_CLAIMED);
        g_signal_connect_swapped(click_home, "pressed", G_CALLBACK(show_home), NULL);
        gtk_widget_add_controller(t_actives.home, GTK_EVENT_CONTROLLER(click_home));
        gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.home);

        gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.settings);
        t_actives.settings = gtk_image_new_from_file("client/media/setting_active.png");
        gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
        load_css_main(t_screen.provider, t_actives.settings);
        gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.settings);
    }
}

void on_light_theme() {
    cur_client.theme = LIGHT_THEME;
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/light.css");
    if(t_main.search_panel)
    {
        gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.home);
        t_actives.home = gtk_image_new_from_file("client/media/home_light.png");
        gtk_widget_set_name(GTK_WIDGET(t_actives.home), "home_icon");
        load_css_main(t_screen.provider, t_actives.home);
        GtkGesture *click_home = gtk_gesture_click_new();
        gtk_gesture_set_state(click_home, GTK_EVENT_SEQUENCE_CLAIMED);
        g_signal_connect_swapped(click_home, "pressed", G_CALLBACK(show_home), NULL);
        gtk_widget_add_controller(t_actives.home, GTK_EVENT_CONTROLLER(click_home));
        gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.home);

        gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.settings);
        t_actives.settings = gtk_image_new_from_file("client/media/setting_active.png");
        gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
        load_css_main(t_screen.provider, t_actives.settings);
        gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.settings);
    }

}
