/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: spawn.h,v 1.3 2002/01/06 13:45:08 sgt Exp $
 */

#ifndef WMLJ_SPAWN_H
#define WMLJ_SPAWN_H

#include <glib.h>

extern void spawn_app(gchar *command);
extern void spawn_url(gchar *command, gchar *url);

#endif /* WMLJ_SPAWN_H */
