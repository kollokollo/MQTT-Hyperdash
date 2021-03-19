/* INPUT_VALUE_DIALOG.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fnmatch.h>
#include <pthread.h>

#include <gtk/gtk.h>

#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "input_value_dialog.h"

static int gtk_usage=0;


typedef struct {
  ELEMENT *el;
  GtkWidget *wg;
  GtkWidget *sv;
  GtkWidget *rb1;
  GtkWidget *rb2;
  GtkWidget *rb3;
  
} COMMON_BLOCK;

static void on_APPLY_number_clicked (GtkWidget *widget, gpointer data) {
  char input_value[256];
  COMMON_BLOCK *cb=(COMMON_BLOCK *)data;
  strncpy(input_value,gtk_entry_get_text(GTK_ENTRY(cb->wg)),sizeof(input_value));
  STRING a;
  ELEMENT *el=cb->el;
  int qos=el->revert;
  double v=myatof(input_value);
  if(v>el->max) v=el->max;
  if(v<el->min) v=el->min;
  STRING format;
  format.pointer=el->format;
  format.len=strlen(format.pointer);
  a=do_using(v,format);
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb->rb1))==TRUE) qos=0;
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb->rb2))==TRUE) qos=1;
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb->rb3))==TRUE) qos=2;
  publish_element(el,a,qos);
  free(a.pointer);
}
static void on_OK_number_clicked (GtkWidget *widget, gpointer data) {
  on_APPLY_number_clicked(widget,data);
  gtk_widget_hide(widget);
}
static void on_SAVE_clicked (GtkWidget *widget, gpointer data) {
  COMMON_BLOCK *cb=(COMMON_BLOCK *)data;
  gtk_label_set_text(GTK_LABEL(cb->sv),gtk_entry_get_text(GTK_ENTRY(cb->wg)));
}

static void on_RESTORE_clicked (GtkWidget *widget, gpointer data) {
  COMMON_BLOCK *cb=(COMMON_BLOCK *)data;
  char buf[128];
  strncpy(buf,gtk_label_get_text(GTK_LABEL(cb->sv)),128);
  gtk_entry_set_text(GTK_ENTRY(cb->wg),buf);
}


static void on_APPLY_string_clicked (GtkWidget *widget, gpointer data) {
  char input_value[256];
  COMMON_BLOCK *cb=(COMMON_BLOCK *)data;
  strncpy(input_value,gtk_entry_get_text(GTK_ENTRY(cb->wg)),sizeof(input_value));
  STRING a;
  ELEMENT *el=cb->el;
  int qos=el->revert;
  a.pointer=input_value;
  a.len=strlen(input_value);
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb->rb1))==TRUE) qos=0;
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb->rb2))==TRUE) qos=1;
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb->rb3))==TRUE) qos=2;
  publish_element(el,a,qos);
}
static void on_OK_string_clicked (GtkWidget *widget, gpointer data) {
  on_APPLY_string_clicked(widget,data);
  gtk_widget_hide(widget);
}
static gboolean delete_event(GtkWidget *widget,GdkEvent *event,gpointer data) {
  /* If you return FALSE in the "delete-event" signal handler,
   * GTK will emit the "destroy" signal. Returning TRUE means
   * you don't want the window to be destroyed.
   * This is useful for popping up 'are you sure you want to quit?'
   * type dialogs. */
  return FALSE;
}
static void destroy(GtkWidget *widget, gpointer data ) {
  gtk_widget_hide(widget);
  free(data);  /* release the common block*/
//  if(--gtk_usage==0) gtk_main_quit();
  gtk_usage--;
}
static void on_cancel_clicked (GtkWidget *widget, gpointer data) {
  gtk_widget_hide(widget);
}

static GtkWidget *create_input_dialog(char *info, char *def, int isnum,ELEMENT *el) {
  GtkWidget *window;
  GtkWidget *box1 = gtk_hbox_new (FALSE, 0);
  GtkWidget *box2 = gtk_hbox_new (FALSE, 0);
  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
  GtkWidget *vbox2 = gtk_vbox_new (FALSE, 0);
  GtkWidget *vbox3 = gtk_vbox_new (FALSE, 0);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window),"Input Topic");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  GtkWidget *button =gtk_button_new_with_label ("OK");
  GtkWidget *button2=gtk_button_new_with_label ("APPLY");
  GtkWidget *button3=gtk_button_new_with_label ("CANCEL");
  GtkWidget *button4=gtk_button_new_with_label ("SAVE");
  GtkWidget *button5=gtk_button_new_with_label ("RESTORE");
  GtkWidget *textarea=gtk_label_new(info);
  GtkWidget *storevalue=gtk_label_new(def);
  GtkWidget *frame=gtk_frame_new("Stored value:");
  GtkWidget *frame2=gtk_frame_new("Quality of Service:");
  GtkWidget *radio1=gtk_radio_button_new_with_label(NULL,"0: at most once");
  GtkWidget *radio2=gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio1)),"1: at least once");
  GtkWidget *radio3=gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio2)),"2: exactly once");
  GtkWidget *inputarea=gtk_entry_new();
  GtkWidget *separator = gtk_hseparator_new ();
  
  
  if(el->revert==0) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio1),TRUE);
  else if(el->revert==1) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio2),TRUE);
  else if(el->revert==2) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio3),TRUE);
  
  
  if(def) gtk_entry_set_text( GTK_ENTRY(inputarea),def);
  COMMON_BLOCK *cb=malloc(sizeof(COMMON_BLOCK));
  cb->el=el;
  cb->wg=inputarea;
  cb->sv=storevalue;
  cb->rb1=radio1;
  cb->rb2=radio2;
  cb->rb3=radio3;
  g_signal_connect (window, "delete-event",G_CALLBACK (delete_event),(gpointer) cb);
  g_signal_connect (window, "destroy",     G_CALLBACK (destroy),(gpointer) cb);

  if(isnum) g_signal_connect(button, "clicked", G_CALLBACK (on_OK_number_clicked),   (gpointer)cb);
  else      g_signal_connect(button, "clicked", G_CALLBACK (on_OK_string_clicked),   (gpointer)cb);
  if(isnum) g_signal_connect(button2,"clicked", G_CALLBACK (on_APPLY_number_clicked),(gpointer)cb);
  else      g_signal_connect(button2,"clicked", G_CALLBACK (on_APPLY_string_clicked),(gpointer)cb);

  g_signal_connect (button3, "clicked", G_CALLBACK (on_cancel_clicked), (gpointer) cb);
  g_signal_connect (button4, "clicked", G_CALLBACK (on_SAVE_clicked), (gpointer) cb);
  g_signal_connect (button5, "clicked", G_CALLBACK (on_RESTORE_clicked), (gpointer) cb);
  g_signal_connect_swapped(button, "clicked",G_CALLBACK(gtk_widget_destroy),window);
  g_signal_connect_swapped(button3,"clicked",G_CALLBACK(gtk_widget_destroy),window);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_box_pack_start(GTK_BOX(vbox), textarea, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), inputarea, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), storevalue, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), box2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), frame2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), separator, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), box1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box1), button2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box1), button3, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box2), button4, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box2), button5, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(frame),vbox2);
  gtk_box_pack_start(GTK_BOX(vbox3), radio1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox3), radio2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox3), radio3, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frame2),vbox3);

  return(window);
}


volatile int mainloop_running=0;

void *mainloop(void *dummy) {
  printf("main loop started.\n");
  mainloop_running=1;
   int rc=1;
   while(rc || 1) {  /* Never end this !*/
     gdk_threads_enter();
     rc=gtk_main_iteration();
     gdk_threads_leave();
   }

  printf("main loop ended. %d\n",gtk_usage);
  mainloop_running=0;
  return(NULL);
}


void topic_in_number_input(ELEMENT *el) {
  char buf[256];
  snprintf(buf,sizeof(buf),"Enter numeric value for topic\n\n%s:\n\nFormat=\"%s\"\n\n",el->topic,el->format);
  gdk_threads_enter();
  GtkWidget *window=create_input_dialog(buf,element_get_current_value(el),1,el);
  gtk_widget_show_all(window);
  gdk_threads_leave();
  gtk_usage++;
  if(!mainloop_running) {
#ifndef WINDOWS
    pthread_t thread;
    if(pthread_create(&thread, NULL, mainloop,NULL)) {
      fprintf(stderr, "Error creating thread\n");
    }
#else
  printf("ERROR: pthread not working!\n");
  mainloop(NULL);
#endif
  }
}

void topic_in_string_input(ELEMENT *el) {
  char buf[256];
  snprintf(buf,sizeof(buf),"Enter string value for topic\n\n%s:\n\n",el->topic);
  gdk_threads_enter();
  GtkWidget *window=create_input_dialog(buf,element_get_current_value(el),0,el);
  gtk_widget_show_all(window);
  gdk_threads_leave();
  gtk_usage++;
  if(!mainloop_running) {
#ifndef WINDOWS
    pthread_t thread;
    if(pthread_create(&thread, NULL, mainloop,NULL)) {
      fprintf(stderr, "Error creating thread\n");
    }
#else
  printf("ERROR: pthread not working!\n");
  mainloop(NULL);
#endif
  }
}


