/* xkb-callbacks.c
 *
 * Copyright (C) 2016 Dmitriy Poltavchenko <admin@linuxhub.ru>
 *
 * Copyright (C) 2008 Alexander Iliev <sasoiliev@mamul.org>
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

#include "xkb-callbacks.h"
#include "xkb-util.h"

void
xkb_active_window_changed( WnckScreen *screen,
                           WnckWindow *previously_active_window,
                           t_xkb_settings *xkb )
{
    WnckWindow *window;
    guint window_id, application_id;

    window = wnck_screen_get_active_window( screen );

    if( !WNCK_IS_WINDOW( window ) )
        return;

    window_id = wnck_window_get_xid( window );
    application_id = wnck_window_get_pid( window );

    xkb_config_window_changed( window_id, application_id );
}

void
xkb_application_closed( WnckScreen *screen,
                        WnckApplication *app,
                        t_xkb_settings *xkb )
{
    guint application_id;

    application_id = wnck_application_get_pid( app );

    xkb_config_application_closed( application_id );
}

void
xkb_window_closed( WnckScreen *screen,
                   WnckWindow *window,
                   t_xkb_settings *xkb )
{
    guint window_id;

    window_id = wnck_window_get_xid( window );

    xkb_config_window_closed( window_id );
}

void
xkb_about( void )
{
    /* This helps prevent multiple instances */
    if( !gtk_grab_get_current() ) {
        /* Create the about dialog */
        GtkWidget* about_dialog = gtk_about_dialog_new();
        gtk_window_set_icon(
            (GtkWindow*)about_dialog,
            gtk_widget_render_icon(about_dialog, GTK_STOCK_ABOUT, GTK_ICON_SIZE_DIALOG, NULL)
        );

        const gchar* authors[] = { AUTHORS, NULL };
        gtk_about_dialog_set_authors( (GtkAboutDialog*)about_dialog, authors );
        gtk_about_dialog_set_name( (GtkAboutDialog*)about_dialog, PACKAGE );
        gtk_about_dialog_set_version( (GtkAboutDialog*)about_dialog, VERSION );
        gtk_about_dialog_set_comments( (GtkAboutDialog*)about_dialog, DESCRIPTION );
        gtk_about_dialog_set_website( (GtkAboutDialog*)about_dialog, PACKAGE_URL );
        gtk_about_dialog_set_copyright( (GtkAboutDialog*)about_dialog, COPYRIGHT );

        GdkPixbuf *pixmap = gdk_pixbuf_new_from_file( APPICON, NULL );
        if( pixmap )
            gtk_about_dialog_set_logo( (GtkAboutDialog*)about_dialog, pixmap );

        /* Run the about dialog */
        gtk_dialog_run( (GtkDialog*)about_dialog );
        gtk_widget_destroy( about_dialog );
    } else {
        /* A window is already open, so we present it to the user */
        GtkWidget *toplevel = gtk_widget_get_toplevel( gtk_grab_get_current() );
        gtk_window_present( (GtkWindow*)toplevel );
    }
}

void
xkb_main_quit( void )
{
    /* Prevent quit with dialogs open */
    if( !gtk_grab_get_current() ) {
        /* Quit the program */
        gtk_main_quit();
    } else {
        /* A window is already open, so we present it to the user */
        GtkWidget *toplevel = gtk_widget_get_toplevel( gtk_grab_get_current() );
        gtk_window_present( (GtkWindow*)toplevel );
    }
}
