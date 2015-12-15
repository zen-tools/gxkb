/* xkb-config.h
 *
 * Copyright (C) 2015 Dmitriy Poltavchenko <admin@linuxhub.ru>
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

#ifndef _GXKB_CONFIG_H_
#define _GXKB_CONFIG_H_

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <glib.h>
#include <libxklavier/xklavier.h>

typedef enum
{
    GROUP_POLICY_GLOBAL             = 0,
    GROUP_POLICY_PER_WINDOW         = 1,
    GROUP_POLICY_PER_APPLICATION    = 2
} t_group_policy;

typedef struct
{
    gchar               *model;
    gchar               *layouts;
    gchar               *variants;
    gchar               *toggle_option;
    gchar               *compose_key_position;
} t_xkb_kbd_config;

typedef struct
{
    t_group_policy      group_policy;
    gint                default_group;
    gboolean            never_modify_config;
    t_xkb_kbd_config    *kbd_config;
} t_xkb_settings;

typedef void      (*XkbCallback)                          ( gint current_group,
                                                            gboolean groups_changed,
                                                            gpointer user_data );

gboolean          xkb_config_initialize                   ( t_xkb_settings *settings,
                                                            XkbCallback callback,
                                                            gpointer data );

void              xkb_config_finalize                     ( void );

void              kbd_config_free                         ( t_xkb_kbd_config *kbd_config );

gboolean          xkb_config_update_settings              ( t_xkb_settings *settings,
                                                            XklEngine *engine );

gint              xkb_config_get_group_count              ( void );

const gchar      *xkb_config_get_group_name               ( gint group );

const gchar      *xkb_config_get_variant                  ( gint group );

gboolean          xkb_config_set_group                    ( gint group );

gboolean          xkb_config_next_group                   ( void );

gboolean          xkb_config_prev_group                   ( void );

gint              xkb_config_variant_index_for_group      ( gint group );

void              xkb_config_window_changed               ( guint new_window_id,
                                                            guint application_id );

void              xkb_config_application_closed           ( guint application_id );

void              xkb_config_window_closed                ( guint window_id );

#endif

