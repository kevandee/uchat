#include "../inc/uch_client.h"

void on_dark_theme() {
    cur_client.theme = DARK_THEME;
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/dark_chat.css");
}

void on_light_theme() {
    cur_client.theme = LIGHT_THEME;
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/light.css");
}
