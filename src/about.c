/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: about.c,v 1.2 2002/01/06 13:45:08 sgt Exp $
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "dlg.h"
#include "about.h"
#include "config.h"

void
cmd_about(GtkWidget *parent, GdkEvent *event) {
  GtkWidget *dialog, *vbox;

  dialog = dlg_new(parent, "About " PACKAGE);

  vbox = gtk_vbox_new(FALSE, 5); {
    GtkWidget *label;

    label = gtk_label_new(PACKAGE "-" VERSION "\n"
			  "A LiveJournal Monitor\n"
			  "(c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>");

    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, FALSE, 0);
  }

  dlg_set_contents(dialog, vbox);
  dlg_add_close(dialog, NULL);
  gtk_widget_show(dialog);
}
