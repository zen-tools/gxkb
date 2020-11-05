/* statusicon.c
 *
 * Copyright (C) 2016 Dmitriy Poltavchenko <poltavchenko.dmitriy@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "statusicon.h"

GtkWidget       *lb_mouse_popup;
GtkWidget       *rb_mouse_popup;
#ifdef HAVE_APPINDICATOR
AppIndicator    *appindicator;
#endif
GtkStatusIcon   *trayicon;
GHashTable      *icon_cache;
statusicon_type icon_type;

void
statusicon_new( void )
{
    #ifdef HAVE_APPINDICATOR
        appindicator = app_indicator_new(
            "example-simple-client",
            "indicator-messages",
            APP_INDICATOR_CATEGORY_APPLICATION_STATUS
        );
        icon_type = APPINDICATOR;
        app_indicator_set_status( appindicator, APP_INDICATOR_STATUS_ACTIVE );
        APP_INDICATOR_GET_CLASS( appindicator )->fallback   = appindicator_fallback;
        APP_INDICATOR_GET_CLASS( appindicator )->unfallback = appindicator_unfallback;
        g_signal_connect( G_OBJECT( appindicator ), "scroll-event",
                          G_CALLBACK( appindicator_icon_scrolled ), NULL );
    #else
        trayicon  = gtk_status_icon_new();
        icon_type = SYSTRAY;

        g_signal_connect( G_OBJECT( trayicon ), "activate"    ,
                          G_CALLBACK( gtk_status_icon_clicked    ), NULL );

        g_signal_connect( G_OBJECT( trayicon ), "scroll-event",
                          G_CALLBACK( gtk_status_icon_scrolled   ), NULL );

        g_signal_connect( G_OBJECT( trayicon ), "popup-menu"  ,
                          G_CALLBACK( gtk_status_icon_popup_menu ), NULL );
    #endif

    icon_cache = g_hash_table_new_full( g_str_hash,
                                        g_str_equal,
                                        g_free,
                                        g_object_unref );

    statusicon_update_menu();
    statusicon_update_current_image();
}

void
gtk_status_icon_clicked( GtkStatusIcon *status_icon, gpointer data )
{
    if( status_icon == NULL )
        return;

    if( xkb_config_get_group_count() > 2 )
    {
        gtk_menu_popup_at_pointer( GTK_MENU( lb_mouse_popup ), NULL );
    }
    else
    {
        xkb_config_next_group();
    }
}

gboolean
gtk_status_icon_scrolled( GtkStatusIcon  *status_icon,
                          GdkEventScroll *event,
                          gpointer data )
{
    if( status_icon == NULL )
        return FALSE;

    switch( event->direction )
    {
        case GDK_SCROLL_UP:
        case GDK_SCROLL_RIGHT:
            xkb_config_prev_group();
            return TRUE;
        case GDK_SCROLL_DOWN:
        case GDK_SCROLL_LEFT:
            xkb_config_next_group();
            return TRUE;
        default:
            return FALSE;
    }

    return FALSE;
}

void
gtk_status_icon_popup_menu( GtkStatusIcon *status_icon, guint button,
                            guint activate_time, gpointer data )
{
    if( status_icon == NULL )
        return;

    gtk_menu_popup_at_pointer( GTK_MENU( rb_mouse_popup ), NULL );
}

void
statusicon_set_group( GtkWidget *item, gpointer data )
{
    gint group = GPOINTER_TO_INT( data );
    xkb_config_set_group( group );
}

void
statusicon_update_current_image( void )
{
    const gchar *group_name = xkb_config_get_group_name( -1 );
    gchar *filepath = xkb_util_get_flag_filename( group_name );

    if( icon_type == SYSTRAY )
    {
        if( trayicon == NULL )
            return;

        GdkPixbuf *pixmap;
        if( !g_hash_table_lookup_extended( icon_cache, filepath, NULL, (gpointer) &pixmap ) )
        {
            pixmap = gdk_pixbuf_new_from_file( filepath, NULL );
            g_hash_table_insert (icon_cache, g_strdup (filepath), pixmap);
        }

        if( !pixmap )
        {
            g_fprintf( stderr, "Can't load image from %s\n", filepath );
            return;
        }

        gtk_status_icon_set_from_pixbuf( trayicon, pixmap );
        gtk_status_icon_set_tooltip_text( trayicon, g_ascii_strup( group_name, -1 ) );
    }
    else if( icon_type == APPINDICATOR )
    {
        #ifdef HAVE_APPINDICATOR
        app_indicator_set_icon( appindicator, filepath );
        #endif
    }
}

void
statusicon_update_menu( void )
{
    // Left button click menu
    gint i;

    statusicon_destroy_menu( lb_mouse_popup );

    lb_mouse_popup = gtk_menu_new();

    gint width, height;
    gtk_icon_size_lookup( GTK_ICON_SIZE_MENU, &width, &height );

    for( i = 0; i < xkb_config_get_group_count(); i++ )
    {
        gchar *layout_string = NULL;

        gchar *imgfilename = xkb_util_get_flag_filename( xkb_config_get_group_name( i ) );
        GdkPixbuf *handle = gdk_pixbuf_new_from_file_at_scale( imgfilename, width, height, TRUE, NULL );
        g_free( imgfilename );

        layout_string = xkb_util_get_layout_string( xkb_config_get_group_name( i ),
                                                    xkb_config_get_variant( i ) );

        layout_string = g_ascii_strup( layout_string, -1 );

        GtkWidget *menu_item = gtk_image_menu_item_new_with_label( layout_string );
        g_free( layout_string );

        g_signal_connect( G_OBJECT( menu_item ), "activate",
                          G_CALLBACK( statusicon_set_group ), GINT_TO_POINTER( i ) );

        if( handle )
        {
            GtkWidget *image = gtk_image_new();

            gtk_image_set_from_pixbuf( GTK_IMAGE( image ), handle );
            gtk_widget_show( image );
            gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM( menu_item ), image );

            g_object_unref( handle );
        }

        gtk_widget_show( menu_item );

        gtk_menu_shell_append( GTK_MENU_SHELL( lb_mouse_popup ), menu_item );
    }


    // Right button click menu
    GtkWidget *mi = NULL;

    statusicon_destroy_menu( rb_mouse_popup );

    if( icon_type == APPINDICATOR )
    {
        // Separator
        mi = gtk_separator_menu_item_new();
        gtk_widget_show( mi );
        gtk_menu_shell_append( GTK_MENU_SHELL( lb_mouse_popup ), mi );
        gtk_widget_set_sensitive( mi, FALSE );
        rb_mouse_popup = lb_mouse_popup;
    }
    else if( icon_type == SYSTRAY )
    {
        rb_mouse_popup = gtk_menu_new();
    }

    mi = gtk_image_menu_item_new_from_stock( "gtk-about", NULL );
    g_signal_connect( G_OBJECT( mi ), "activate", (GCallback)xkb_about, NULL );
    gtk_menu_shell_append( GTK_MENU_SHELL( rb_mouse_popup ), mi );
    gtk_widget_show( mi );

    if( icon_type == SYSTRAY )
    {
        // Separator
        mi = gtk_separator_menu_item_new();
        gtk_widget_show( mi );
        gtk_menu_shell_append( GTK_MENU_SHELL( rb_mouse_popup ), mi );
        gtk_widget_set_sensitive( mi, FALSE );
    }

    mi = gtk_image_menu_item_new_from_stock( "gtk-quit", NULL );
    g_signal_connect( G_OBJECT( mi ), "activate", (GCallback)xkb_main_quit, NULL );
    gtk_menu_shell_append( GTK_MENU_SHELL( rb_mouse_popup ), mi );
    gtk_widget_show( mi );

    #ifdef HAVE_APPINDICATOR
    if( icon_type == APPINDICATOR )
        app_indicator_set_menu( appindicator, GTK_MENU( rb_mouse_popup ) );
    #endif
}

void
statusicon_destroy_menu( GtkWidget *menu )
{
    if( menu )
    {
        gtk_widget_destroy( menu );
        g_object_ref_sink( menu );
        g_object_unref( menu );
    }
}

void
statusicon_free( void )
{
    statusicon_destroy_menu( rb_mouse_popup );
    statusicon_destroy_menu( lb_mouse_popup );

    g_hash_table_destroy( icon_cache );

    if( trayicon )
        g_object_unref( trayicon );
}

#ifdef HAVE_APPINDICATOR
GtkStatusIcon *
appindicator_fallback( AppIndicator *indicator )
{
    icon_type = SYSTRAY;

    if( trayicon )
    {
        gtk_status_icon_set_visible( trayicon, TRUE );
        statusicon_update_menu();
        statusicon_update_current_image();
        return trayicon;
    }

    trayicon = gtk_status_icon_new();

    g_signal_connect( G_OBJECT( trayicon ), "activate"    ,
                      G_CALLBACK( gtk_status_icon_clicked    ), NULL );

    g_signal_connect( G_OBJECT( trayicon ), "scroll-event",
                      G_CALLBACK( gtk_status_icon_scrolled   ), NULL );

    g_signal_connect( G_OBJECT( trayicon ) , "popup-menu" ,
                      G_CALLBACK( gtk_status_icon_popup_menu ), NULL );

    statusicon_update_menu();
    statusicon_update_current_image();

    return trayicon;
}

void
appindicator_unfallback( AppIndicator *indicator, GtkStatusIcon *status_icon )
{
    gtk_status_icon_set_visible( status_icon, FALSE );
    icon_type = APPINDICATOR;
    statusicon_update_menu();
    statusicon_update_current_image();
}

void
appindicator_icon_scrolled( AppIndicator * indicator, gint delta,
                            GdkScrollDirection direction, gpointer user_data )
{
    switch( direction )
    {
        case GDK_SCROLL_UP:
        case GDK_SCROLL_RIGHT:
            xkb_config_prev_group();
        break;
        case GDK_SCROLL_DOWN:
        case GDK_SCROLL_LEFT:
            xkb_config_next_group();
        break;
    }
}

#endif
