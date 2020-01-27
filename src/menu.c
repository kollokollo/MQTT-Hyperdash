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


static void menu_move(MENUENTRY *me)     {current_action=A_MOVE;   update_statusline();}
static void menu_copy(MENUENTRY *me)     {current_action=A_COPY;   update_statusline();}
static void menu_identify(MENUENTRY *me) {current_action=A_NONE;   update_statusline();}
static void menu_resize(MENUENTRY *me)   {current_action=A_RESIZE; update_statusline();}
static void menu_mtb(MENUENTRY *me)      {current_action=A_MTB;    update_statusline();}
static void menu_add(MENUENTRY *me)      {current_action=A_ADD;    update_statusline();}
static void menu_delete(MENUENTRY *me)   {current_action=A_DELETE; update_statusline();}
static void menu_edit_prop(MENUENTRY *me){current_action=A_EDIT;   update_statusline();}
static void menu_set_foreground(MENUENTRY *me){current_action=A_SFGC;   update_statusline();}
static void menu_set_background(MENUENTRY *me){current_action=A_SBGC;   update_statusline();}
static void menu_set_font(MENUENTRY *me) {current_action=A_SFONT;   update_statusline();}

static void menu_undodelete(MENUENTRY *me) {
  printf ("%s\n",me->text);
  if(undo_element) {
    printf("Restore element...\n");
    add_element(maindash,undo_element);
    undo_element=NULL;
    is_modified=1;
    draw_dash(maindash,mainwindow);
 
    if(pixmap) g_object_unref(pixmap);
    pixmap=NULL;
    update_drawarea();
    update_title(ifilename);
    gtk_widget_queue_draw_area(drawing_area,0,0,mainwindow->w,mainwindow->h);
  }
}


static void menu_elements(char *typ) {
  int i;
  for(i=0;i<anzeltyp;i++) {
    if(!strcmp(typ,eltyps[i].name)) current_element=i;
  }
  if(current_element>=0) {
    current_action=A_ADD;
    update_statusline();
    printf("Elements: %s, opcode=%d\n",typ,current_element);
  }
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
  printf("grid: %d\n",do_grid);
  if(pixmap) g_object_unref(pixmap);
  pixmap=NULL;
  update_drawarea();
}
static void menu_select_fgc(MENUENTRY *me) {
  char buf[64];
  snprintf(buf,sizeof(buf),"$%s",tohex(current_fgc));
  int rc=colorselect_dialog("Select foregroud color",buf);
  if(rc) {
    current_fgc=(long)myatof(buf);
  }
}
static void menu_select_bgc(MENUENTRY *me) {
  char buf[64];
  snprintf(buf,sizeof(buf),"$%s",tohex(current_bgc));
  int rc=colorselect_dialog("Select backgroud color",buf);
  if(rc) {
    current_bgc=(long)myatof(buf);
  }
}
/* open a file selector to select a new font.*/
static void menu_select_font(MENUENTRY *me) {
  char buf[256];
  sprintf(buf,"%s.ttf",current_font);
  int rc=fileselect_dialog(buf,fontdir,"*.ttf");
  if(rc && *buf) {
    if(!fnmatch("*.ttf",buf,FNM_NOESCAPE)) {
      buf[strlen(buf)-4]=0;
    }
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
{0,"Undo delete",     menu_undodelete,NULL},
{0,"Delete Elements", menu_delete,NULL},
//{0,"Copy Elements",   menuitem_response,NULL},
//{0,"Paste Elements",  menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Edit Broker ...",menu_edit_broker,NULL},
{0,"-------------",NULL,NULL},
{0,"Edit Properties ...",menu_edit_prop,NULL},

{MENU_TITLE,"Edit",NULL,NULL},

{0,"# elements",  menu_elements,NULL},
{MENU_TITLE,"Element",NULL,NULL},
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
//{0,"Select Font",  menuitem_response,NULL},
//{0,"Select Layout",menuitem_response,NULL},
{0,"-------------",NULL,NULL},
{0,"Add Element",  menu_add,NULL},

{MENU_TITLE,"Action",NULL,NULL},

{0,"[X] Grid",menu_grid_onoff,NULL},
{0,"-------------",NULL,NULL},
{0,"Select foreground color ...",menu_select_fgc,NULL},
{0,"Select background color ...",menu_select_bgc,NULL},
{0,"Select text font ...",       menu_select_font,NULL},

{MENU_TITLE,"Settings",NULL,NULL},
{0,"About MQTT-Hyperdash DashDesign ...",about_dialog,NULL},
{MENU_TITLE,"About",NULL,NULL},

{0,NULL,NULL,NULL}
};
