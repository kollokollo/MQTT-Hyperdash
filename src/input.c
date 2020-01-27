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
#include <fnmatch.h>

#ifndef WINDOWS
#include <gtk/gtk.h>
#endif

#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "input.h"


/*
TODO: 

SAVE and RESTORE Buttons in the dialog which can save a value on a per topic basis for
later restore.

*/


static int re_button=0;
#ifndef WINDOWS
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

   // g_print ("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */

//    return TRUE;
    return FALSE;
}

static void ddestroy(GtkWidget *widget, gpointer data ) {
    gtk_main_quit ();
}
#endif
int message_dialog(char *title,char *text, int anzbut) {
#ifndef WINDOWS
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
#endif
  return(re_button);
}


/**********************************************************
 **  File select dialog                                  **
 **********************************************************
 */


static volatile int fileselect_return;
static char fileselect_name[256];

#ifndef WINDOWS
/* Get the selected filename and print it to the console */
static void file_ok_sel (GtkWidget *w, GtkFileSelection *fs) {
  strncpy(fileselect_name,gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)),sizeof(fileselect_name));
  fileselect_return=1;
  gtk_main_quit();
}

void destroy (GtkWidget *w, gpointer *data) {
  gtk_main_quit();
}
#endif

int fileselect_dialog(char *filename, const char *path, const char *mask) {
  char buf[256];
  snprintf(buf,sizeof(buf),"%s/%s",path,filename);
  fileselect_return=0;

#ifndef WINDOWS

/* Create a new file selection widget */
    GtkWidget *filew = gtk_file_selection_new("Dashboard file selection");
    
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
#endif
    if(fileselect_return>0) {
      strcpy(filename,fileselect_name);
    }
    return(fileselect_return);
}


/**********************************************************
 **  Color picker dialog                                 **
 **********************************************************
 */


GdkColor color;
GdkColor ncolor;
GtkWidget *colw=NULL;

static void color_changed_cb(GtkWidget *widget,GtkColorSelection *colorsel) {
  GdkColor ncolor;
  gtk_color_selection_get_current_color (colorsel, &ncolor);
}

int colorselect_dialog(const char *title,char *gc) {
  int ret=0;
  unsigned int fgc=(unsigned int)myatof(gc);
  gint response;
  GtkColorSelection *colorsel;
  if(!colw) colw=gtk_color_selection_dialog_new (title);
  colorsel=GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(colw)->colorsel);
  color.red=(fgc&0xff000000)>>16;
  color.blue=(fgc&0xff00);
  color.green=(fgc&0xff0000)>>8;

  gtk_color_selection_set_previous_color (colorsel, &color);
  gtk_color_selection_set_current_color (colorsel, &color);
  gtk_color_selection_set_current_alpha(colorsel,(fgc&0xff)<<8);
  gtk_color_selection_set_has_palette (colorsel, TRUE);
  gtk_color_selection_set_has_opacity_control(colorsel, TRUE);
/* Connect to the "color_changed" signal, set the client-data
       * to the colorsel widget */
  g_signal_connect(colorsel, "color_changed",
                        G_CALLBACK (color_changed_cb), (gpointer) colorsel);
/* Show the dialog */
      response = gtk_dialog_run (GTK_DIALOG(colw));

  if(response==GTK_RESPONSE_OK) {
    gtk_color_selection_get_current_color (colorsel, &color);
    int al=gtk_color_selection_get_current_alpha(colorsel);
    printf("New color: r,g,b,a=%x %x %x %x\n",color.red,color.green,color.blue,al);
    fgc=(al>>8)|(color.blue&0xff00)|((color.green&0xff00)<<8)|((color.red&0xff00)<<16);
    sprintf(gc,"$%s",tohex(fgc));
    ret=1;
  } else {
    // gtk_widget_modify_bg (drawingarea, GTK_STATE_NORMAL, &color);
    ret=0;
  }
  gtk_widget_hide(colw);
  return(ret);
}


static volatile int input_return;
static char input_value[256];

#ifndef WINDOWS
static void inputOK_clicked (GtkWidget *widget, gpointer data) {
  input_return=1;
  strncpy(input_value,gtk_entry_get_text( GTK_ENTRY(data)),sizeof(input_value));
  // g_print(input_value);
}
#endif
int input_dialog(const char *topic, char *value, char *def) {
  char buf[256];
  snprintf(buf,sizeof(buf),"Enter Value for Topic\n\n%s:\n\n",topic);

#ifndef WINDOWS
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
  if(def) gtk_entry_set_text( GTK_ENTRY(inputarea),def);

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
#endif
  if(input_return>0) {
    strcpy(value,input_value);
    return(1);
  }
  return(0);
}


typedef struct {
  char name[32];
  char value[128];
  GtkWidget *widget;
} PVAL;

PVAL pval[32];
int anzpval=0;
static void inputOK_clicked_2(GtkWidget *widget, gpointer data) {
  input_return=1;
  int i;
  for(i=0;i<anzpval;i++) {
    strncpy(pval[i].value,gtk_entry_get_text(GTK_ENTRY(pval[i].widget)),128);
  }
}


static void on_dashbrowsebutton_clicked(GtkWidget *widget, gpointer data) {
  PVAL *val=(PVAL *)data;
  printf("Browsebutton: \n");
  printf("<%s>\n",gtk_entry_get_text(GTK_ENTRY(val->widget)));
  char newdash[256];
  strcpy(newdash,gtk_entry_get_text(GTK_ENTRY(val->widget)));
  if(*newdash=='\"') memmove(newdash,newdash+1,strlen(newdash)-2);
  /* open a file selector to select a new dash to display.*/
  int rc=fileselect_dialog(newdash,dashboarddir,"*.dash");
  if(rc && newdash[0]) {
    if(!fnmatch("*.dash",newdash,FNM_NOESCAPE)) {
      newdash[strlen(newdash)-5]=0;
      printf("Kürze auf: <%s>\n",newdash);
    }
    char buf[256];
    if(!strncmp(newdash,dashboarddir,strlen(dashboarddir))) sprintf(buf,"\"%s\"",newdash+strlen(dashboarddir)+1);
    else sprintf(buf,"\"%s\"",newdash);
    
    printf("Result. <%s>\n",buf);
    gtk_entry_set_text(GTK_ENTRY(val->widget),buf);
  }
}
static void on_fontbrowsebutton_clicked(GtkWidget *widget, gpointer data) {
  PVAL *val=(PVAL *)data;
  printf("Browsebutton: \n");
  printf("<%s>\n",gtk_entry_get_text(GTK_ENTRY(val->widget)));
  char newdash[256];
  strcpy(newdash,gtk_entry_get_text(GTK_ENTRY(val->widget)));
  if(*newdash=='\"') memmove(newdash,newdash+1,strlen(newdash)-2);
  /* open a file selector to select a new font.*/
  int rc=fileselect_dialog(newdash,fontdir,"*.ttf");
  if(rc && *newdash) {
    if(!fnmatch("*.ttf",newdash,FNM_NOESCAPE)) {
      newdash[strlen(newdash)-4]=0;
      printf("Kürze auf: <%s>\n",newdash);
    }
    char buf[256];
    if(!strncmp(newdash,fontdir,strlen(fontdir))) sprintf(buf,"\"%s\"",newdash+strlen(fontdir)+1);
    else sprintf(buf,"\"%s\"",newdash);
    
    printf("Result. <%s>\n",buf);
    gtk_entry_set_text(GTK_ENTRY(val->widget),buf);
  }
}
static void on_topicbrowsebutton_clicked(GtkWidget *widget, gpointer data) {
  PVAL *val=(PVAL *)data;
  printf("Browsebutton: \n");
  printf("<%s>\n",gtk_entry_get_text(GTK_ENTRY(val->widget)));
  char newdash[256];
  strcpy(newdash,gtk_entry_get_text(GTK_ENTRY(val->widget)));
  if(*newdash=='\"') memmove(newdash,newdash+1,strlen(newdash)-2);

  /* TODO open a topic selector to select a topic to display.*/
//    printf("Result. <%s>\n",buf);
//    gtk_entry_set_text(GTK_ENTRY(val->widget),buf);
  
}
static void on_colorbrowsebutton_clicked(GtkWidget *widget, gpointer data) {
  PVAL *val=(PVAL *)data;
  char newdash[256];
  strcpy(newdash,gtk_entry_get_text(GTK_ENTRY(val->widget)));
  if(*newdash=='\"') memmove(newdash,newdash+1,strlen(newdash)-2);
  int rc=colorselect_dialog("Select color",newdash);
  if(rc) {
    gtk_entry_set_text(GTK_ENTRY(val->widget),newdash);
  }  
}






static void on_iconbrowsebutton_clicked(GtkWidget *widget, gpointer data) {
  PVAL *val=(PVAL *)data;
  printf("Browsebutton: \n");
  printf("<%s>\n",gtk_entry_get_text(GTK_ENTRY(val->widget)));
  char newdash[256];
  strcpy(newdash,gtk_entry_get_text(GTK_ENTRY(val->widget)));
  if(*newdash=='\"') memmove(newdash,newdash+1,strlen(newdash)-2);
  /* open a file selector to select a new dash to display.*/
  int rc=fileselect_dialog(newdash,icondir,"*.png");
  if(rc && newdash[0]) {
    char buf[256];
    if(!strncmp(newdash,icondir,strlen(icondir))) sprintf(buf,"\"%s\"",newdash+strlen(icondir)+1);
    else sprintf(buf,"\"%s\"",newdash);
    
    printf("Result. <%s>\n",buf);
    gtk_entry_set_text(GTK_ENTRY(val->widget),buf);
  }
}




static void on_bitmapbrowsebutton_clicked(GtkWidget *widget, gpointer data) {
  PVAL *val=(PVAL *)data;
  printf("Browsebutton: \n");
  printf("<%s>\n",gtk_entry_get_text(GTK_ENTRY(val->widget)));
  char newdash[256];
  strcpy(newdash,gtk_entry_get_text(GTK_ENTRY(val->widget)));
  if(*newdash=='\"') memmove(newdash,newdash+1,strlen(newdash)-2);
  /* open a file selector to select a new dash to display.*/
  int rc=fileselect_dialog(newdash,bitmapdir,"*");
  if(rc && newdash[0]) {
    char buf[256];
    if(!strncmp(newdash,bitmapdir,strlen(bitmapdir))) sprintf(buf,"\"%s\"",newdash+strlen(bitmapdir)+1);
    else sprintf(buf,"\"%s\"",newdash);
    
    printf("Result. <%s>\n",buf);
    gtk_entry_set_text(GTK_ENTRY(val->widget),buf);
  }
}




/*  Add buttons to 
         FGB, BGC, AGC to open a color picker 
	 BITMAP, ICON, DASH to open a fileselector
	 */

int property_dialog(char *elline) {
  char a[256*4];
  char aa[64];
  char b[256*4];
  anzpval=0;
#ifndef WINDOWS
  GtkWidget *window;
  GtkWidget *button,*button2;
  GtkWidget *box1 = gtk_hbox_new (FALSE, 0);
  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
  GtkWidget *wd,*hb;
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window),"Edit Element Properties");
  
  g_signal_connect (window, "delete-event",G_CALLBACK (delete_event), NULL);
  g_signal_connect (window, "destroy",     G_CALLBACK (ddestroy), NULL);
  
  
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  button = gtk_button_new_with_label ("OK");
  button2 = gtk_button_new_with_label ("CANCEL");
  g_signal_connect (button, "clicked", G_CALLBACK (inputOK_clicked_2), (gpointer) NULL);
  g_signal_connect (button2, "clicked", G_CALLBACK (on_button_clicked), (gpointer) "2");
  g_signal_connect_swapped(button,"clicked",G_CALLBACK(gtk_widget_destroy),window);
  g_signal_connect_swapped(button2,"clicked",G_CALLBACK (gtk_widget_destroy),window);

  gtk_container_add(GTK_CONTAINER (window), vbox);
  
  if(wort_sep(elline,':',1,a,b)) {
    strcpy(aa,a);
    wd=gtk_label_new("Edit Properties:");
    gtk_box_pack_start(GTK_BOX(vbox), wd, TRUE, TRUE, 0);
    wd=gtk_label_new(a);
    gtk_box_pack_start(GTK_BOX(vbox), wd, TRUE, TRUE, 0);
  }
  gtk_box_pack_start (GTK_BOX(box1), button, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(box1), button2, TRUE, TRUE, 0);
  xtrim(b,1,b);
  printf("b=<%s>\n",b);
  while(wort_sep(b,' ',1,a,b)) {
    char *p=a;
    while(*p && *p!='=') p++;
    if(*p==0) break;
    *p++=0;
    hb=gtk_hbox_new(FALSE, 0);
    wd=gtk_label_new(a);
    gtk_box_pack_start(GTK_BOX(hb), wd, TRUE, TRUE, 0);
    pval[anzpval].widget=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(pval[anzpval].widget),p);
    
    if(!strcmp(a,"BGC") || !strcmp(a,"FGC") || !strcmp(a,"AGC") || !strcmp(a,"TGC")) {
      wd=gtk_button_new_with_label("Browse");
      g_signal_connect(wd,"clicked",G_CALLBACK (on_colorbrowsebutton_clicked), (gpointer) &pval[anzpval]);
      gtk_box_pack_end(GTK_BOX(hb),wd,TRUE, TRUE, 0);      
    }
    if(!strcmp(a,"TOPIC")) {
      wd=gtk_button_new_with_label("Browse");
      g_signal_connect(wd,"clicked",G_CALLBACK (on_topicbrowsebutton_clicked), (gpointer) &pval[anzpval]);
      gtk_box_pack_end(GTK_BOX(hb),wd,TRUE, TRUE, 0);      
    }
    if(!strcmp(a,"FONT")) {
      wd=gtk_button_new_with_label("Browse");
      g_signal_connect(wd,"clicked",G_CALLBACK (on_fontbrowsebutton_clicked), (gpointer) &pval[anzpval]);
      gtk_box_pack_end(GTK_BOX(hb),wd,TRUE, TRUE, 0);      
    }
    if(!strcmp(a,"ICON")) {
      wd=gtk_button_new_with_label("Browse");
      g_signal_connect(wd,"clicked",G_CALLBACK (on_iconbrowsebutton_clicked), (gpointer) &pval[anzpval]);
      gtk_box_pack_end(GTK_BOX(hb),wd,TRUE, TRUE, 0);      
    }
    if(!strcmp(a,"BITMAP")) {
      wd=gtk_button_new_with_label("Browse");
      g_signal_connect(wd,"clicked",G_CALLBACK (on_bitmapbrowsebutton_clicked), (gpointer) &pval[anzpval]);
      gtk_box_pack_end(GTK_BOX(hb),wd,TRUE, TRUE, 0);      
    }
    if(!strcmp(a,"DASH")) {
      wd=gtk_button_new_with_label("Browse");
      g_signal_connect(wd,"clicked",G_CALLBACK (on_dashbrowsebutton_clicked), (gpointer) &pval[anzpval]);
      gtk_box_pack_end(GTK_BOX(hb),wd,TRUE, TRUE, 0);      
    }

    gtk_box_pack_end(GTK_BOX(hb), pval[anzpval].widget, FALSE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(vbox), hb, TRUE, TRUE, 0);
    
    
    strncpy(pval[anzpval].name,a,32);
    strncpy(pval[anzpval].value,p,128);
    anzpval++;
  }
  gtk_box_pack_start (GTK_BOX(vbox), box1, TRUE, TRUE, 0);
  input_return=0;
  gtk_widget_show_all(window);
  gtk_main();

  if(input_return>0) {
    sprintf(elline,"%s:",aa);
    int i;
    for(i=0;i<anzpval;i++) {
      sprintf(elline+strlen(elline)," %s=%s",pval[i].name,pval[i].value);
    }
    return(1);
  }
  #endif
  return(0);
}
