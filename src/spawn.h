/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: spawn.h,v 1.2 2002/01/05 22:49:46 sgt Exp $
 */

#ifndef WMLJ_SPAWN_H
#define WMLJ_SPAWN_H

#include <glib.h>

extern void spawn_app(gchar *command);
extern void spawn_url(gchar *command, gchar *url);

#endif /* WMLJ_SPAWN_H */
