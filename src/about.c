/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: about.c,v 1.1 2002/01/05 15:31:17 sgt Exp $
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
			  "(c) 2001, Sergei Barbarash <sgt@outline.ru>");

    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, FALSE, 0);
  }

  dlg_set_contents(dialog, vbox);
  dlg_add_close(dialog, NULL);
  gtk_widget_show(dialog);
}
