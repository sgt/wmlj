/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: spawn.c,v 1.2 2002/01/05 22:49:46 sgt Exp $
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <glib.h>

#include "spawn.h"
#include "wmlj.h"

static int
spawn_cmd(gchar *cmd) {
  gchar *argv[5];

  if (DEBUG)
    fprintf(stderr, "spawning %s...\n", cmd);

  argv[0] = "/bin/sh";
  argv[1] = "-c";
  argv[2] = cmd;
  argv[3] = NULL;

  return execv("/bin/sh", argv);
}

void
spawn_app(gchar *command) {
  pid_t pid;

  if (command == NULL)
    return;

  pid = fork();

  if (pid < 0) {
    perror("fork");
    return;
  }
  else if (pid == 0) { /* child */
    if (spawn_cmd(command) < 0) {
      perror("exec");
    }

    /* never gets here, unless /bin/sh is bad. */
    _exit(0);
  }
}

void
spawn_url(gchar *command, gchar *url) {
  gchar *cmd;

  cmd = g_strdup_printf(command, url);

  spawn_app(cmd);
}
