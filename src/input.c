/* INPUT.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "input.h"


/*TODO: 

The last message ssen cannot be received from the broker, 
so one has to store them on reception. This involves a more difficult handling of
incoming messages...


SAVE and RESTORE Buttons in the dialog which can save a value on a per topic basis for
later restore.


The input dialog windows are tricky. Dowe need to use a widget library?
What about portability?

*/

static int re_button=0;

static void on_button_clicked (GtkWidget *widget, gpointer data) {
  re_button=(int)(((char *)data)[0]-'0');
}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

    g_print ("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */

//    return TRUE;
    return FALSE;
}

static void ddestroy(GtkWidget *widget, gpointer data ) {
    gtk_main_quit ();
}
int message_dialog(char *title,char *text, int anzbut) {
  GtkWidget *window;
  GtkWidget *button,*button2,*textarea;
  GtkWidget *box1 = gtk_hbox_new (FALSE, 0);
  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
 // gtk_init (&argc, &argv);
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window),title);
  
  g_signal_connect (window, "delete-event",G_CALLBACK (delete_event), NULL);
  g_signal_connect (window, "destroy",     G_CALLBACK (ddestroy), NULL);
  
  
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  button = gtk_button_new_with_label ("OK");
  button2 = gtk_button_new_with_label ("CANCEL");
  textarea = gtk_label_new(text);
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_clicked), (gpointer) "1");
  g_signal_connect (button2, "clicked", G_CALLBACK (on_button_clicked), (gpointer) "2");

  g_signal_connect_swapped (button, "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              window);
  g_signal_connect_swapped (button2, "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              window);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_box_pack_start (GTK_BOX(vbox), textarea, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), box1, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(box1), button, TRUE, TRUE, 0);
  if(anzbut>1) gtk_box_pack_start (GTK_BOX(box1), button2, TRUE, TRUE, 0);
  
  gtk_widget_show_all (window);
  gtk_main ();

  return(re_button);
}




static volatile int fileselect_return;
static char fileselect_name[256];

/* Get the selected filename and print it to the console */
static void file_ok_sel (GtkWidget *w, GtkFileSelection *fs) {
  strcpy(fileselect_name,gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
  fileselect_return=1;
  gtk_main_quit();
}

void destroy (GtkWidget *w, gpointer *data) {
  gtk_main_quit();
}


int fileselect_dialog(char *filename, const char *path, const char *mask) {
  char buf[256];
  sprintf(buf,"%s/%s",path,filename);
  fileselect_return=0;



/* Create a new file selection widget */
    GtkWidget *filew = gtk_file_selection_new("Dashbord file selection");
    
    gtk_signal_connect(GTK_OBJECT(filew),"destroy",
                        (GtkSignalFunc) destroy, &filew);
    /* Connect the ok_button to file_ok_sel function */
    gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
                        "clicked", (GtkSignalFunc) file_ok_sel, filew );
    
    /* Connect the cancel_button to destroy the widget */
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
                               "clicked", (GtkSignalFunc) gtk_widget_destroy,
                               GTK_OBJECT (filew));
    g_signal_connect_swapped (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
                              "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              filew);
    g_signal_connect_swapped (GTK_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
                              "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              filew);

    /* Lets set the filename, as if this were a save dialog, and we are giving
     a default filename */
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(filew),buf);
    gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(filew));

    gtk_widget_show_all (filew);
    fileselect_return=0;
    gtk_main ();

    if(fileselect_return>0) {
      strcpy(filename,fileselect_name);
    }
    return(fileselect_return);
}
static volatile int input_return;
static char input_value[256];

static void inputOK_clicked (GtkWidget *widget, gpointer data) {
  input_return=1;
  strcpy(input_value,gtk_entry_get_text( GTK_ENTRY(data)));
  g_print(input_value);
}

int input_dialog(const char *topic, char *value) {
  char buf[256];
  sprintf(buf,"Enter Value for Topic\n%s:\n",topic);



  GtkWidget *window;
  GtkWidget *button,*button2,*textarea,*inputarea;
  GtkWidget *box1 = gtk_hbox_new (FALSE, 0);
  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window),"Input Topic");
  
  g_signal_connect (window, "delete-event",G_CALLBACK (delete_event), NULL);
  g_signal_connect (window, "destroy",     G_CALLBACK (ddestroy), NULL);
  
  
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  button = gtk_button_new_with_label ("OK");
  button2 = gtk_button_new_with_label ("CANCEL");
  textarea = gtk_label_new(buf);
  inputarea=gtk_entry_new();
  gtk_entry_set_text( GTK_ENTRY(inputarea),"default"); /* TODO:
  
  save the last value in el-> structure, so that it can be used here....
  
  */
  
  g_signal_connect (button, "clicked", G_CALLBACK (inputOK_clicked), (gpointer) inputarea);
  g_signal_connect (button2, "clicked", G_CALLBACK (on_button_clicked), (gpointer) "2");

  g_signal_connect_swapped (button, "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              window);
  g_signal_connect_swapped (button2, "clicked",
			      G_CALLBACK (gtk_widget_destroy),
                              window);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_box_pack_start (GTK_BOX(vbox), textarea, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), inputarea, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), box1, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(box1), button, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(box1), button2, TRUE, TRUE, 0);
  input_return=0;
  gtk_widget_show_all (window);
  gtk_main ();

    if(input_return>0) {
      strcpy(value,input_value);
      return(1);
    }


  return(0);
}
