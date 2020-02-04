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

#ifndef WINDOWS
#include <gtk/gtk.h>
#endif

#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "mqtt.h"
#include "input_value_dialog.h"

static int gtk_usage=0;

typedef struct {
  ELEMENT *el;
  GtkWidget *wg;
} COMMON_BLOCK;

static void on_APPLY_number_clicked (GtkWidget *widget, gpointer data) {
  char input_value[256];
  COMMON_BLOCK *cb=(COMMON_BLOCK *)data;
  strncpy(input_value,gtk_entry_get_text(GTK_ENTRY(cb->wg)),sizeof(input_value));
  STRING a;
  ELEMENT *el=cb->el;
  double v=myatof(input_value);
  if(v>el->max) v=el->max;
  if(v<el->min) v=el->min;
  STRING format;
  format.pointer=el->format;
  format.len=strlen(format.pointer);
  a=do_using(v,format);
  
  if(!el->subtopic) mqtt_publish(el->topic,a,el->revert,1);
  else printf("ERROR: Publishing to subtopics is currently not supported!\n");

  free(a.pointer);
}
static void on_OK_number_clicked (GtkWidget *widget, gpointer data) {
  on_APPLY_number_clicked(widget,data);
  gtk_widget_hide(widget);
}



static void on_APPLY_string_clicked (GtkWidget *widget, gpointer data) {
  char input_value[256];
  COMMON_BLOCK *cb=(COMMON_BLOCK *)data;
  strncpy(input_value,gtk_entry_get_text(GTK_ENTRY(cb->wg)),sizeof(input_value));
  STRING a;
  ELEMENT *el=cb->el;
  a.pointer=input_value;
  a.len=strlen(input_value);
  if(!el->subtopic) mqtt_publish(el->topic,a,el->revert,1);
  else printf("ERROR: Publishing to subtopics is currently not supported!\n");
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
  GtkWidget *textarea,*inputarea;
  GtkWidget *box1 = gtk_hbox_new (FALSE, 0);
  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window),"Input Topic");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  GtkWidget *button =gtk_button_new_with_label ("OK");
  GtkWidget *button2=gtk_button_new_with_label ("APPLY");
  GtkWidget *button3=gtk_button_new_with_label ("CANCEL");
  textarea = gtk_label_new(info);
  inputarea=gtk_entry_new();
  if(def) gtk_entry_set_text( GTK_ENTRY(inputarea),def);
  COMMON_BLOCK *cb=malloc(sizeof(COMMON_BLOCK));
  cb->el=el;
  cb->wg=inputarea;
  g_signal_connect (window, "delete-event",G_CALLBACK (delete_event),(gpointer) cb);
  g_signal_connect (window, "destroy",     G_CALLBACK (destroy),(gpointer) cb);

  if(isnum) g_signal_connect(button, "clicked", G_CALLBACK (on_OK_number_clicked), (gpointer) cb);
  else      g_signal_connect(button, "clicked", G_CALLBACK (on_OK_string_clicked), (gpointer) cb);
  if(isnum) g_signal_connect(button2,"clicked", G_CALLBACK (on_APPLY_number_clicked), (gpointer) cb);
  else      g_signal_connect(button2,"clicked", G_CALLBACK (on_APPLY_string_clicked), (gpointer) cb);

  g_signal_connect (button3, "clicked", G_CALLBACK (on_cancel_clicked), (gpointer) cb);

  g_signal_connect_swapped(button, "clicked",G_CALLBACK(gtk_widget_destroy),window);
  g_signal_connect_swapped(button3,"clicked",G_CALLBACK(gtk_widget_destroy),window);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_box_pack_start(GTK_BOX(vbox), textarea, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), inputarea, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), box1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box1), button2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box1), button3, TRUE, TRUE, 0);

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
  char *def=subscriptions[el->subscription].last_value.pointer;
  snprintf(buf,sizeof(buf),"Enter numeric value for topic\n\n%s:\n\nFormat=\"%s\"\n\n",el->topic,el->format);
  gdk_threads_enter();
  GtkWidget *window=create_input_dialog(buf,def,1,el);
  gtk_widget_show_all(window);
  gdk_threads_leave();
  gtk_usage++;
  if(!mainloop_running) {
    pthread_t thread;
    if(pthread_create(&thread, NULL, mainloop,NULL)) {
      fprintf(stderr, "Error creating thread\n");
    }
  }
}

void topic_in_string_input(ELEMENT *el) {
  char buf[256];
  char *def=subscriptions[el->subscription].last_value.pointer;
  snprintf(buf,sizeof(buf),"Enter string value for topic\n\n%s:\n\n",el->topic);
  gdk_threads_enter();
  GtkWidget *window=create_input_dialog(buf,def,0,el);
  gtk_widget_show_all(window);
  gdk_threads_leave();
  gtk_usage++;
  if(!mainloop_running) {
    pthread_t thread;
    if(pthread_create(&thread, NULL, mainloop,NULL)) {
      fprintf(stderr, "Error creating thread\n");
    }
  }
}


