/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: dlg.c,v 1.1 2002/01/05 15:31:17 sgt Exp $
 */

/* large portions of this code was adopted from LoserJabber (see README) */

#include <stdlib.h>

#include "dlg.h"

static void
dlg_init(GtkWidget *dlg, GtkWidget *parent, gchar *title) {

  if (parent)
    gtk_window_set_transient_for(GTK_WINDOW(dlg), GTK_WINDOW(parent));

  gtk_window_set_title(GTK_WINDOW(dlg), title);

  /* make window fixed-size and modal */
  gtk_window_set_policy(GTK_WINDOW(dlg), FALSE, TRUE, TRUE);
  gtk_window_set_modal(GTK_WINDOW(dlg), TRUE);

  /* sort of a hack, to make the buttons justify the way i like. */
  GTK_BOX(GTK_DIALOG(dlg)->action_area)->homogeneous = 0;
}

GtkWidget*
dlg_new(GtkWidget* parent, gchar *title) {
  GtkWidget *dlg;

  dlg = gtk_dialog_new();
  dlg_init(dlg, parent, title);
  return dlg;
}

void
dlg_set_contents(GtkWidget *dlg, GtkWidget *contents) {
  GtkWidget *vbox;
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
  gtk_box_pack_start(GTK_BOX(vbox), contents, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show_all(GTK_DIALOG(dlg)->vbox);
}

void
dlg_add_close(GtkWidget *dlg, gchar *caption) {
  GtkWidget *button;

  button = gtk_button_new_with_label(caption ? caption : "  Close  ");
  gtk_signal_connect_object(GTK_OBJECT(button), "clicked", 
			    GTK_SIGNAL_FUNC(gtk_widget_destroy),
			    GTK_OBJECT(dlg));
  gtk_box_pack_end(GTK_BOX(GTK_DIALOG(dlg)->action_area), button,
		   FALSE, FALSE, 0);
  gtk_widget_show_all(GTK_DIALOG(dlg)->action_area);
}

GtkWidget*
dlg_add_ok_cancel(GtkWidget *dlg, gchar *ok_caption, 
		 GtkSignalFunc ok_func, gpointer data) {
  GtkWidget *ok, *cancel;

  cancel = gtk_button_new_with_label("  Cancel  ");

  gtk_signal_connect_object(GTK_OBJECT(cancel), "clicked", 
			    GTK_SIGNAL_FUNC(gtk_widget_destroy),
			    GTK_OBJECT(dlg));
  gtk_box_pack_end(GTK_BOX(GTK_DIALOG(dlg)->action_area), cancel,
		   FALSE, FALSE, 0);

  ok = gtk_button_new_with_label(ok_caption ? ok_caption : "  OK  ");
  gtk_signal_connect(GTK_OBJECT(ok), "clicked", 
		     GTK_SIGNAL_FUNC(ok_func), data);
  gtk_box_pack_end(GTK_BOX(GTK_DIALOG(dlg)->action_area), ok,
		   FALSE, FALSE, 0);

  gtk_widget_show_all(GTK_DIALOG(dlg)->action_area);
  return ok;
}

void
dlg_toggle_cb(GtkWidget *button, gboolean *b) {
  *b = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
}

void
dlg_entry_str_cb(GtkWidget *entry, gchar **str) {
  /* XXX g_strdup needed? */
  *str = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
}

void
dlg_entry_num_cb(GtkWidget *entry, gint *num) {
  *num = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
}

GtkWidget*
dlg_entry_new_with_text(gchar *text) {
  GtkWidget *entry;

  entry = gtk_entry_new();
  if (text)
    gtk_entry_set_text(GTK_ENTRY(entry), text);

  return entry;
}

/* a common dialog for sever/port entry */
GtkWidget*
dlg_address_port(gchar **conf_server, gint *conf_port) {
  GtkWidget *hbox;

  hbox = gtk_hbox_new(FALSE, 5); {
    GtkWidget *label, *server, *port;

    gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);

    label = gtk_label_new("Address:");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    server = dlg_entry_new_with_text(*conf_server);
    gtk_box_pack_start(GTK_BOX(hbox), server, TRUE, TRUE, 0);

    label = gtk_label_new("Port:");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    port = dlg_entry_new_with_text(g_strdup_printf("%d", *conf_port));
    gtk_entry_set_max_length(GTK_ENTRY(port), 5);
    gtk_widget_set_usize(port, gdk_string_width(port->style->font,
						"8080 "), -1);

    gtk_box_pack_start(GTK_BOX(hbox), port, FALSE, FALSE, 0);

    gtk_signal_connect(GTK_OBJECT(server), "changed",
		       GTK_SIGNAL_FUNC(dlg_entry_str_cb), conf_server);
    gtk_signal_connect(GTK_OBJECT(port), "changed",
		       GTK_SIGNAL_FUNC(dlg_entry_num_cb), conf_port);
  }

  return hbox;
}
