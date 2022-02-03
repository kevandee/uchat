#include "../inc/uch_client.h"


void show_message_menu(gpointer data) {
    (void)data;
    printf("bebra\n");
    GMenu * menu = g_menu_new ();
    //char tmp[700];
    //sprintf (tmp, "app.delete-cc-binding::%d", 1);
    GMenuItem *item = g_menu_item_new("Delete", "app.close");
    //g_menu_append (menu, "_Delete", tmp);
    g_menu_append_item(menu, item);
    GtkPopoverMenu * popover_menu = GTK_POPOVER_MENU (gtk_popover_menu_new_from_model (G_MENU_MODEL (menu)));
    gtk_popover_set_has_arrow (GTK_POPOVER (popover_menu), false);
    gtk_widget_set_parent (GTK_WIDGET (popover_menu), GTK_WIDGET (data));
    gtk_popover_set_pointing_to (GTK_POPOVER (popover_menu), &(GdkRectangle){5, 5, 0, 0});
    gtk_popover_popup (GTK_POPOVER (popover_menu));
    gtk_popover_present (GTK_POPOVER (popover_menu));
}
