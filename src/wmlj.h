/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: wmlj.h,v 1.6 2002/02/06 13:46:03 sgt Exp $
 */

#ifndef WMLJ_WMLJ_H
#define WMLJ_WMLJ_H

#include <pthread.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "config.h"

#define RC_FILE ".wmljrc"

#define SERVER_PATH "/interface/flat"
#define USERAGENT "Gtk-WMLJ/" VERSION

#define DEBUG 1

/* icon types */
typedef enum {
  LOGO_MAIN = 0,
  LOGO_BW,
  LOGO_ANIM_START
} IconType;

typedef struct {
  gchar *lastupdate;
  gint new_messages;
} CheckFriends;

typedef struct {
  GtkWidget *win;

  GtkWidget *msettings;

  GtkWidget *logo;

  guint cf_timeout_id;
  guint anim_timeout_id;

  pthread_t network_thread;
  pthread_mutex_t network_mutex;
  pthread_mutex_t network_error_mutex; /* if locked, there are net errors */

  CheckFriends cf;

  gint pixmap_state;

  GdkPixmap *datapix[LOGO_ANIM_START+2];
  GdkBitmap *datamask[LOGO_ANIM_START+2];

} Wmlj;

extern Wmlj wmlj;

extern void wmlj_pixmap_set(IconType icon);
extern void wmlj_cf_timeout_add();
extern void wmlj_cf_timeout_remove();
extern void wmlj_anim_timeout_add();
extern void wmlj_anim_timeout_remove();

#endif /* WMLJ_WMLJ_H */
