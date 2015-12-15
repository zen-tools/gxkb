/* vim: set backspace=2 ts=4 softtabstop=4 sw=4 cinoptions=>4 expandtab autoindent smartindent: */
/* xkb-callbacks.c
 *
 * Copyright (C) 2013 Dmitriy Poltavchenko <zen@root.ua>
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
xkb_active_window_changed (WnckScreen *screen,
                           WnckWindow *previously_active_window,
                           t_xkb *xkb)
{
    WnckWindow *window;
    guint window_id, application_id;

    window = wnck_screen_get_active_window (screen);
    if (!WNCK_IS_WINDOW (window)) return;
    window_id = wnck_window_get_xid (window);
    application_id = wnck_window_get_pid (window);

    xkb_config_window_changed (window_id, application_id);
}

void
xkb_application_closed (WnckScreen *screen,
                        WnckApplication *app,
                        t_xkb *xkb)
{
    guint application_id;

    application_id = wnck_application_get_pid (app);

    xkb_config_application_closed (application_id);
}

void
xkb_window_closed (WnckScreen *screen,
                   WnckWindow *window,
                   t_xkb *xkb)
{
    guint window_id;

    window_id = wnck_window_get_xid (window);

    xkb_config_window_closed (window_id);
}

void
xkb_tray_icon_clicked (GtkStatusIcon *status_icon, gpointer data)
{
    if (xkb_config_get_group_count () > 2)
    {
        t_xkb *xkb = (t_xkb *) data;
        gtk_menu_popup(GTK_MENU (xkb->lb_mouse_popup),
                    NULL, NULL, gtk_status_icon_position_menu, status_icon,
                    0, gtk_get_current_event_time ());
    }
    else
    {
        xkb_config_next_group ();
    }
}

gboolean
xkb_tray_icon_scrolled (GtkStatusIcon *btn,
                        GdkEventScroll *event,
                        gpointer data)
{
    switch (event->direction)
    {
    case GDK_SCROLL_UP:
    case GDK_SCROLL_RIGHT:
        xkb_config_next_group ();
        return TRUE;
    case GDK_SCROLL_DOWN:
    case GDK_SCROLL_LEFT:
        xkb_config_prev_group ();
        return TRUE;
    default:
        return FALSE;
    }

    return FALSE;
}

void
xkb_tray_icon_popup_menu (GtkStatusIcon *status_icon, guint button,
                          guint activate_time, gpointer data)
{
    t_xkb *xkb = (t_xkb *) data;
    gtk_menu_popup (GTK_MENU (xkb->rb_mouse_popup),
                    NULL, NULL, gtk_status_icon_position_menu, status_icon, button,
                    activate_time);
}
