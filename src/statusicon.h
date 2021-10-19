/* statusicon.h
 *
 * Copyright (C) 2016 Dmytro Poltavchenko <dmytro.poltavchenko@gmail.com>
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

#ifndef _GXKB_STATUSICON_H_
#define _GXKB_STATUSICON_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_APPINDICATOR
#include <libayatana-appindicator/app-indicator.h>
#endif

#include "gettext.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "xkb-callbacks.h"
#include "xkb-util.h"

typedef enum { SYSTRAY, APPINDICATOR } statusicon_type;

void statusicon_new(void);

void gtk_status_icon_clicked(GtkStatusIcon *status_icon, gpointer data);

gboolean gtk_status_icon_scrolled(GtkStatusIcon *status_icon,
                                  GdkEventScroll *event, gpointer data);

void gtk_status_icon_popup_menu(GtkStatusIcon *status_icon, guint button,
                                guint activate_time, gpointer data);

void statusicon_set_group(GtkWidget *item, gpointer data);

void statusicon_update_current_image(void);

void statusicon_update_menu(void);

void statusicon_destroy_menu(GtkWidget *menu);

void statusicon_free(void);

#ifdef HAVE_APPINDICATOR
GtkStatusIcon *appindicator_fallback(AppIndicator *indicator);

void appindicator_unfallback(AppIndicator *indicator,
                             GtkStatusIcon *status_icon);

void appindicator_icon_scrolled(AppIndicator *indicator, gint delta,
                                GdkScrollDirection direction,
                                gpointer user_data);
#endif

#endif
