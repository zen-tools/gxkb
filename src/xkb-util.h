/* xkb-util.h
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

#ifndef __GXKB_UTIL_H__
#define __GXKB_UTIL_H__

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <glib.h>

gchar*      xkb_util_get_flag_filename      (const gchar* group_name);

gchar*      xkb_util_get_layout_string      (const gchar* group_name,
                                             const gchar* variant);

gchar*      xkb_util_get_data_dir           (void);

gchar*      xkb_util_get_config_dir         (void);

gchar*      xkb_util_get_config_file        (void);

#endif
