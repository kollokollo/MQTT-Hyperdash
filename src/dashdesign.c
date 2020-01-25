/* DASHDESIGN.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
/*
 * Thease are the sources for the MQTT Hyperdash dashboad grafical editor.
 * 
 * TODO: The whole thing it still al lot of mess.... help!
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "config.h"
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "input.h"
#include "dashdesign.h"

#if defined WINDOWS 
  #define EX_OK 0
#else
  #include <sysexits.h>
#endif

char ifilename[128]="new.dash";
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
char call_options[256]="";



int current_action=0;
int is_modified=0;

int current_mouse_x=0;
int current_mouse_y=0;

int do_grid=0;


SDL_Surface *surface;
GtkWidget *textarea;
/* Backing pixmap for drawing area */
static GdkPixmap *pixmap = NULL;


static void update_statusline();
static void update_title(const char *t);
static void update_drawarea();

static void intro() {
  printf("**********************************************************\n"
         "*  %10s  dashdesign          V." VERSION "            *\n"
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
    ,"dashdesign",ifilename,icondir,bitmapdir,dashboarddir,fontdir);
}
#define COLLECT()
#define COLLECT2()
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


/* Print a string when a menu item is selected */

static void menuitem_response(MENUENTRY *me) {
    printf ("%s\n",me->text);
}
static void menu_elements(char *typ) {
    printf("Elements: %s\n",typ);
}

static void about_dialog(MENUENTRY *me) {
  message_dialog(PACKAGE_NAME " Info",
                 PACKAGE_NAME "\n===============\n\n Version " VERSION "\n\n" 
		 VERSION_DATE "\n\nDash-Designer\n\n (c) by Markus Hoffmann et. al.\n",1);

}

static void init_newloaded_dash() {
  is_modified=0;
  init_dash(maindash);
  close_pixmap(mainwindow);
  mainwindow=open_pixmap(maindash->tree[maindash->panelelement].text,PACKAGE_NAME,0,0,maindash->tree[maindash->panelelement].w,maindash->tree[maindash->panelelement].h,0);
  global_window=mainwindow; /* TODO */
  draw_dash(maindash,mainwindow);

  if(pixmap) g_object_unref(pixmap);
  pixmap=NULL;
  update_title(ifilename);
  update_drawarea();
  update_statusline();
}

static void emergency_save_dialog() {
  printf("WARNING: Dashboard has not been saved!\n");
  char buffer[256];
  snprintf(buffer,sizeof(buffer),"ERROR:\nCurrent Dashboard has not yet been saved.\n"
  				 "Do you want to save it now?\n\n"
  				 "%s\n\n",
        			 ifilename);
  if(message_dialog("MQTT Hyperdash Dashdesign Warning",buffer,2)==1) {
    save_dash(maindash,ifilename);
    is_modified=0;
    update_title(ifilename);
  } else {    
    /* Emergency-save the dashboard. */
    char newname[strlen(ifilename)+12];
    strcpy(newname,ifilename);
    strcat(newname,".autosave");
    save_dash(maindash,newname);
  }
}

static void menu_new(MENUENTRY *me) {

/* Check if current file is nodified. If so open Warning dialog and offer to save the file */

  if(is_modified) emergency_save_dialog();
  
/* Then free the menu and create a new one. */
  
  snprintf(ifilename,sizeof(ifilename),"new.dash");
  free_dash(maindash);
  maindash=new_dash(ifilename);
  init_newloaded_dash();
}
static void menu_load(MENUENTRY *me) {
  char newdash[256];
  newdash[0]=0;
  /* Check if current file is nodified. If so open Warning dialog and offer to save the file */
  if(is_modified) emergency_save_dialog();

  /* open a file selector to select a new dash to display.*/
  int rc=fileselect_dialog(newdash,dashboarddir,"*.dash");
  if(rc && newdash[0]) {
    if(exist(newdash))  snprintf(ifilename,sizeof(ifilename),"%s",newdash);
    else snprintf(ifilename,sizeof(ifilename),"%s/%s",dashboarddir,newdash);
    free_dash(maindash); 
    if(exist(ifilename)) {
      maindash=load_dash(ifilename);
    } else {
      maindash=new_dash(ifilename);
      printf("ERROR: could not load %s. New!\n",ifilename);
    }
    init_newloaded_dash();
  }
}
static void menu_merge(MENUENTRY *me) {
  char newdash[256];
  char buf[256];
  newdash[0]=0;
  /* open a file selector to select a new dash to display.*/
  int rc=fileselect_dialog(newdash,dashboarddir,"*.dash");
  if(rc && newdash[0]) {
    if(exist(newdash))  snprintf(buf,sizeof(buf),"%s",newdash);
    else snprintf(buf,sizeof(buf),"%s/%s",dashboarddir,newdash);
    if(exist(buf)) {
      maindash=merge_dash(maindash,buf);
      init_newloaded_dash();
    }
  }
}
static void menu_save(MENUENTRY *me) {
  if(is_modified || 1) {  /* Do it anyways... */
    save_dash(maindash,ifilename);
    is_modified=0;
    update_title(ifilename);
  }
}
static void menu_save_as(MENUENTRY *me) {
  char newdash[256];
  newdash[0]=0;
  /* open a file selector to select a new dash save to.*/
  int rc=fileselect_dialog(newdash,dashboarddir,"*.dash");
  if(rc && newdash[0]) {
    if(exist(newdash)) {
      char buffer[256];
      snprintf(buffer,sizeof(buffer),"WARNING:\nSelected file \n%s \ndoes already exist.\n"
  				 "Do you want to overwrite it now?\n\n",
        			 newdash);
      if(message_dialog("MQTT Hyperdash Dashdesign Warning",buffer,2)!=1) return;
    }
    snprintf(ifilename,sizeof(ifilename),"%s",newdash);
    menu_save(me);
  }
}
static void menu_start_hyperdash(MENUENTRY *me) {
  if(is_modified) emergency_save_dialog();
  if(is_modified) {
    char newname[strlen(ifilename)+12];
    strcpy(newname,ifilename);
    strcat(newname,".autosave");
    call_a_dash(newname);
  } else call_a_dash(ifilename);
}
static void menu_quit(MENUENTRY *me) {
/* Check if current file is nodified. If so open Warning dialog and offer to save the file */
  if(is_modified) emergency_save_dialog();

  gtk_main_quit();
}

static void menu_grid_onoff(MENUENTRY *me) {
  do_grid=(int)gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(me->widget));
  printf("grid: %d\n",do_grid);
  if(pixmap) g_object_unref(pixmap);
  pixmap=NULL;
  update_drawarea();
}

static void menu_move(MENUENTRY *me)     {current_action=A_MOVE;   update_statusline();}
static void menu_identify(MENUENTRY *me) {current_action=A_NONE;   update_statusline();}
static void menu_resize(MENUENTRY *me)   {current_action=A_RESIZE; update_statusline();}
static void menu_mtb(MENUENTRY *me)      {current_action=A_MTB;    update_statusline();}
static void menu_add(MENUENTRY *me)      {current_action=A_ADD;    update_statusline();}
static void menu_delete(MENUENTRY *me)   {current_action=A_DELETE; update_statusline();}



#define MENU_TITLE 1
#define MENU_ENTRY 0

MENUENTRY menuentries[]={
{0,"New",          menu_new,NULL},
{0,"-------------",NULL,NULL},
{0,"Load ...",     menu_load,NULL},
{0,"Merge ...",    menu_merge,NULL},
{0,"-------------",NULL,NULL},
{0,"Save",         menu_save,NULL},
{0,"Save As ...",  menu_save_as,NULL},
{0,"-------------",NULL,NULL},
{0,"Run Dashboard",menu_start_hyperdash,NULL},
{0,"-------------",NULL,NULL},
{0,"Quit",         menu_quit,NULL},
{MENU_TITLE,"File",NULL,NULL},
{0,"Undo delete",     menuitem_response,NULL},
{0,"Delete Elements", menu_delete,NULL},
{0,"Copy Elements",   menuitem_response,NULL},
{0,"Paste Elements",  menuitem_response,NULL},

{MENU_TITLE,"Edit",NULL,NULL},

{0,"# elements",  menu_elements,NULL},
{MENU_TITLE,"Element",NULL,NULL},
{0,"Identify",  menu_identify,NULL},
{0,"-------------",NULL,NULL},
{0,"Move",      menu_move,NULL},
{0,"Resize",    menu_resize,NULL},
{0,"-------------",NULL,NULL},
{0,"Move to Background",menu_mtb,NULL},
{0,"-------------",NULL,NULL},
{0,"Rotate left",  menuitem_response,NULL},
{0,"Rotate right", menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Change Text",  menuitem_response,NULL},
{0,"Change Topic", menuitem_response,NULL},
{0,"Change Format",menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Select forground color",menuitem_response,NULL},
{0,"Select background color",menuitem_response,NULL},
{0,"Set default background",menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Select Font",  menuitem_response,NULL},
{0,"Select Layout",menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Add Element",  menu_add,NULL},

{MENU_TITLE,"Action",NULL,NULL},

{0,"[X] Grid",menu_grid_onoff,NULL},

{MENU_TITLE,"Settings",NULL,NULL},

{0,"About MQTT-Hyperdash dashdesign ...",about_dialog,NULL},
{MENU_TITLE,"About",NULL,NULL},

{0,NULL,NULL,NULL}
};

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

/* Draw a rectangle on the screen */
static void draw_brush(GtkWidget *widget, gdouble x, gdouble y) {
  GdkRectangle update_rect;
  update_rect.x = x - 4;
  update_rect.y = y - 4;
  update_rect.width = 8;
  update_rect.height = 8;
  gdk_draw_rectangle(pixmap,
		      widget->style->black_gc,
		      TRUE,
		      update_rect.x, update_rect.y,
		      update_rect.width, update_rect.height);
  gtk_widget_queue_draw_area(widget, 
		              update_rect.x, update_rect.y,
		              update_rect.width, update_rect.height);
}
static void update_statusline() {
  char status_text[256];

  snprintf(status_text,sizeof(status_text),"%d elements, (%dx%d), X=%d, Y=%d, Action: %s",
    maindash->anzelement,mainwindow->w,mainwindow->h,current_mouse_x,current_mouse_y,action_names[current_action]);
  gtk_label_set_text(GTK_LABEL(textarea),status_text);
}

int selected_element=-1;
int mouse_rel_x,mouse_rel_y;

static gboolean button_press_event(GtkWidget *widget,GdkEventButton *event) {
  fix_pixmap(widget);
  if(event->button==1 && pixmap!=NULL) {
    if(event->x<mainwindow->w && event->y<mainwindow->h) {
      current_mouse_x = event->x;
      current_mouse_y = event->y;
    }
    int idx=find_element(maindash,-1,current_mouse_x,current_mouse_y,0,0);
    
    switch(current_action) {
    case A_NONE:       /* Identify Elements under the mouse pointer */
      printf("(%d,%d): {\n",current_mouse_x,current_mouse_y);
      while(idx>=0) {
        printf("  Element #%d <%s>\n",idx,element2a(&maindash->tree[idx]));
	if(idx==0) break;
        idx=find_element(maindash,idx-1,current_mouse_x,current_mouse_y,0,0);      
      }
      printf("}\n");
      break;
    case A_MOVE:
    case A_RESIZE:
      if(!((maindash->tree[idx].type&EL_PANEL)==EL_PANEL)) {
        selected_element=idx;
        mouse_rel_x=current_mouse_x;
        mouse_rel_y=current_mouse_y;
	gdk_draw_rectangle(pixmap,widget->style->white_gc,FALSE,
	maindash->tree[idx].x, maindash->tree[idx].y,maindash->tree[idx].w, maindash->tree[idx].h);
        gtk_widget_queue_draw_area(widget,maindash->tree[idx].x, maindash->tree[idx].y,
	  maindash->tree[idx].w+1, maindash->tree[idx].h+1);
      } else selected_element=-1;
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
        printf("Move Element #%d from (%d,%d) to (%d,%d)\n",selected_element,maindash->tree[idx].x,maindash->tree[idx].y, 
	new_x,new_y);
        maindash->tree[idx].x=new_x;
        maindash->tree[idx].y=new_y;
	is_modified=1;
	selected_element=-1;
	draw_dash(maindash,mainwindow);
 
	if(pixmap) g_object_unref(pixmap);
        pixmap=NULL;
        update_drawarea();
        update_title(ifilename);
	gtk_widget_queue_draw_area(widget,0,0,mainwindow->w,mainwindow->h);
      }
      break;
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
        maindash->tree[idx].w=new_w;
        maindash->tree[idx].h=new_h;
	is_modified=1;
	selected_element=-1;
	draw_dash(maindash,mainwindow);
 
	if(pixmap) g_object_unref(pixmap);
        pixmap=NULL;
        update_drawarea();
        update_title(ifilename);
	gtk_widget_queue_draw_area(widget,0,0,mainwindow->w,mainwindow->h);

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
    default:
      draw_brush(widget, current_mouse_x, current_mouse_y);
    }
  }
  return TRUE;
}

void quit() {
  exit(0);
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


GtkWidget *window;
GtkWidget *drawing_area;

static void update_title(const char *t) {
  char buf[256];
  if(window) {
    if(is_modified) snprintf(buf,sizeof(buf),"%s (modified)",t);
    else snprintf(buf,sizeof(buf),"%s",t);
    gtk_window_set_title (GTK_WINDOW (window),buf);
  }
}
static void update_drawarea() {
  gtk_widget_set_size_request (GTK_WIDGET (drawing_area), maindash->tree[maindash->panelelement].w, maindash->tree[maindash->panelelement].h);
}

extern const int anzeltyp;
extern const ELDEF eltyps[];

int main(int argc, char *argv[]) {
    GtkWidget *menu;
    GtkWidget *menu_bar;
    GtkWidget *scrollarea;

    GtkWidget *vbox;
    
    int i;


    hyperdash_set_defaults();
    kommandozeile(argc,argv);    /* process command line */
    if(!exist(ifilename)) {
      char buf[strlen(ifilename)+1];
      strcpy(buf,ifilename);
      snprintf(ifilename,sizeof(ifilename),"%s/%s",dashboarddir,buf);
    }

    gtk_init (&argc, &argv);
    init_sdl();
    
    if(exist(ifilename)) {
      maindash=load_dash(ifilename);
    } else {
      maindash=new_dash(ifilename);
      printf("ERROR: could not load %s. New!\n",ifilename);
    }
    is_modified=0;
    init_dash(maindash);
    mainwindow=open_pixmap(maindash->tree[maindash->panelelement].text,PACKAGE_NAME,0,0,maindash->tree[maindash->panelelement].w,maindash->tree[maindash->panelelement].h,0);
    global_window=mainwindow; /* TODO */
    draw_dash(maindash,mainwindow);

    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 320, 200);
    update_title(ifilename);
    g_signal_connect(window, "delete-event",G_CALLBACK (gtk_main_quit), NULL);

    /* Init the menu-widget, and remember -- never
     * gtk_show_widget() the menu widget!! 
     * This is the menu that holds the menu items, the one that
     * will pop up when you click on the "Root Menu" in the app */
    menu = gtk_menu_new ();
    /* Create a menu-bar to hold the menus and add it to our main window */
    menu_bar = gtk_menu_bar_new ();

    /* Next we make a little loop that makes three menu-entries for "test-menu".
     * Notice the call to gtk_menu_shell_append.  Here we are adding a list of
     * menu items to our menu.  Normally, we'd also catch the "clicked"
     * signal on each of the menu items and setup a callback for it,
     * but it's omitted here to save space. */

     i=0;
     while(menuentries[i].text) {     
       if(menuentries[i].typ==MENU_TITLE) {
         /* Create a new menu-item with a name... */
	 menuentries[i].widget=gtk_menu_item_new_with_label(menuentries[i].text);
         /* Show the widget */
         gtk_widget_show (menuentries[i].widget);
         /* Now we specify that we want our newly created "menu" to be the menu
          * for the "root menu" */
         gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuentries[i].widget), menu);
         /* Init the menu-widget, and remember -- never
          * gtk_show_widget() the menu widget!! 
          * This is the menu that holds the menu items, the one that
          * will pop up when you click on the "Root Menu" in the app */
          menu = gtk_menu_new ();
          /* And finally we append the menu-item to the menu-bar -- this is the
           * "root" menu-item I have been raving about =) */
          gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar),menuentries[i].widget );
       } else {
         /* Create a new menu-item with a name... */
         if(*menuentries[i].text=='-') menuentries[i].widget=gtk_separator_menu_item_new();
	 else if(*menuentries[i].text=='[') menuentries[i].widget=gtk_check_menu_item_new_with_label(menuentries[i].text+4);
        else if(*menuentries[i].text=='#') {
	  menuentries[i].widget=gtk_separator_menu_item_new();
	  int k;
	  GtkWidget *gw;
	  for(k=1;k<anzeltyp;k++) {
	    if(strcmp(eltyps[k].name,"BROKER") && strcmp(eltyps[k].name,"PANEL")) {
	      gw=gtk_menu_item_new_with_label(eltyps[k].name);
              gtk_menu_shell_append(GTK_MENU_SHELL (menu),gw);
	      g_signal_connect_swapped(gw, "activate",
	    			  G_CALLBACK (menuentries[i].function), 
        			  (void *) eltyps[k].name); 
	      gtk_widget_show (gw);
	    }
	  }
	}
         else menuentries[i].widget=gtk_menu_item_new_with_label(menuentries[i].text);
         /* ...and add it to the menu. */
         gtk_menu_shell_append (GTK_MENU_SHELL (menu),menuentries[i].widget);
	 /* Do something interesting when the menuitem is selected */
	 g_signal_connect_swapped(menuentries[i].widget, "activate",
	    			  G_CALLBACK (menuentries[i].function), 
        			  (MENUENTRY *) &menuentries[i]);

         /* Show the widget */
         gtk_widget_show (menuentries[i].widget);
       }
       i++;
     }
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

  g_signal_connect (drawing_area, "expose_event",
		    G_CALLBACK (expose_event), NULL);
  g_signal_connect (drawing_area, "configure_event",
		    G_CALLBACK (configure_event), NULL);

     /* Event signals */

  g_signal_connect (drawing_area, "motion_notify_event",
		    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (drawing_area, "button_press_event",
		    G_CALLBACK (button_press_event), NULL);
  g_signal_connect (drawing_area, "button_release_event",
		    G_CALLBACK (button_release_event), NULL);

  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK| GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK);

    
    
    
    
    
    scrollarea=gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollarea),
                                   GTK_POLICY_AUTOMATIC,   //  horizontal ALWAYS/AUTOMATIC
                                   GTK_POLICY_AUTOMATIC);     //  vertical
  //  gtk_widget_set_size_request (GTK_WIDGET (scrollarea), -1, -1);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollarea), drawing_area);


    
    gtk_widget_show(scrollarea);
    gtk_box_pack_end (GTK_BOX (vbox), scrollarea, TRUE, TRUE, 2);
    
    /* always display the window as the last step so it all splashes on
     * the screen at once. */
    gtk_widget_show (window);
    update_statusline();

  /* Main User Input Handling etc... */

  gtk_main ();

  if(is_modified) {
    printf("WARNING: Dashboard has not been saved!\n");
    /* Emergency-save the dashboard. */
    char newname[strlen(ifilename)+12];
    strcpy(newname,ifilename);
    strcat(newname,".autosave");
    save_dash(maindash,newname);
  }
  close_pixmap(mainwindow);
  free_dash(maindash);
  return(EX_OK);
}

