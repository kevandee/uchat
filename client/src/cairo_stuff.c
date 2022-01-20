#include "../inc/uch_client.h"

cairo_surface_t *get_surface_from_jpg(const char *filename) {
        GdkPixbuf *pixbuf;
        gint width;
        gint height;
        cairo_format_t format;
        cairo_surface_t *surface;
        cairo_t *cr;
        pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
        g_assert (pixbuf != NULL);
        format = (gdk_pixbuf_get_has_alpha (pixbuf)) ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;
        width = gdk_pixbuf_get_width (pixbuf);
        height = gdk_pixbuf_get_height (pixbuf);
        surface = cairo_image_surface_create (format, width, height);
        g_assert (surface != NULL);
        cr = cairo_create (surface);
        /* Draw the pixbuf */
        gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
        cairo_paint (cr);
       
        /* Save to a PNG file */
        cairo_surface_write_to_png (surface, "output.png");
        cairo_destroy (cr);
        return surface;
}

cairo_surface_t *scale_to_half(cairo_surface_t *s, int orig_width, int orig_height, int scaled_width, int scaled_height) { 
    double param1 = orig_width/scaled_width * orig_width/orig_height;
    if (param1 == 1) {
        param1 = orig_width/scaled_width;
    }
    //printf("param %f\n", param1);
    double param2 = orig_height/scaled_height * orig_width/orig_height;
    if (param2 == 1) {
        param2 = orig_height/scaled_height;
    }
    cairo_surface_t *result = cairo_surface_create_similar(s, cairo_surface_get_content(s), orig_width/param1, orig_height/param2); 
    cairo_t *cr = cairo_create(result); 
    cairo_scale(cr, 1/param1, 1/param2); 
    cairo_set_source_surface(cr, s, 0, 0); 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE); 
    cairo_paint(cr); 
    cairo_destroy(cr); 
    return result; 
}


/*
    GtkEventController *keys_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(t_screen.main_window, keys_controller);
    
   // draw circle frow image

    GtkWidget *darea = NULL;
    gint width, height;
    
    cairo_surface_t *image = get_surface_from_jpg("test.jpeg");//cairo_image_surface_create_from_png("test_circle.png");
    //cairo_surface_t *scaled_image = cairo_image_surface_create_for_data(cairo_image_surface_get_data(image), CAIRO_FORMAT_RGB24, 64, 64, cairo_image_surface_get_stride(image));
    width = cairo_image_surface_get_width(image);
    height = cairo_image_surface_get_height(image);
    printf("%d %d\n", width, height);
    
    cairo_surface_t *scaled_image = scale_to_half(image, width, height, 128, 128);
    width = cairo_image_surface_get_width(scaled_image);
    height = cairo_image_surface_get_height(scaled_image);
    printf("%d %d\n", width, height);
    

    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 256);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 256);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle, scaled_image, NULL);
    gtk_widget_set_halign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);

    g_signal_connect(keys_controller, "key-pressed", G_CALLBACK(key_press_event), darea);
    GtkWidget *grid = gtk_grid_new();

    for (int i = 0; i < 9; i++) {
        //gtk_widget_set_valign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
        for (int j = 0; j < 5; j++) {
            darea = gtk_drawing_area_new();
            gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 128);
            gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 128);
            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle, scaled_image, NULL);
            
            gtk_grid_attach(GTK_GRID (grid), darea, i, j, 1, 1);   
        }
    }
    
    gtk_box_append (GTK_BOX(main_box), darea);
    */