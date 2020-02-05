/* DASHDESIGN.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
/*
 * Thease are the sources for the MQTT Hyperdash dashboad grafical editor.
 * 
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "config.h"
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "input.h"
#include "dashdesign.h"
#include "menu.h"
#include "element_groups.h"

#if defined WINDOWS 
  #define EX_OK 0
#else
  #include <sysexits.h>
#endif

char ifilename[SIZEOF_IFILENAME]="new.dash";
int verbose=0;
int do_connection=0;   /* Do not connect to a broker .*/
int do_show_invisible=1; /* Also draw invisible elements. */
DASH *maindash;
WINDOW *mainwindow;

extern WINDOW *global_window;  /* TODO */
char *broker_override=NULL;
char *broker_user=NULL;
char *broker_passwd=NULL;
char *topic_prefix=NULL;
char call_options[SIZEOF_CALL_OPTIONS]="";

int current_element=DEFAULT_ELEMENT;
int current_group=DEFAULT_GROUP;
int current_action=A_NONE;

unsigned long int current_fgc=0xffffffff;
unsigned long int current_bgc=0x40ff;
char current_font[256]=DEFAULT_FONT;
char current_topic[256]=DEFAULT_TOPIC;

int is_modified=0;
int do_grid=0;

int current_mouse_x=0;
int current_mouse_y=0;

SDL_Surface *surface;
GtkWidget *textarea;
GtkWidget *window;
GtkWidget *drawing_area;
/* Backing pixmap for drawing area */
GdkPixmap *pixmap = NULL;

ELEMENT *undo_element=NULL;

const char *action_names[]={
"Identify",
"Move",
"Copy",
"Resize",
"Move to Bkg",
"Add",
"Add group",
"Delete",
"Edit",
"Set foreground color",
"Set background color",
"Set font",
"Set topic"
};


static void intro() {
  printf("**********************************************************\n"
         "*  %10s  " MQTT_DASHDESIGN_EXECUTABLE_NAME "          V." VERSION "            *\n"
         "*                       by Markus Hoffmann 2019-2020 (c) *\n"
         "* version date:           %30s *\n"
         "**********************************************************\n\n",
	     PACKAGE_NAME,VERSION_DATE);
}
static void usage() {
  printf(
    "Usage: %s [-h] [<filename>] --- edit dashboard [%s]\n\n"
    " -h --help\t\t--- usage\n"
    " --iconpath <path>\t--- set path for icon files [%s]\n"
    " --bitmappath <path>\t--- set path for bitmap files [%s]\n"
    " --dashpath <path>\t--- set path for dash files [%s]\n"
    " --fontpath <path>\t--- set path for bitmap files [%s]\n"
    " --broker <url>\t\t--- use this broker by default.\n"
    " --user <user>\t\t--- use this username for broker.\n"
    " --passwd <passwd>\t\t--- use this password for broker.\n"
    ,MQTT_DASHDESIGN_EXECUTABLE_NAME,ifilename,icondir,bitmapdir,dashboarddir,fontdir);
}
#define COLLECT() snprintf(call_options+strlen(call_options),sizeof(call_options)-strlen(call_options)," %s",argumente[count])
#define COLLECT2() snprintf(call_options+strlen(call_options),sizeof(call_options)-strlen(call_options)," %s %s",argumente[count-1],argumente[count])
static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters and */
  /* Collect call options used when hyperdash calls itself for a subDash: */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      intro();
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"-v"))	      {verbose++; COLLECT();}
    else if(!strcmp(argumente[count],"-q"))	      {verbose--; COLLECT();}
    else if(!strcmp(argumente[count],"--iconpath"))   {strncpy(icondir,     argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--bitmappath")) {strncpy(bitmapdir,   argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--fontpath"))   {strncpy(fontdir,     argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--dashpath"))   {strncpy(dashboarddir,argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--broker"))     {broker_override=argumente[++count];COLLECT2();}
    else if(!strcmp(argumente[count],"--user"))       {broker_user=    argumente[++count];COLLECT2();}
    else if(!strcmp(argumente[count],"--passwd"))     {broker_passwd=  argumente[++count];COLLECT2();}
    else if(*(argumente[count])=='-')                 {COLLECT();}
    else {
      strncpy(ifilename,argumente[count],sizeof(ifilename));
    }
  }
  if(quitflag) exit(EX_OK);
}

/* Initialize after open of a new dashboard file. */
void init_newloaded_dash() {
  is_modified=0;
  init_dash(maindash);
  close_pixmap(mainwindow);
  mainwindow=open_pixmap(maindash->tree[maindash->panelelement].text,PACKAGE_NAME,0,0,maindash->tree[maindash->panelelement].w,maindash->tree[maindash->panelelement].h,0);
  global_window=mainwindow; /* TODO */
  draw_dash(maindash,mainwindow);

  current_fgc=maindash->tree[maindash->panelelement].fgc;
  current_bgc=maindash->tree[maindash->panelelement].bgc;
  if(maindash->tree[maindash->panelelement].font)
    strncpy(current_font,maindash->tree[maindash->panelelement].font,sizeof(current_font));

  if(pixmap) {g_object_unref(pixmap);pixmap=NULL;}
  gtk_window_resize(GTK_WINDOW(window),mainwindow->w+2,mainwindow->h+54);
  update_title(ifilename);
  update_drawarea();
  update_statusline();
}

void emergency_save_dialog() {
  char buffer[256];
  snprintf(buffer,sizeof(buffer),"WARNING:\nCurrent dashboard has not yet been saved.\n"
  				 "Do you want to save it now?\n\n"
  				 "%s\n\n",ifilename);
  if(message_dialog("MQTT Hyperdash Dashdesign Warning",buffer,2)==1) {
    save_dash(maindash,ifilename);
    is_modified=0;
    update_title(ifilename);
  } else {    
    /* Emergency-save the dashboard. */
    char newname[strlen(ifilename)+12];
    strcpy(newname,ifilename);
    strcat(newname,AUTOSAVE_ENDING);
    save_dash(maindash,newname);
  }
}

char *converted_pixels=NULL;

void fix_pixmap(GtkWidget *widget) {
  if(!pixmap) {
    pixmap = gdk_pixmap_new(widget->window,widget->allocation.width,widget->allocation.height,-1);			   
    /* Swap BGRA to RGBA */
    converted_pixels=realloc(converted_pixels,surface->w*surface->h*4);
    char *buf=converted_pixels;
    int i;
    for(i=0;i<surface->w*surface->h;i++) {
      buf[4*i+0]=((char *)(surface->pixels))[4*i+2];
      if(do_grid && (i%5)==0 && ((i/surface->w)%5)==0) buf[4*i+1]=((char *)(surface->pixels))[4*i+1]+0x7f;
      else buf[4*i+1]=((char *)(surface->pixels))[4*i+1];
      buf[4*i+2]=((char *)(surface->pixels))[4*i+0];
      buf[4*i+3]=((char *)(surface->pixels))[4*i+3];
    }
    gdk_draw_rgb_32_image(pixmap,widget->style->white_gc,0,0,surface->w,surface->h,0,(const guchar *)buf,4*surface->w);

    gtk_widget_queue_draw_area(widget,0,0,widget->allocation.width,widget->allocation.height);
  }
}



/* Create a new backing pixmap of the appropriate size */
static gboolean configure_event( GtkWidget *widget, GdkEventConfigure *event) {
  fix_pixmap(widget);
  return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event) {
  fix_pixmap(widget);
  gdk_draw_drawable(widget->window,
		     widget->style->fg_gc[gtk_widget_get_state (widget)],
		     pixmap,
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  return FALSE;
}

void update_statusline() {
  char status_text[256];
  char buf[128];
  if(current_action==A_ADD) snprintf(buf,sizeof(buf),"%s %s, FGC=$%s, BGC=$%s, Font=%s",
    action_names[current_action],eltyps[current_element].name,tohex(current_fgc), 
    tohex(current_bgc),current_font);
  else if(current_action==A_ADDGROUP) snprintf(buf,sizeof(buf),"%s %s, FGC=$%s, BGC=$%s, Font=%s",
    action_names[current_action],groups[current_group].name,tohex(current_fgc), 
    tohex(current_bgc),current_font);
  else if(current_action==A_SFGC) snprintf(buf,sizeof(buf),"%s $%s",
    action_names[current_action],tohex(current_fgc));
  else if(current_action==A_SBGC) snprintf(buf,sizeof(buf),"%s $%s",
    action_names[current_action],tohex(current_bgc));
  else if(current_action==A_SFONT) snprintf(buf,sizeof(buf),"%s %s",
    action_names[current_action],current_font);
  else if(current_action==A_STOPIC) snprintf(buf,sizeof(buf),"%s %s",
    action_names[current_action],current_topic);
  else snprintf(buf,sizeof(buf),"%s",action_names[current_action]);

  snprintf(status_text,sizeof(status_text),"%d elements, (%dx%d), X=%d, Y=%d, Action: %s",
    maindash->anzelement,mainwindow->w,mainwindow->h,current_mouse_x,current_mouse_y,buf);
  gtk_label_set_text(GTK_LABEL(textarea),status_text);
}

int selected_element=-1;
int mouse_rel_x,mouse_rel_y;

void redraw_panel(GtkWidget *widget) {
  draw_dash(maindash,mainwindow);

  if(pixmap) g_object_unref(pixmap);
  pixmap=NULL;
  update_drawarea();
  update_title(ifilename);
  gtk_widget_queue_draw_area(widget,0,0,mainwindow->w,mainwindow->h);
}


static gboolean button_press_event(GtkWidget *widget,GdkEventButton *event) {
  fix_pixmap(widget);
  if(event->button>0 && pixmap!=NULL) {
    if(event->x<mainwindow->w && event->y<mainwindow->h) {
      current_mouse_x = event->x;
      current_mouse_y = event->y;
    }
    int idx=find_element(maindash,-1,current_mouse_x,current_mouse_y,0,0);
    
    switch(current_action) {
    case A_NONE:       /* Identify Elements under the mouse pointer */
      if(event->button==1) {
        char buf[1024*2];
        sprintf(buf,"(%d,%d): {\n",current_mouse_x,current_mouse_y);
        while(idx>=0) {
          sprintf(buf+strlen(buf),"  #%d %s\n",idx,element2a(&maindash->tree[idx]));
	  if(idx==0) break;
          idx=find_element(maindash,idx-1,current_mouse_x,current_mouse_y,0,0);      
        }
        sprintf(buf+strlen(buf),"}\n");
	int i=0;
	char *p=buf;
	while(*p) {
	  if(*p=='\n') i=0;
          if(++i>96 && isspace(*p)) {i=0;*p='\n';}
	  p++;
	}
	message_dialog(PACKAGE_NAME " Element Info",buf,1);
      } else {
        printf("(%d,%d): {\n",current_mouse_x,current_mouse_y);
        while(idx>=0) {
          printf("  Element #%d <%s>\n",idx,element2a(&maindash->tree[idx]));
	  if(idx==0) break;
          idx=find_element(maindash,idx-1,current_mouse_x,current_mouse_y,0,0);      
        }
        printf("}\n");
      }
      break;
    case A_ADD:
    case A_ADDGROUP:
      selected_element=maindash->anzelement;
      mouse_rel_x=current_mouse_x;
      mouse_rel_y=current_mouse_y;
      ELEMENT el=default_element(current_element,current_fgc,current_bgc, current_font);
      el.x=current_mouse_x;
      el.y=current_mouse_y;
      if(do_grid) {
        el.x-=el.x%5;
        el.y-=el.y%5;
      }
      add_element(maindash,&el);
      
      gdk_draw_rectangle(pixmap,widget->style->white_gc,FALSE,
        current_mouse_x, current_mouse_y,5,5);
      gtk_widget_queue_draw_area(widget,current_mouse_x, current_mouse_y,5+1, 5+1);
      break;
    case A_DELETE:
      if(event->button==1) {
        if(idx>=0 && !((maindash->tree[idx].type&EL_PANEL)==EL_PANEL)) {
	  printf("Delete Element: #%d\n",idx);
	  /* in den undo-buffer überführen...*/
          if(undo_element) {
	    free_element(undo_element);
	    free(undo_element);
	  }
	  undo_element=calloc(1,sizeof(ELEMENT));
	  *undo_element=maindash->tree[idx];
	  delete_element(maindash,idx);
	
	  is_modified=1;
	  redraw_panel(widget);
        }
      }
      break;
    case A_EDIT:
      if(event->button==1) {
        printf("Edit Element: #%d\n",idx);
        if(idx>=0) {
          if(edit_element(&(maindash->tree[idx]))) {
	    if(idx==maindash->panelelement) {
	      printf("The Panel was changed!\n");
	      close_pixmap(mainwindow);
              mainwindow=open_pixmap(maindash->tree[maindash->panelelement].text,PACKAGE_NAME,0,0,maindash->tree[maindash->panelelement].w,maindash->tree[maindash->panelelement].h,0);
              global_window=mainwindow; /* TODO */
              draw_dash(maindash,mainwindow);
              gtk_window_resize(GTK_WINDOW(window),mainwindow->w+2,mainwindow->h+54);
	    }
            is_modified=1;
	    redraw_panel(widget);
          }
        }
      }
      break;
    case A_SFGC:
      printf("Set foreground color for element: #%d\n",idx);
      if(idx>=0) {
        maindash->tree[idx].fgc=current_fgc;
        is_modified=1;
	redraw_panel(widget);
      }
      break;
    case A_SBGC:
      printf("Set background color for element: #%d\n",idx);
      if(idx>=0) {
        maindash->tree[idx].bgc=current_bgc;
        is_modified=1;
	redraw_panel(widget);
      }
      break;
    case A_SFONT:
      printf("Set font for element: #%d\n",idx);
      if(idx>=0) {
        if(maindash->tree[idx].font) {
	  free(maindash->tree[idx].font);
	  maindash->tree[idx].font=strdup(current_font);
          int i=add_font(maindash->tree[idx].font,maindash->tree[idx].fontsize);
          if(!fonts[i].font) open_font(i);
          maindash->tree[idx].fontnr=i;
          is_modified=1;
	  redraw_panel(widget);
	}
      }
      break;
    case A_STOPIC:
      while(idx>=0) {
        if((maindash->tree[idx].type&EL_DYNAMIC)==EL_DYNAMIC) {
          printf("Set topic for element: #%d\n",idx);
	  free(maindash->tree[idx].topic);
	  maindash->tree[idx].topic=strdup(current_topic);
	  is_modified=1;
	  redraw_panel(widget);
	}
	if(idx==0) break;
        idx=find_element(maindash,idx-1,current_mouse_x,current_mouse_y,0,0);      
      }
      break;
    case A_MTB:
      if(idx>=0 && !((maindash->tree[idx].type&EL_PANEL)==EL_PANEL)) {
	ELEMENT el=duplicate_element(&(maindash->tree[idx]));
	delete_element(maindash,idx);
        /* Suche Panel element.*/
	int k=0;
	for(k=0;k<maindash->anzelement;k++) {
	  if((maindash->tree[k].type&EL_PANEL)==EL_PANEL) {
	    k++;
	    break;
	  }
	}
	printf("Move element: #%d to background ... (%d)\n",idx,k);
        insert_element(maindash,k,&el);

	is_modified=1;
	redraw_panel(widget);
      }
      break;
    case A_MOVE:
    case A_COPY:
    case A_RESIZE:
      if(event->button==1) {
        if(idx>=0 && !((maindash->tree[idx].type&EL_PANEL)==EL_PANEL)) {
          selected_element=idx;
          mouse_rel_x=current_mouse_x;
          mouse_rel_y=current_mouse_y;
	  gdk_draw_rectangle(pixmap,widget->style->white_gc,FALSE,
	  maindash->tree[idx].x, maindash->tree[idx].y,maindash->tree[idx].w, maindash->tree[idx].h);
          gtk_widget_queue_draw_area(widget,maindash->tree[idx].x, maindash->tree[idx].y,
 	    maindash->tree[idx].w+1, maindash->tree[idx].h+1);
        } else selected_element=-1;
      } else {
        if(idx>=0) {
          char buf[1024*2];
	  *buf=0;
	  if((maindash->tree[idx].type&EL_COMPOUND)==EL_COMPOUND) {
            printf("copy compound to clipboard [");
	    int i;
 	    ELEMENT *el;
	    for(i=0;i<idx;i++) {
	      el=&(maindash->tree[i]);
	      if(el->x>=maindash->tree[idx].x && 
	         el->x+el->w<=maindash->tree[idx].x+maindash->tree[idx].w &&
	         el->y>=maindash->tree[idx].y && 
 	         el->y+el->h<=maindash->tree[idx].y+maindash->tree[idx].h) {
                   sprintf(buf+strlen(buf),"%s\n",element2a(&(maindash->tree[i])));
	           printf(" #%d",i);
	      }
	    }
	  } 
          sprintf(buf+strlen(buf),"%s\n",element2a(&(maindash->tree[idx])));
          /* Copy to clipboard */
          GtkClipboard *clip=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
          gtk_clipboard_set_text(clip,buf,strlen(buf));
	  printf("]\nCopy to clipboard: \n%s",buf);
	}
      }
      break;
    }
    update_statusline();
  } 
  return TRUE;
}
static gboolean button_release_event(GtkWidget *widget,GdkEventButton *event) {
  fix_pixmap(widget);
  if(event->button==1 && pixmap!=NULL) {
    if(event->x<mainwindow->w && event->y<mainwindow->h) {
      current_mouse_x = event->x;
      current_mouse_y = event->y;
    }
    switch(current_action) {
    case A_MOVE:
      if(selected_element>=0) {
        int idx=selected_element;
	int new_x=maindash->tree[idx].x+current_mouse_x-mouse_rel_x;
	int new_y=maindash->tree[idx].y+current_mouse_y-mouse_rel_y;
	if(new_x<0) new_x=0;
	if(new_y<0) new_y=0;
	if(new_x>mainwindow->w) new_x=mainwindow->w-1;
	if(new_y>mainwindow->h) new_y=mainwindow->h-1;
	if(do_grid) {
	  new_x-=(new_x%5);
	  new_y-=(new_y%5);
	}
	if((maindash->tree[idx].type&EL_COMPOUND)==EL_COMPOUND) {
          printf("move compound.\n");
	  int i;
	  ELEMENT *el;
	  for(i=0;i<idx;i++) {
	    el=&(maindash->tree[i]);
	    if(el->x>=maindash->tree[idx].x && 
	       el->x+el->w<=maindash->tree[idx].x+maindash->tree[idx].w &&
	       el->y>=maindash->tree[idx].y && 
 	       el->y+el->h<=maindash->tree[idx].y+maindash->tree[idx].h) {
	         el->x+=new_x-maindash->tree[idx].x;
	         el->y+=new_y-maindash->tree[idx].y;
	         printf("Move %d\n",i);
	    }
	  }
	}
        printf("Move Element #%d from (%d,%d) to (%d,%d)\n",selected_element,maindash->tree[idx].x,maindash->tree[idx].y, 
	  new_x,new_y);
        maindash->tree[idx].x=new_x;
        maindash->tree[idx].y=new_y;
	
	
	is_modified=1;
	selected_element=-1;
        redraw_panel(widget);
      }
      break;
    case A_COPY:
      if(selected_element>=0) {
        int idx=selected_element;
	int new_x=maindash->tree[idx].x+current_mouse_x-mouse_rel_x;
	int new_y=maindash->tree[idx].y+current_mouse_y-mouse_rel_y;
	if(new_x<0) new_x=0;
	if(new_y<0) new_y=0;
	if(new_x>mainwindow->w) new_x=mainwindow->w-1;
	if(new_y>mainwindow->h) new_y=mainwindow->h-1;
	if(do_grid) {
	  new_x-=(new_x%5);
	  new_y-=(new_y%5);
	}
	if((maindash->tree[idx].type&EL_COMPOUND)==EL_COMPOUND) {
          printf("copy compound.\n");
	  int i;
	  ELEMENT *el;
	  ELEMENT elc;
	  for(i=0;i<idx;i++) {
	    el=&(maindash->tree[i]);
	    if(el->x>=maindash->tree[idx].x && 
	       el->x+el->w<=maindash->tree[idx].x+maindash->tree[idx].w &&
	       el->y>=maindash->tree[idx].y && 
 	       el->y+el->h<=maindash->tree[idx].y+maindash->tree[idx].h) {
  	         elc=duplicate_element(&(maindash->tree[i]));

	         elc.x+=new_x-maindash->tree[idx].x;
	         elc.y+=new_y-maindash->tree[idx].y;
                 add_element(maindash,&elc);
	         printf("Copy %d\n",i);
	    }
	  }
	}
        printf("Copy Element #%d from (%d,%d) to (%d,%d)\n",selected_element,maindash->tree[idx].x,maindash->tree[idx].y, 
  	  new_x,new_y);
        ELEMENT el=duplicate_element(&(maindash->tree[idx]));
        el.x=new_x;
	el.y=new_y;
	add_element(maindash,&el);
	is_modified=1;
	selected_element=-1;
	redraw_panel(widget);
      }
      break;
     case A_ADD:
     case A_RESIZE:
      if(selected_element>=0) {
        int idx=selected_element;
	int new_w=current_mouse_x-maindash->tree[idx].x;
	int new_h=current_mouse_y-maindash->tree[idx].y;
	if(new_w<5) new_w=5;  /* Always have a minimum size. */
	if(new_h<5) new_h=5;
	// if(new_w>mainwindow->w) new_w=mainwindow->w; /* Maybe allow bigger ?*/
	// if(new_h>mainwindow->h) new_h=mainwindow->h;
	if(do_grid) {
	  new_w-=(new_w%5);
	  new_h-=(new_h%5);
	}
        printf("Resize Element #%d from (%d,%d) to (%d,%d)\n",selected_element,maindash->tree[idx].w,maindash->tree[idx].h, 
	  new_w,new_h);
	scale_element(&(maindash->tree[idx]),new_w,new_h);

	is_modified=1;
	selected_element=-1;
	redraw_panel(widget);
      }
      break;
     case A_ADDGROUP:
       if(selected_element>=0) {
        int idx=selected_element;
	int new_w=current_mouse_x-maindash->tree[idx].x;
	int new_h=current_mouse_y-maindash->tree[idx].y;
	int new_x=maindash->tree[idx].x;
	int new_y=maindash->tree[idx].y;
	
	delete_element(maindash,idx);

	if(new_w<5) new_w=5;  /* Always have a minimum size. */
	if(new_h<5) new_h=5;
	// if(new_w>mainwindow->w) new_w=mainwindow->w; /* Maybe allow bigger ?*/
	// if(new_h>mainwindow->h) new_h=mainwindow->h;
	if(do_grid) {
	  new_w-=(new_w%5);
	  new_h-=(new_h%5);
	}
        printf("Add Element Group (%d,%d) (%dx%d)\n",new_x,new_y,new_w,new_h);

        char *text=(groups[current_group].function)(new_x,new_y,new_w,new_h);
        if(add_element_group(text)) {
	  is_modified=1;
	  selected_element=-1;
	  redraw_panel(widget);
	}
	free(text);
      }
      break;
    }
    update_statusline();
  }
  return TRUE;
}


static gboolean motion_notify_event(GtkWidget *widget,GdkEventMotion *event) {
  fix_pixmap(widget);
  GdkModifierType state;
  if(event->is_hint) {
    gdk_window_get_pointer (event->window, &current_mouse_x, &current_mouse_y, &state);
    if(current_mouse_x>mainwindow->w) current_mouse_x=mainwindow->w;
    if(current_mouse_y>mainwindow->h) current_mouse_y=mainwindow->h;
  } else {
    if(event->x<mainwindow->w && event->y<mainwindow->h) {
      current_mouse_x = event->x;
      current_mouse_y = event->y;
    }
    state=event->state;
  }
  update_statusline();
  if(state&GDK_BUTTON1_MASK && pixmap != NULL) {
    switch(current_action) {
    case A_MOVE:
    case A_COPY:
      if(selected_element>=0) {
        int mx=maindash->tree[selected_element].x+current_mouse_x-mouse_rel_x;
	int my=maindash->tree[selected_element].y+current_mouse_y-mouse_rel_y;
	int mw=maindash->tree[selected_element].w;
	int mh=maindash->tree[selected_element].h;
	if(mx<0) mx=0;
	if(my<0) my=0;
	if(mx>mainwindow->w) mx=mainwindow->w-1;
	if(my>mainwindow->h) my=mainwindow->h-1;
	static int omx,omy;
        if(converted_pixels) gdk_draw_rgb_32_image(pixmap,widget->style->white_gc,0,0,surface->w,surface->h,0,(const guchar *)converted_pixels,4*surface->w);
        gdk_draw_rectangle(pixmap,widget->style->white_gc,FALSE,mx, my,mw, mh);
        gtk_widget_queue_draw_area(widget,min(mx,omx),min(my,omy),max(mx,omx)-min(mx,omx)+mw+1,max(my,omy)-min(my,omy)+mh+1);
        omx=mx; 
	omy=my;
      }
      break;
    case A_RESIZE:
    case A_ADD:
    case A_ADDGROUP:
      if(selected_element>=0) {
        int mx=maindash->tree[selected_element].x;
	int my=maindash->tree[selected_element].y;
	int mw=current_mouse_x-maindash->tree[selected_element].x;
	int mh=current_mouse_y-maindash->tree[selected_element].y;
	if(mw<5) mw=5;
	if(mh<5) my=5;
	static int omw,omh;
        if(converted_pixels) gdk_draw_rgb_32_image(pixmap,widget->style->white_gc,0,0,surface->w,surface->h,0,(const guchar *)converted_pixels,4*surface->w);
        gdk_draw_rectangle(pixmap,widget->style->white_gc,FALSE,mx, my,mw, mh);
        gtk_widget_queue_draw_area(widget,mx,my,max(mw,omw)+1,max(mh,omh)+1);
        omw=mw; 
	omh=mh;
      }
      break;
    }
  }
  return TRUE;
}


#ifndef WINDOWS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

static int init_sdl() {
  static int done=0;
  if(done) return(0);
#ifndef WINDOWS
  XInitThreads();
#endif
//  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) return -1;
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0 ) return -1;
  
  /* Initialize SDL_ttf  */
    if(TTF_Init()==-1) return -1;
    
  /* Enable Unicode translation */
  SDL_EnableUNICODE(1);
  done=1;
  return(0);
}


WINDOW *open_pixmap(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h, int fullscreen) {
  WINDOW *nw=calloc(sizeof(WINDOW),1);
  init_sdl();
  printf("Create surface of size: %dx%d\n",w,h);
  surface = SDL_CreateRGBSurface(0, w, h, 32,0, 0, 0, 0);
  if (surface == NULL) {
      printf("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
      exit(1);
  }
  nw->display=surface;
  nw->x=nw->y=0;
  nw->w=surface->w;
  nw->h=surface->h;
  nw->fcolor=0xffffffff;
  nw->bcolor=0;
  if(nw->title) free(nw->title);
  nw->title=strdup(title);
  if(nw->info) free(nw->info);
  nw->info=strdup(info); 
  return(nw);
}

void close_pixmap(WINDOW *nw) {
  SDL_FreeSurface(nw->display);
  if(nw->title) free(nw->title);
  if(nw->info) free(nw->info);
  free(nw);
}

void update_title(const char *t) {
  char buf[256];
  if(window) {
    if(is_modified) snprintf(buf,sizeof(buf),"%s (modified)",t);
    else snprintf(buf,sizeof(buf),"%s",t);
    gtk_window_set_title (GTK_WINDOW (window),buf);
  }
}
void update_drawarea() {
  gtk_widget_set_size_request(GTK_WIDGET(drawing_area),maindash->tree[maindash->panelelement].w, maindash->tree[maindash->panelelement].h);
}

static gboolean on_enter(GtkWidget *darea, GdkEventCrossing *event) {
  gdk_window_set_cursor(window->window, cursor);
  return(TRUE);
}
static gboolean on_leave(GtkWidget *darea, GdkEventCrossing *event) {
  GdkDisplay *display = gtk_widget_get_display (darea);
  GdkCursor *dc=gdk_cursor_new_from_name(display,"default");
  gdk_window_set_cursor(window->window,dc);
  return(TRUE);
}
int main(int argc, char *argv[]) {
  GtkWidget *menu_bar;
  GtkWidget *scrollarea;
  GtkWidget *vbox;
  
  hyperdash_set_defaults();
  kommandozeile(argc,argv);    /* process command line */
  if(!exist(ifilename)) {
    char buf[strlen(ifilename)+1];
    strcpy(buf,ifilename);
    snprintf(ifilename,sizeof(ifilename),"%s/%s",dashboarddir,buf);
  }

  gtk_init (&argc, &argv);
  init_sdl();
    
  if(exist(ifilename)) maindash=load_dash(ifilename);
  else {
    maindash=new_dash(ifilename);
    if(verbose>=0) printf("could not load %s. New!\n",ifilename);
  }
  is_modified=0;
  init_dash(maindash);

  mainwindow=open_pixmap(maindash->tree[maindash->panelelement].text,PACKAGE_NAME,0,0,maindash->tree[maindash->panelelement].w,maindash->tree[maindash->panelelement].h,0);
    global_window=mainwindow; /* TODO */
    draw_dash(maindash,mainwindow);

    /* create a new window */
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(GTK_WIDGET (window), 350, 150);
    gtk_window_set_default_size(GTK_WINDOW (window), mainwindow->w+2, mainwindow->h+54);
    update_title(ifilename);
    g_signal_connect(window,"delete-event",G_CALLBACK (gtk_main_quit), NULL);

    menu_bar=init_menu(); /* Create the Menu */

    /* A vbox to put a menu and a button in: */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_show(vbox);
    gtk_box_pack_start (GTK_BOX (vbox), menu_bar, FALSE, FALSE, 2);
    gtk_widget_show (menu_bar);
    textarea = gtk_label_new("Init");
    gtk_widget_show (textarea);
    gtk_box_pack_start (GTK_BOX (vbox), textarea, FALSE, FALSE, 2);
    
    
    drawing_area=gtk_drawing_area_new();
    update_drawarea();
    gtk_widget_show (drawing_area);
    
  /* Signals used to handle backing pixmap */

  g_signal_connect(drawing_area,"expose_event",G_CALLBACK (expose_event), NULL);
  g_signal_connect(drawing_area,"configure_event",G_CALLBACK (configure_event), NULL);

  /* Event signals (mouse) */

  g_signal_connect(drawing_area,"motion_notify_event",G_CALLBACK(motion_notify_event), NULL);
  g_signal_connect(drawing_area,"button_press_event", G_CALLBACK(button_press_event),  NULL);
  g_signal_connect(drawing_area,"button_release_event",G_CALLBACK(button_release_event), NULL);
  g_signal_connect(drawing_area,"enter-notify-event",G_CALLBACK(on_enter), NULL);
  g_signal_connect(drawing_area,"leave-notify-event",G_CALLBACK(on_leave), NULL);

  gtk_widget_set_events(drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK| GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_ENTER_NOTIFY_MASK 
			 | GDK_LEAVE_NOTIFY_MASK);
    
    scrollarea=gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollarea),
                                   GTK_POLICY_AUTOMATIC,   //  horizontal ALWAYS/AUTOMATIC
                                   GTK_POLICY_AUTOMATIC);     //  vertical
  //  gtk_widget_set_size_request (GTK_WIDGET (scrollarea), -1, -1);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollarea), drawing_area);
  gtk_widget_show(scrollarea);
  gtk_box_pack_end(GTK_BOX (vbox), scrollarea, TRUE, TRUE, 2);
    
  /* always display the window as the last step so it all splashes on
   * the screen at once. */
  gtk_widget_show(window);
  menu_set_default();

  /* Main User Input Handling etc... */

  gtk_main ();

  if(is_modified) {
    printf("WARNING: Dashboard has not been saved!\n");
    /* Emergency-save the dashboard. */
    char newname[strlen(ifilename)+12];
    strcpy(newname,ifilename);
    strcat(newname,AUTOSAVE_ENDING);
    save_dash(maindash,newname);
  }
  close_pixmap(mainwindow);
  free_dash(maindash);
  return(EX_OK);
}
