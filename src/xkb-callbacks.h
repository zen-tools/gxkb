/* xkb-callbacks.h
 *
 * Copyright (C) 2016 Dmytro Poltavchenko <dmytro.poltavchenko@gmail.com>
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
#include "config.h"
#endif

#include "xkb-config.h"
#include <libwnck/libwnck.h>

void xkb_active_window_changed(WnckScreen *screen,
                               WnckWindow *previously_active_window,
                               t_xkb_settings *xkb);

void xkb_application_closed(WnckScreen *screen, WnckApplication *app,
                            t_xkb_settings *xkb);

void xkb_window_closed(WnckScreen *screen, WnckWindow *window,
                       t_xkb_settings *xkb);

void xkb_about(void);

void xkb_main_quit(void);

#endif
