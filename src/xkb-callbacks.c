/* vim: set backspace=2 ts=4 softtabstop=4 sw=4 cinoptions=>4 expandtab autoindent smartindent: */
/* xkb-callbacks.c
 *
 * Copyright (C) 2014 Dmitriy Poltavchenko <admin@linuxhub.ru>
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
xkb_about( t_xkb_settings *xkb )
{
    GtkWidget *about_dialog = gtk_message_dialog_new(
        NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_CLOSE,
        "%s\nX11 Keyboard switcher\nAuthor: Dmitriy Poltavchenko <%s>",
        PACKAGE_STRING,
        PACKAGE_BUGREPORT
    );

    gtk_window_set_title(
        GTK_WINDOW( about_dialog ),
        g_strconcat(
            "About ",
            PACKAGE_NAME,
            NULL
        )
    );

    /* Destroy the dialog when the user responds to it (e.g. clicks a button) */
    g_signal_connect_swapped(
        about_dialog,
        "response",
        G_CALLBACK( gtk_widget_hide ),
        about_dialog
    );

    gtk_widget_show( about_dialog );
}

