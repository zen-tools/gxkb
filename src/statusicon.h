#ifndef _GXKB_STATUSICON_H_
#define _GXKB_STATUSICON_H_

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#ifdef HAVE_APPINDICATOR
#   include <libappindicator/app-indicator.h>
#endif

#include <gtk/gtk.h>
#include "xkb-callbacks.h"
#include "xkb-util.h"

typedef enum {SYSTRAY, APPINDICATOR} statusicon_type;

GtkWidget       *lb_mouse_popup;
GtkWidget       *rb_mouse_popup;
#ifdef HAVE_APPINDICATOR
AppIndicator    *appindicator;
#endif
GtkStatusIcon   *trayicon;
statusicon_type icon_type;

void            status_icon_new                     ( void );

void            gtk_status_icon_clicked             ( GtkStatusIcon *status_icon,
                                                      gpointer data );


gboolean        gtk_status_icon_scrolled            ( GtkStatusIcon *status_icon,
                                                      GdkEventScroll *event,
                                                      gpointer data );

void            gtk_status_icon_popup_menu          ( GtkStatusIcon *status_icon,
                                                      guint button,
                                                      guint activate_time,
                                                      gpointer data );

void            statusicon_set_group                ( GtkWidget *item,
                                                      gpointer data );

void            statusicon_set_image                ( gchar *filepath );

void            statusicon_update_current_image     ( void );

void            statusicon_update_menu              ( void );

void            statusicon_destroy_menu             ( GtkWidget *menu );

void            statusicon_free                     ( void );

#ifdef HAVE_APPINDICATOR
GtkStatusIcon   *appindicator_fallback              ( AppIndicator  *indicator );

void            appindicator_unfallback             ( AppIndicator  *indicator,
                                                      GtkStatusIcon *status_icon );

void            appindicator_icon_scrolled          ( AppIndicator  *indicator,
                                                      gint delta,
                                                      GdkScrollDirection direction,
                                                      gpointer user_data );
#endif

#endif
