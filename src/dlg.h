/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: dlg.h,v 1.1 2002/01/05 15:31:17 sgt Exp $
 */

#ifndef WMLJ_DLG_H
#define WMLJ_DLG_H

#include <gtk/gtk.h>

extern GtkWidget* dlg_new(GtkWidget *parent, gchar *title);
extern void dlg_set_contents(GtkWidget *dlg, GtkWidget *contents);
extern void dlg_add_close(GtkWidget *dlg, gchar *caption);
extern GtkWidget* dlg_add_ok_cancel(GtkWidget *dlg, gchar *ok_caption, 
				    GtkSignalFunc ok_func, gpointer data);
extern void dlg_toggle_cb(GtkWidget *button, gboolean *b);
extern void dlg_entry_str_cb(GtkWidget *entry, gchar **str);
extern void dlg_entry_num_cb(GtkWidget *entry, gint *num);
GtkWidget* dlg_entry_new_with_text(gchar *text);
extern GtkWidget* dlg_address_port(gchar **conf_server, gint *conf_port);
#endif /* WMLJ_DLG_H */
