/* vim: set backspace=2 ts=4 softtabstop=4 sw=4 cinoptions=>4 expandtab autoindent smartindent: */
/* xkb-callbacks.h
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

#ifndef _GXKB_CALLBACKS_H_
#define _GXKB_CALLBACKS_H_

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "common.h"

void            xkb_active_window_changed    (WnckScreen *screen,
        WnckWindow *previously_active_window,
        t_xkb *xkb);

void            xkb_application_closed       (WnckScreen *screen,
        WnckApplication *app,
        t_xkb *xkb);

void            xkb_window_closed        (WnckScreen *screen,
        WnckWindow *window,
        t_xkb *xkb);

void            xkb_tray_icon_clicked       (GtkStatusIcon *status_icon, gpointer data);


gboolean        xkb_tray_icon_scrolled      (GtkStatusIcon *btn,
        GdkEventScroll *event,
        gpointer data);

void            xkb_tray_icon_popup_menu           (GtkStatusIcon *status_icon, guint button,
        guint activate_time, gpointer data);

#endif

