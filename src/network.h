/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: network.h,v 1.3 2002/01/06 15:48:00 sgt Exp $
 */

#ifndef WMLJ_NETWORK_H
#define WMLJ_NETWORK_H

#include <glib.h>
#include "wmlj.h"

extern gboolean check_friends();
extern gboolean lj_login();

#endif /* WMLJ_NETWORK_H */
