/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: wmlj.h,v 1.4 2002/01/09 16:13:54 sgt Exp $
 */

#ifndef WMLJ_WMLJ_H
#define WMLJ_WMLJ_H

#include <pthread.h>
#include <glib.h>

#include "config.h"

#define RC_FILE ".wmljrc"

#define SERVER_PATH "/interface/flat"
#define USERAGENT "Gtk-WMLJ/" VERSION

#define DEBUG 1

typedef struct {
  gchar *lastupdate;
  gint new_messages;
} CheckFriends;

extern CheckFriends cf;

extern guint cf_timeout_id;
extern guint anim_timeout_id;
extern pthread_mutex_t network_mut;

extern void wmlj_cf_timeout_add();
extern void wmlj_cf_timeout_remove();
extern void wmlj_anim_timeout_add();
extern void wmlj_anim_timeout_remove();

#endif /* WMLJ_WMLJ_H */
