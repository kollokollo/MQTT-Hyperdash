/* MENU.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <gtk/gtk.h>
#include "config.h"
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "input.h"
#include "dashdesign.h"
#include "util.h"
#include "menu.h"

static MENUENTRY menuentries[];

/* Initializes the menu */

GtkWidget *init_menu() {
  GtkWidget *menu;
  GtkWidget *menu_bar;
  int i=0;
  /* Init the menu-widget, and remember -- never
   * gtk_show_widget() the menu widget!! 
   * This is the menu that holds the menu items, the one that
   * will pop up when you click on the "Root Menu" in the app */
  menu=gtk_menu_new();
  /* Create a menu-bar to hold the menus for add it to main window */
  menu_bar=gtk_menu_bar_new();
    /* Next we make a little loop that makes menu-entries.
     * Notice the call to gtk_menu_shell_append.  Here we are adding a list of
     * menu items to our menu. */
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
	  GSList *group = NULL;
	  for(k=1;k<anzeltyp;k++) {
	    if(strcmp(eltyps[k].name,"BROKER") && strcmp(eltyps[k].name,"PANEL")) {
	      gw=gtk_radio_menu_item_new_with_label(group,eltyps[k].name);
	      group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (gw));
	      if(k==DEFAULT_ELEMENT) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gw),TRUE);
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
      gtk_widget_show(menuentries[i].widget);
    }
    i++;
  }
  return(menu_bar);
}

GdkCursor *cursor=NULL;
/* Set the mouse cursor which will be used when the mouse enters the 
   drawing area.*/
static void set_cursor(const char *name) {
  if(!strcmp(name,"delete"))     cursor=gdk_cursor_new(GDK_PIRATE);
  else if(!strcmp(name,"add"))   cursor=gdk_cursor_new(GDK_PLUS);
  else if(!strcmp(name,"edit"))  cursor=gdk_cursor_new(GDK_PENCIL);
  else if(!strcmp(name,"size"))  cursor=gdk_cursor_new(GDK_SIZING);
  else if(!strcmp(name,"color")) cursor=gdk_cursor_new(GDK_SPRAYCAN);
  else if(!strcmp(name,"?"))     cursor=gdk_cursor_new(GDK_QUESTION_ARROW);
  else cursor=gdk_cursor_new_from_name(gdk_display_get_default(),name);
}

static void menu_move(MENUENTRY *me)     {current_action=A_MOVE;   set_cursor("move");    update_statusline();}
static void menu_copy(MENUENTRY *me)     {current_action=A_COPY;   set_cursor("copy");    update_statusline();}
static void menu_identify(MENUENTRY *me) {current_action=A_NONE;   set_cursor("?");       update_statusline();}
static void menu_resize(MENUENTRY *me)   {current_action=A_RESIZE; set_cursor("size");    update_statusline();}
static void menu_mtb(MENUENTRY *me)      {current_action=A_MTB;    set_cursor("pointer"); update_statusline();}
static void menu_add(MENUENTRY *me)      {current_action=A_ADD;    set_cursor("add");     update_statusline();}
static void menu_delete(MENUENTRY *me)   {current_action=A_DELETE; set_cursor("delete");  update_statusline();}
static void menu_edit_prop(MENUENTRY *me){current_action=A_EDIT;   set_cursor("edit");    update_statusline();}
static void menu_set_foreground(MENUENTRY *me){current_action=A_SFGC;  set_cursor("color");   update_statusline();}
static void menu_set_background(MENUENTRY *me){current_action=A_SBGC;  set_cursor("color");   update_statusline();}
static void menu_set_font(MENUENTRY *me) {current_action=A_SFONT;  set_cursor("pointer"); update_statusline();}

static void menu_undodelete(MENUENTRY *me) {
  if(undo_element) {
    printf("Restore element...\n");
    add_element(maindash,undo_element);
    undo_element=NULL;
    is_modified=1;
    redraw_panel(drawing_area);
  }
}
static void menu_paste(MENUENTRY *me) {
  GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gchar *text = gtk_clipboard_wait_for_text(clip);
  printf("Clipboard: <%s>\n",text);
  if(text && *text) {
    ELEMENT elc;
    char *p1,*p2;
    int done=0;
    p2=p1=text;
    while(!done) {
      while(*p2 && *p2!='\n') p2++;
      if(*p2==0) {done=1;break;}
      else *p2=0;
      bzero(&elc,sizeof(ELEMENT));
      init_element(&elc,p1);
      printf("Add Element: <%s>\n",p1);
      add_element(maindash,&elc);
      p1=++p2;
    }
    is_modified=1;
    redraw_panel(drawing_area);
  }
}

static void menu_elements(char *typ) {
  int i;
  for(i=0;i<anzeltyp;i++) {
    if(!strcmp(typ,eltyps[i].name)) current_element=i;
  }
  if(current_element>=0) menu_add(NULL);
}

static void about_dialog(MENUENTRY *me) {
  message_dialog(PACKAGE_NAME " Info",
                 PACKAGE_NAME "\n===============\n\n Version " VERSION "\n\n" 
		 VERSION_DATE "\n\nDash-Designer\n\n (c) by Markus Hoffmann et. al.\n",1);

}
static void menu_new(MENUENTRY *me) {
/* Check if current file is nodified. 
   If so, open a warning dialog and offer 
   to save the file 
 */
  if(is_modified) emergency_save_dialog();
/* Then free the menu and create a new one. */  
  snprintf(ifilename,SIZEOF_IFILENAME,"new.dash");
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
    if(exist(newdash))  snprintf(ifilename,SIZEOF_IFILENAME,"%s",newdash);
    else snprintf(ifilename,SIZEOF_IFILENAME,"%s/%s",dashboarddir,newdash);
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
    snprintf(ifilename,SIZEOF_IFILENAME,"%s",newdash);
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
  if(pixmap) g_object_unref(pixmap);
  pixmap=NULL;
  update_drawarea();
}
static void menu_select_fgc(MENUENTRY *me) {
  char buf[64];
  snprintf(buf,sizeof(buf),"$%s",tohex(current_fgc));
  int rc=colorselect_dialog("Select foregroud color",buf);
  if(rc) current_fgc=(long)myatof(buf);
}
static void menu_select_bgc(MENUENTRY *me) {
  char buf[64];
  snprintf(buf,sizeof(buf),"$%s",tohex(current_bgc));
  int rc=colorselect_dialog("Select backgroud color",buf);
  if(rc) current_bgc=(long)myatof(buf);
}
/* open a file selector to select a new font.*/
static void menu_select_font(MENUENTRY *me) {
  char buf[256];
  sprintf(buf,"%s.ttf",current_font);
  int rc=fileselect_dialog(buf,fontdir,"*.ttf");
  if(rc && *buf) {
    if(!fnmatch("*.ttf",buf,FNM_NOESCAPE)) buf[strlen(buf)-4]=0;
    if(!strncmp(buf,fontdir,strlen(fontdir))) sprintf(current_font,"%s",buf+strlen(fontdir)+1);
    else sprintf(current_font,"%s",buf);
  }
}

static void menu_edit_broker(MENUENTRY *me) {
  int k=0;
  for(k=0;k<maindash->anzelement;k++) {
    if((maindash->tree[k].type&EL_BROKER)==EL_BROKER) {
      ELEMENT *el=&(maindash->tree[k]);
      if(edit_element(el)) {
        is_modified=1;
	printf("Broker changed: %s %s %s\n",el->filename,el->text, el->format);	
	if(el->text && (el->text[0]==0 || !strcmp(el->text,"(null)"))) {
	  free(el->text);
	  el->text=NULL;
	}
	if(el->format && (el->format[0]==0 || !strcmp(el->format,"(null)"))) {
	  free(el->format);
	  el->format=NULL;
	}
	printf("Broker changed: %s \n",element2a(el));
      }
      return;
    }
  }
}

static MENUENTRY menuentries[]={
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
{0,"Undo delete",     menu_undodelete,NULL},
{0,"Delete Element", menu_delete,NULL},
//{0,"Copy Elements",   menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Paste Elements",  menu_paste,NULL},
{0,"-------------",NULL,NULL},
{0,"Edit Broker ...",menu_edit_broker,NULL},
{0,"-------------",NULL,NULL},
{0,"Edit Properties ...",menu_edit_prop,NULL},

{MENU_TITLE,"Edit",NULL,NULL},

{0,"# elements",  menu_elements,NULL},
{MENU_TITLE,"Element",NULL,NULL},
#if 0
//{0,"Analog H scaler input with ticker",  menu_,NULL},
//{0,"Analog V scaler input with ticker",  menu_,NULL},
{0,"-------------",NULL,NULL},
//{0,"Analog value display",  menu_,NULL},
//{0,"Digital value display",  menu_,NULL},
//{0,"Analog Meter with value display",  menu_,NULL},
{MENU_TITLE,"Groups",NULL,NULL},
#endif
{0,"Identify Elements",  menu_identify,NULL},
{0,"-------------",NULL,NULL},
{0,"Move Element",      menu_move,NULL},
{0,"Copy Element",      menu_copy,NULL},
{0,"Resize Element",    menu_resize,NULL},
{0,"-------------",NULL,NULL},
{0,"Move to Background",menu_mtb,NULL},
{0,"-------------",NULL,NULL},
//{0,"Rotate left",  menuitem_response,NULL},
//{0,"Rotate right", menuitem_response,NULL},
{0,"-------------",NULL,NULL},
//{0,"Change Text",  menuitem_response,NULL},
//{0,"Change Topic", menuitem_response,NULL},
//{0,"Change Format",menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Set foreground color",menu_set_foreground,NULL},
{0,"Set background color",menu_set_background,NULL},
{0,"Set font",            menu_set_font,NULL},
{0,"-------------",NULL,NULL},
//{0,"Select Layout",menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Edit Element",menu_edit_prop,NULL},
{0,"-------------",NULL,NULL},
{0,"Add Element",  menu_add,NULL},

{MENU_TITLE,"Action",NULL,NULL},

{0,"[X] Grid",menu_grid_onoff,NULL},
{0,"-------------",NULL,NULL},
{0,"Select foreground color ...",menu_select_fgc,NULL},
{0,"Select background color ...",menu_select_bgc,NULL},
{0,"Select text font ...",       menu_select_font,NULL},

{MENU_TITLE,"Settings",NULL,NULL},
{0,"About " PACKAGE_NAME " DashDesign ...",about_dialog,NULL},
{MENU_TITLE,"About",NULL,NULL},

{0,NULL,NULL,NULL}
};
/* Set the default menu states....*/
void menu_set_default() {
  menu_identify(NULL); /* Set initial Action and mouse cursor. */
}
