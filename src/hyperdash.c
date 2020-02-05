/* HYPERDASH.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include <locale.h> 
#include "config.h"
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "mqtt.h"
#include "elements.h"
#include "input.h"

#ifdef WINDOWS
char icondir[256]="icons";
char bitmapdir[256]="bitmaps";
char fontdir[256]="C:/Windows/Fonts";
char dashboarddir[256]=".";
char hyperdashdir[256]=".";
#else
char icondir[256]="/usr/share/hyperdash/icons";
char bitmapdir[256]="/usr/share/hyperdash/bitmaps";
char fontdir[256]="/usr/share/fonts/truetype/msttcorefonts";
char dashboarddir[256]="/usr/share/hyperdash/dashboards";
char hyperdashdir[256]="."; /* Must be writable */
#endif
DASH *global_dash;
WINDOW *global_window;

  /* Set the default path where the .dash files are searched for. 
   * The path can be overridden by a commandline parameter.
   */
void hyperdash_set_defaults() {
  char path[256];
  char *envptr=getenv("HOME");
  if(envptr!=NULL) {
    snprintf(path,sizeof(path),"%s/.hyperdash",envptr);
    if(exist(path)) { /* It does exist! */
      strncpy(hyperdashdir,path,256);
    } else if(verbose>=0) printf("%s does not exist.\n",path);
    
    snprintf(path,sizeof(path),"%s/.hyperdash/dashboards",envptr);
    if(verbose>0) printf("Try dashboard path: %s\n",path);
    if(exist(path)) { /* It does exist! */
      strncpy(dashboarddir,path,256);
    } else if(verbose>=0) printf("%s does not exist.\n",path);
  } else if(verbose>=0) printf("$HOME not set.\n");
}

/* Element definitions for all element types. 
   You cann add new types or define aliases, if you want, but
   keep in mind that this should stay simple and too many 
   element types slows the dashboards down. 

Each element can have the flags:
  EL_VISIBLE  -- The element is visible, something needs to be drawn
  EL_DYNAMIC  -- The element updates with changing topic content
  EL_INPUT    -- The element takes user input (via mouse or keyboard)
  EL_BROKER   -- The element is a broker element
  EL_PANEL    -- The element is a panel element
  
And you must provide following functions for each element: 

  void i_element(ELEMENT *,char *)                  -- an initializing function
  void d_element(ELEMENT *,WINDOW *)                -- a drawing function
  void u_element(ELEMENT *,WINDOW *, char *)        -- an update function
  int  c_element(ELEMENT *,WINDOW *,int, int, int)  -- a click function
  
  */

const ELDEF eltyps[]= {
 {EL_IGNORE,                     "#",            NULL,         NULL,       NULL,     NULL,   NULL}, /* must be there to occupy entry 0 */
 {EL_BROKER,                     "BROKER",       i_broker,     NULL,       NULL,     NULL,   s_broker},
 {EL_PANEL|EL_VISIBLE|EL_INPUT,  "PANEL",        i_panel,      d_panel,    NULL,     c_panel,s_panel},
 {EL_COMPOUND|EL_VISIBLE,        "COMPOUND",     i_compound,   d_compound, NULL,     NULL,   s_compound},
 {EL_VISIBLE,                    "LINE",         i_line,       d_line,     NULL,     NULL,   s_line},
 {EL_VISIBLE,                    "BOX",          i_box,        d_box,      NULL,     NULL,   s_box},
 {EL_VISIBLE,                    "PBOX",         i_pbox,       d_pbox,     NULL,     NULL,   s_pbox},
 {EL_VISIBLE,                    "CIRCLE",       i_box,        d_circle,   NULL,     NULL,   s_box},
 {EL_VISIBLE,                    "PCIRCLE",      i_pbox,       d_pcircle,  NULL,     NULL,   s_pbox},
 {EL_VISIBLE,                    "FRAME",        i_frame,      d_frame,    NULL,     NULL,   s_frame},
 {EL_VISIBLE|EL_INPUT,           "FRAMETOGGLE",  i_frame,      d_frame,    NULL,     c_frame,s_frame},
 {EL_VISIBLE,                    "TEXT",         i_string,     d_string,   NULL,     NULL,   s_string},
 {EL_VISIBLE,                    "BITMAP",       i_bitmap,     d_bitmap,   NULL,     NULL,   s_bitmap},
 {EL_VISIBLE,                    "ICON",         i_icon,       d_icon,     NULL,     NULL,   s_icon},
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICSTRING",  i_tstring,    d_tstring,  u_tstring,NULL,   s_tstring},
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICNUMBER",  i_tnumber,    d_tnumber,  u_tnumber,NULL,   s_tnumber},
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"HBAR",         i_bar,        d_hbar,     u_hbar,   c_bar,  s_bar},
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"VBAR",         i_bar,        d_vbar,     u_vbar,   c_bar,  s_bar},
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"METER",        i_meter,      d_meter,    u_meter,  c_meter,  s_meter},
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"VMETER",       i_bar,	       d_tvmeter,  u_tvmeter,c_bar,  s_bar}, 
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"HMETER",       i_bar,	       d_thmeter,  u_thmeter,c_bar,  s_bar}, 
 {EL_VISIBLE|EL_DYNAMIC,         "TEXTLABEL"  ,  i_textlabel,  d_textlabel,u_textlabel,NULL, s_textlabel},
 {EL_VISIBLE|EL_DYNAMIC,         "BITMAPLABEL",  i_bitmaplabel,d_bitmaplabel,u_bitmaplabel,NULL, s_bitmaplabel},
 {EL_VISIBLE|EL_DYNAMIC,         "FRAMELABEL",   i_framelabel, d_framelabel,u_framelabel,NULL,s_framelabel},
 {EL_INPUT|EL_DYNAMIC,           "TOPICINAREA",  i_tinarea,    NULL,       NULL,     c_tinarea,  s_tinarea},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TOPICINSTRING",i_tinstring,  d_subscribe,NULL,   c_tinstring,s_tinstring},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TOPICINNUMBER",i_tinnumber,  d_subscribe,NULL,   c_tinnumber,s_tinnumber},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"HSCALER",      i_scaler,     d_hscaler,  u_hscaler,c_hscaler, s_scaler},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"VSCALER",      i_scaler,     d_vscaler,  u_vscaler,c_vscaler, s_scaler},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TICKER",       i_tticker,    d_subscribe,NULL,     c_tticker, s_ticker},
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"PLOT",         i_plot,       d_plot,     u_plot,   c_plot,    s_plot},
 {EL_VISIBLE|EL_DYNAMIC,         "SCMDLABEL",    i_scmdlabel,  d_subscribe,u_scmdlabel,NULL,    s_scmdlabel}, 
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICIMAGE",   i_timage,     d_subscribe,u_timage,  NULL,     s_timage}, 
 {EL_VISIBLE|EL_DYNAMIC,         "TEXTAREA",     i_textarea,   d_textarea, u_textarea,NULL,     s_textarea}, 
 {EL_INPUT,                      "SHELLCMD",     i_shellcmd,   NULL,       NULL,     c_shellcmd, s_shellcmd},
 {EL_INPUT,                      "DASH",         i_subdash,    NULL,       NULL,     c_subdash,  s_subdash},

};
const int anzeltyp=sizeof(eltyps)/sizeof(ELDEF);

char *element_get_current_value(ELEMENT *el) {
  static char buf[256];
  if(!el->subtopic) return(subscriptions[el->subscription].last_value.pointer);
  else {
    STRING a=json_get_value(el->subtopic,subscriptions[el->subscription].last_value);
    if(a.pointer) strncpy(buf,a.pointer,sizeof(buf));
    else *buf=0;
    free(a.pointer);
    return(buf);
  }
}


void publish_element(ELEMENT *el,STRING a, int qos) {
  if(!el->subtopic) mqtt_publish(el->topic,a,qos,1);
  else { /* Publish to subtopic */
    STRING b=json_replace_value(el->subtopic,subscriptions[el->subscription].last_value,a.pointer);
    mqtt_publish(subscriptions[el->subscription].topic,b,qos,1);
    free(b.pointer);
  }
}


static int click_element(ELEMENT *el, WINDOW *win, int x, int y,int b) {
  int j=(el->type&0xff);
  if(verbose>0) printf("click element %d with %d. %s\n",j,b,eltyps[j].name);
  if(eltyps[j].click) return( (eltyps[j].click)(el,win,x,y,b));
  return(0);
}

char *element2a(ELEMENT *el) {
  int j=(el->type&0xff);
  char *ret=NULL;
  if(j==EL_IGNORE) return(el->line);
  locale_t safe_locale = newlocale(LC_NUMERIC_MASK, "C", duplocale(LC_GLOBAL_LOCALE));
  locale_t old = uselocale(safe_locale);
  if(eltyps[j].tostring) ret=(eltyps[j].tostring)(el);
  uselocale(old);
  freelocale(safe_locale);
  return(ret);
}


static void update_element(ELEMENT *el, WINDOW *win, STRING message) {
  int j=(el->type&0xff);
  if(eltyps[j].update) (eltyps[j].update)(el,win,message.pointer,message.len);
}
static void draw_element(ELEMENT *el, WINDOW *win) {
  int j=(el->type&0xff);
  if(eltyps[j].draw) (eltyps[j].draw)(el,win);
}


void init_element(ELEMENT *el,const char *line) {
  char a[256*4];
  char b[256*4];
  el->type=EL_IGNORE;
  if(!line || *line==0)   return;
  if(*line=='#') return;    
  if(wort_sep(line,':',1,a,b)==2) {
    xtrim(a,1,a);
    xtrim(b,1,b);
    //        printf("Keyword: <%s> <%s>\n",a,b);
    int j;
    for(j=0;j<anzeltyp;j++) {
      if(!strcmp(eltyps[j].name,a)) {
  	el->type=eltyps[j].opcode|(j&0xff);
  	// printf("found...%x\n",dash->tree[i].type);
  	if((el->type&EL_DYNAMIC)==EL_DYNAMIC) 
  	  el->topic=strdup(key_value(b,"TOPIC","TOPIC"));
  	if((el->type&EL_VISIBLE)==EL_VISIBLE ||
  	   (el->type&EL_INPUT)==EL_INPUT) {
  	  el->x=atoi(key_value(b,"X","0"));
  	  el->y=atoi(key_value(b,"Y","0"));
        }
  	if(eltyps[j].init) (eltyps[j].init)(el,b);
  	break;
      }
    }
    if(j==anzeltyp) printf("Unknown element <%s>\n",line);
  } else printf("Unknown element <%s>\n",line);
}


void init_dash(DASH *dash) {
  /*
   Alle element typen aus Zeilen bestimen. 
   Broker finden und verbinden (client-id = panel name)
   x,y,w,h Parameter rausfinden.
   subscribe to all topics.
   
  */
  int i;
  global_dash=dash;  /* TODO */

  for(i=0;i<dash->anzelement;i++) {
    if(dash->tree[i].line[0]!='#') xtrim(dash->tree[i].line,1,dash->tree[i].line);
    if(dash->tree[i].line[0]==0) dash->tree[i].type=EL_IGNORE;    
    else if(dash->tree[i].line[0]=='#') dash->tree[i].type=EL_IGNORE;    
    else {
      init_element(&(dash->tree[i]),dash->tree[i].line);
      if((dash->tree[i].type&EL_PANEL)==EL_PANEL) dash->panelelement=i;
    }
  }
}

/* Creates a default element with default values depending on the elements type */

ELEMENT default_element(int type,unsigned long int fgc,unsigned long int bgc, const char *font) {
  ELEMENT rel;
  bzero(&rel,sizeof(ELEMENT));
  rel.type=eltyps[type].opcode|type;
  rel.w=32;
  rel.h=32;
  rel.fgc=fgc;
  rel.bgc=bgc;
  rel.agc=0xff00ff;
  rel.min=-1;
  rel.max=1;
  rel.increment=0.1;
  if((eltyps[type].opcode&EL_DYNAMIC)==EL_DYNAMIC) {
    rel.topic=strdup(DEFAULT_TOPIC);
  }
  if(!strcmp(eltyps[type].name,"BROKER")) {
    rel.text=strdup(DEFAULT_BROKER);
  } 
  if(!strcmp(eltyps[type].name,"TEXT") || 
            !strcmp(eltyps[type].name,"TEXTLABEL") ||
            !strcmp(eltyps[type].name,"TOPICSTRING") ||
	    !strcmp(eltyps[type].name,"TOPICNUMBER") ) {
    rel.text=strdup("Text");
    rel.fontsize=DEFAULT_FONTSIZE;
    rel.h=DEFAULT_FONTSIZE;
    rel.w=4*DEFAULT_FONTSIZE;
    rel.font=strdup(font);
  }
  if(!strcmp(eltyps[type].name,"TEXTAREA")) {
    rel.font=strdup(DEFAULT_FONT);  
    rel.fontsize=DEFAULT_FONTSIZE;
  }
  
  if(!strcmp(eltyps[type].name,"PANEL")) {
    rel.font=strdup(DEFAULT_FONT);  
  }
  if(!strcmp(eltyps[type].name,"TOPICNUMBER") || 
     !strcmp(eltyps[type].name,"TOPICINNUMBER")) {
    rel.format=strdup(DEFAULT_FORMAT);  
  }
  if(!strcmp(eltyps[type].name,"BITMAP")) {
    char f[256];
    rel.filename=strdup(DEFAULT_BITMAP);
    snprintf(f,sizeof(f),"%s/%s",bitmapdir,rel.filename);
    if(exist(f)) {
      int w,h;
      rel.data[0]=get_bitmap(f,&w,&h);
      if(verbose>0) printf("Bitmap: %dx%d\n",rel.w,rel.h);
      rel.w=w;
      rel.h=h;
    }
  }
  if(!strcmp(eltyps[type].name,"ICON")) {
    char f[256];
    int w=32,h=32;
    rel.filename=strdup(DEFAULT_ICON);  
    snprintf(f,sizeof(f),"%s/%s",icondir,rel.filename);
    if(exist(f)) {
      rel.data[0]=get_icon(f,&w,&h);
      rel.w=w;
      rel.h=h;
    } 
  }
  if(!strcmp(eltyps[type].name,"DASH")) {
    rel.text=strdup(DEFAULT_DASH);  
  }
  if(!strcmp(eltyps[type].name,"SHELLCMD")) {
    rel.text=strdup(DEFAULT_SHELLCMD);  
  }
  if(!strcmp(eltyps[type].name,"LINE")) {
    rel.x2=rel.x+rel.w;
    rel.y2=rel.y+rel.h;
  }
  if(!strcmp(eltyps[type].name,"METER")) {
    rel.amin=-225;
    rel.amax=45;
  }
  return(rel);
}

/* Scales properties of element according to type, 
   (e.g. font size) 
 */

void scale_element(ELEMENT *el,int w,int h) {
 // double scale_x=(double)w/(double)el->w;
  double scale_y=(double)h/(double)el->h;
  
  int new_fs=el->fontsize*scale_y;
  if(new_fs!=el->fontsize && el->font) {
    int i=add_font(el->font,new_fs);
    if(!fonts[i].font) open_font(i);
    el->fontnr=i;
  }
  el->fontsize=new_fs;
  if(strcmp(eltyps[el->type&0xff].name,"BITMAP") && 
     strcmp(eltyps[el->type&0xff].name,"ICON")) {
    el->w=w;
    el->h=h;
  }
  if(!strcmp(eltyps[el->type&0xff].name,"LINE")) {
    el->x2=el->x+el->w;
    el->y2=el->y+el->h;
  }
}

/* Opens a dialog to let the user edit all properties of an element. */
extern int s_maxval;
int edit_element(ELEMENT *el) {
  printf("Edit element: %s\n",eltyps[el->type&0xff].name);
  s_maxval=1;
  char *t=element2a(el);
  int rc=property_dialog(t);
  if(rc==1) {
    free_element(el);
    init_element(el,t);
  }
  s_maxval=0;
  return(rc);
}
ELEMENT duplicate_element(ELEMENT *el) {
  ELEMENT rel=*el;
  if(el->topic) rel.topic=strdup(el->topic);
  if(el->font)  rel.font=strdup(el->font);
  if(el->text)  rel.text=strdup(el->text);
  if(el->filename)  rel.filename=strdup(el->filename);
  if(el->format)  rel.format=strdup(el->format);
  
  int j;
  for(j=0;j<10;j++) {
    if(el->label[j].pointer) {
      rel.label[j].pointer=malloc(el->label[j].len);
      memcpy(rel.label[j].pointer,el->label[j].pointer,el->label[j].len);
    }
    if(el->data[j].pointer) {
      rel.data[j].pointer=malloc(el->data[j].len);
      memcpy(rel.data[j].pointer,el->data[j].pointer,el->data[j].len);
    }
    if(el->data2[j].pointer) {
      rel.data2[j].pointer=malloc(el->data2[j].len);
      memcpy(rel.data2[j].pointer,el->data2[j].pointer,el->data2[j].len);
    }
  }
  return(rel);
}



void free_element(ELEMENT *el) {
  free(el->topic);
  el->topic=NULL;
  free(el->subtopic);
  el->subtopic=NULL;
  free(el->font);
  el->font=NULL;
  free(el->text);
  el->text=NULL;
  free(el->filename);
  el->filename=NULL;
  free(el->format);
  el->format=NULL;
  int j;
  for(j=0;j<10;j++) {
    free(el->label[j].pointer);
    el->label[j].pointer=NULL;
    el->label[j].len=0;
    free(el->data[j].pointer);
    el->data[j].pointer=NULL;
    el->data[j].len=0;
    free(el->data2[j].pointer);
    el->data2[j].pointer=NULL;
    el->data2[j].len=0;
  }  
}


/* Undo actions from init_dash */

void close_dash(DASH *dash) {
  int i;
  mqtt_unsubscribe_all();     /* unsubscribe from all subscriptions */
  clear_all_subscriptions();  /* free the subscription list */
  mqtt_disconnect();  /* Verbindung zum Broker trennen. */ 
  for(i=0;i<dash->anzelement;i++) {
    free_element(&(dash->tree[i]));
  }
  clear_all_fonts();
}

/* Delete Element from list but do not free it.*/
void delete_element(DASH *dash, int idx) {
  if(idx<0 || idx>=dash->anzelement) return;
  if(idx==dash->anzelement-1) {
    dash->anzelement--;
    return;
  }
  int i;
  for(i=idx+1;i<dash->anzelement;i++) {
    dash->tree[i-1]=dash->tree[i];
  }
  dash->anzelement--;
}


/* Insert Element at position idx.*/
void insert_element(DASH *dash, int idx, ELEMENT *el) {
  if(idx<0 || idx>dash->anzelement) return;
  if(idx==dash->anzelement) return(add_element(dash,el));
  int i;
  i=dash->anzelement++;
  dash->tree=realloc(dash->tree,dash->anzelement*sizeof(ELEMENT));
  int j;
  for(j=i-1;j>=idx;j--) {
    dash->tree[j+1]=dash->tree[j];
  }
  dash->tree[idx]=*el;
}


void add_element(DASH *dash, ELEMENT *el) {
  int i;
  i=dash->anzelement++;
  dash->tree=realloc(dash->tree,dash->anzelement*sizeof(ELEMENT));
  dash->tree[i]=*el;
}

void draw_dash(DASH *dash, WINDOW *win) {
  int i;
  open_all_fonts();
  for(i=0;i<dash->anzelement;i++) {
    if((dash->tree[i].type&EL_VISIBLE)==EL_VISIBLE) draw_element(&(dash->tree[i]),win);
    else if(do_show_invisible) {
      draw_invisible_element(&(dash->tree[i]),win);
    }
  }
  SDL_Flip(win->display); 
  mqtt_subscribe_all();
}



void update_topic_message(int sub,const char *topic_name, STRING message) {
  if(sub<0) return;  /* Ignore it. */
  DASH *dash=global_dash;
  WINDOW *win=global_window;
  int i;
  for(i=0;i<dash->anzelement;i++) {
    if((dash->tree[i].type&EL_DYNAMIC)==EL_DYNAMIC) {
      if(dash->tree[i].subscription==sub) {
        if(!dash->tree[i].subtopic) update_element(&(dash->tree[i]),win,message);
        else {
	/*
	  printf("Update Element w subtopic:\n");
	  printf("mtopic =<%s>\n",topic_name);
	  printf("message=<%s>\n",message.pointer);
	  printf("etopic =<%s>\n",dash->tree[i].topic);
	  printf("esubtopic=<%s>\n",dash->tree[i].subtopic);
	  */
	  STRING submessage=json_get_value(dash->tree[i].subtopic,message);
          update_element(&(dash->tree[i]),win,submessage);
          free(submessage.pointer);
	}
      }
    } 
  }
  SDL_Flip(win->display);
}

/* Starting at the end of the element tree, find the first element which 
   is under the koordinates (x,y) and return its id. If not found
   -1 is returned.*/

int find_element(DASH *dash,int st, int x, int y, unsigned int mask, unsigned int match) {
  int i;
  if(st<0) st=dash->anzelement-1;
  for(i=st;i>=0;i--) {
    if((dash->tree[i].type&mask)==match) {
      if(dash->tree[i].x<=x && dash->tree[i].y<=y &&
  	 dash->tree[i].x+dash->tree[i].w>x &&
  	 dash->tree[i].y+dash->tree[i].h>y)
	 return(i);
    } 
  }
  return(i);
}


/* Handle the user input on a dashboard. */

int handle_dash(DASH *dash, WINDOW *win) {
  int x,y,b,s;
  int a;
  while(1) {
    a=mouseevent(win,&x,&y,&b,&s);
    if(a==-1) {  /* Window should be closed */
      return(0);
    } else if(a==-2) {
      printf(PACKAGE_NAME ": Connection lost!\n");
      return(-1);
    } else if(a==1) {
      if(s==1) {
        int i,consumed=0;
        for(i=dash->anzelement-1;i>=0;i--) {
          if((dash->tree[i].type&EL_INPUT)==EL_INPUT) {
	    if(dash->tree[i].x<=x && dash->tree[i].y<=y &&
	       dash->tree[i].x+dash->tree[i].w>x &&
	       dash->tree[i].y+dash->tree[i].h>y)
               consumed=click_element(&(dash->tree[i]),win,x,y,b);
	       if(consumed) break;
          } 
        }
      }
    }
  }
}



/* remove dash from memory and clear dashbuffer 
   basically undo everything from load_dash and init_dash
   */
void free_dash(DASH *dash) {
  while(--(dash->anzelement)>=0) {
    free_element(&(dash->tree[dash->anzelement]));
  }
  free(dash->tree);
  free(dash->name);
  free(dash->buffer);
  free(dash);
}

/* Prepare the dashboard after it has been loaded into the buffer. 
   All elements will be created.
 */

static void prepare_dash(DASH *dash) {
   /* Zeilenzahl herausbekommen */
  char *pos=dash->buffer;
  int oldanz=dash->anzelement;
  int i=0;

  dash->anzelement=0;
  
  /*Erster Durchgang */
  while(i<dash->bufferlen) {
      if(i<dash->bufferlen-1 && dash->buffer[i]=='\r' && dash->buffer[i+1]=='\n') {  /*DOS WINDOWS line ending behandeln.*/
        dash->buffer[i]='\n';
	dash->buffer[i+1]=2;  /* Marker */
      }
      if(i>0 && dash->buffer[i]=='\n' && dash->buffer[i-1]=='\\') {
        dash->buffer[i]=1;   /* Marker */
        dash->buffer[i-1]=' ';
      } else if(dash->buffer[i]==0 || dash->buffer[i]=='\n') {
        dash->buffer[i]=0;
	dash->anzelement++;
      } else if(dash->buffer[i]==9) dash->buffer[i]=' '; /* TABs entfernen*/
      i++;
    }
    if(i>0 && dash->buffer[i-1]!=0) dash->anzelement++;  /*letzte Zeile hatte kein \n*/
    dash->tree=(ELEMENT *)realloc(dash->tree,dash->anzelement*sizeof(ELEMENT));

    bzero(&(dash->tree[oldanz]),(dash->anzelement-oldanz)*sizeof(ELEMENT));


    /* Zweiter Durchgang */
    int len;
    len=i=0;
    while(i<dash->bufferlen) {
      if(dash->buffer[i]==1) {
        dash->buffer[i]=' '; /* Marker entfernen*/
	if(i<dash->bufferlen+1 &&  dash->buffer[i+1]==2) {
	  dash->buffer[i+1]=' '; /* Marker entfernen*/
	}
      } else if(dash->buffer[i]==2) {
        dash->buffer[i]=0; /* Marker entfernen*/
	pos++;  // alternativ: pos=dash->buffer+i+1;
      } else if(dash->buffer[i]==0) {
        dash->tree[len++].line=pos;
        pos=dash->buffer+i+1;
      }
      i++;
    }
  if((pos-dash->buffer)<dash->bufferlen) {
    dash->tree[len++].line=pos;  /* Potenzielle letzte Zeile ohne \n */
    dash->buffer[i]=0; /*stelle sicher dass die letzte Zeile durch ein 0 beendet ist*/
  }
}

/* Create a simple new dashboard to start with in dashdesign. */

DASH *new_dash(const char *filename) {
  DASH *dash=calloc(sizeof(DASH),1);
  dash->name=strdup(filename);
  dash->buffer=strdup(
    "# New Dashboard generated by dashdesign\n"
    "BROKER: URL=\"tcp://localhost:1883\"\n"
    "PANEL:  TITLE=\"MQTT-Hyperdash new.dash\" W=640 H=400 FGC=$ffffffff BGC=$40ff\n"
    "TEXT:   X=10 Y=20 FGC=$ffff00FF h=40 TEXT=\"Title\" FONT=\"Arial_Bold\" FONTSIZE=20\n"
  );
  dash->bufferlen=strlen(dash->buffer);
  prepare_dash(dash);
  return(dash);
}
/* load a dashboard from file */
DASH *load_dash(const char *filename) {
  DASH *dash=calloc(sizeof(DASH),1);
  return(merge_dash(dash,filename));
}
/* merge a dashboard file to the existing dashboard */
DASH *merge_dash(DASH *dash, const char *fname) {
  char b[256];
  FILE *dptr=fopen(fname,"rb"); 
  int len=lof(dptr);   /* Filelaenge rauskriegen */
  fclose(dptr);
  if(verbose>0) printf("<-- %s ",fname);
  dash->buffer=realloc(dash->buffer,dash->bufferlen+len+1);
  if(dash->name) {
    snprintf(b,sizeof(b),"%s+%s",dash->name,fname);
    free(dash->name);
    dash->name=strdup(b);
  } else {
    dash->name=strdup(fname);
  }
  bload(fname,dash->buffer+dash->bufferlen,len);
  dash->bufferlen+=len;
  prepare_dash(dash);
  if(verbose>=0) printf("(%d elements)\n",dash->anzelement);
  return(dash);
}
/* save a dashboard to file */
void save_dash(DASH *dash, const char *filename) {
  if(!filename) return;
  int i;
  if(exist(filename)) {
    char newname[strlen(filename)+8];
    strcpy(newname,filename);
    strcat(newname,".bck");
    printf("%s already exist.\nSave a backup copy %s.\n",filename,newname);
    int ret=rename(filename,newname);
    if(ret) printf("Error: unable to rename the file");
  }
  if(verbose>=0) printf("--> %s [",filename);
  FILE *fptr=fopen(filename,"w");
  if(fptr == NULL) {
      printf("Error opening file %s\n",filename);   
      return;
  }
  fprintf(fptr,"# saved by dashdesign. %s %s\n",date_string(),time_string());
  for(i=0;i<dash->anzelement;i++) {
    fprintf(fptr,"%s\n",element2a(&dash->tree[i]));
  }
  fclose(fptr);
  if(verbose>=0) printf("] (%d)\n",dash->anzelement);
}
extern char call_options[];
void call_a_dash(char *filename) {
  char buf[256];
  snprintf(buf,sizeof(buf),MQTT_HYPERDASH_EXECUTABLE_NAME "%s %s &",call_options,filename);
  if(verbose>=0) printf("Dash start: <%s>\n",filename);
  if(verbose>=0) printf("call: <%s>\n",buf);
  if(system(buf)==-1) printf(MQTT_HYPERDASH_EXECUTABLE_NAME "Error: system\n");
}
