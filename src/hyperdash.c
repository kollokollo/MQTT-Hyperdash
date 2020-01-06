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
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "mqtt.h"
#include "input.h"
#include "elements.h"


char icondir[256]="/usr/share/hyperdash/icons";
char bitmapdir[256]="/usr/share/hyperdash/bitmaps";
char fontdir[256]="/usr/share/fonts/truetype/msttcorefonts";
char dashboarddir[256]="/usr/share/hyperdash/dashboards";

/* Update Functions */


#define PI       3.141592653589793

const Sint16 zeigerx[7]={0, 200,400,500, 400,200,  0};
const Sint16 zeigery[7]={25, 25,100,  0,-100,-25,-25};

void u_meter(ELEMENT *el,WINDOW *win, char *message) {
  int i;
  double v=myatof(message);
  double phi;
  int lim=(int)(el->amax-el->amin);
  int flag=1;
  if(lim<0) {lim=-lim;flag=-1;}

/* SDL cannot draw arcs and segments of a circle, however this can be
implemented using polygons....
*/
  int anz=2+360;
  Sint16 vx[anz],vy[anz];
  phi=PI*el->amin/180;
  for(i=0;i<7;i++) {
    /* Rotate */
    vx[i]=(Sint16)( cos(phi)*(double)zeigerx[i]+sin(phi)*(double)zeigery[i]);
    vy[i]=(Sint16)(-sin(phi)*(double)zeigerx[i]+cos(phi)*(double)zeigery[i]);
    /* Scale */
    vx[i]=(Sint16)((double)vx[i]*(double)el->w/1100.0);
    vy[i]=(Sint16)((double)vy[i]*(double)el->h/1100.0);
    /* translate*/
    vx[i]+=el->x+el->w/2;
    vy[i]+=el->y+el->h/2;
  }
  filledPolygonColor(win->display,&vx[0],&vy[0],7,el->bgc);
  phi=PI*el->amax/180;
  for(i=0;i<7;i++) {
    /* Rotate */
    vx[i]=(Sint16)( cos(phi)*(double)zeigerx[i]+sin(phi)*(double)zeigery[i]);
    vy[i]=(Sint16)(-sin(phi)*(double)zeigerx[i]+cos(phi)*(double)zeigery[i]);
    /* Scale */
    vx[i]=(Sint16)((double)vx[i]*(double)el->w/1100.0);
    vy[i]=(Sint16)((double)vy[i]*(double)el->h/1100.0);
    /* translate*/
    vx[i]+=el->x+el->w/2;
    vy[i]+=el->y+el->h/2;
  }
  filledPolygonColor(win->display,&vx[0],&vy[0],7,el->bgc);

  phi=(v-el->min)/(el->max-el->min);
  if(phi<0) phi=0;
  if(phi>1) phi=1;
  phi=phi*PI/180*(el->amax-el->amin)+PI/180*el->amin;

  
  vx[0]=el->x+el->w/2;
  vy[0]=el->y+el->h/2;
 // printf("flag=%d, lim=%d\n",flag,lim);
  for(i=0;i<lim;i++) {
    if(flag>0) {
      vx[i+1]=el->w/2*cos(PI*((double)i+el->amin)/180);
      vy[i+1]=-el->h/2*sin(PI*((double)i+el->amin)/180);
    } else {
      vx[i+1]=el->w/2*cos(PI*((double)i+el->amax)/180);
      vy[i+1]=-el->h/2*sin(PI*((double)i+el->amax)/180);
    }
    /* translate*/
    vx[i+1]+=el->x+el->w/2;
    vy[i+1]+=el->y+el->h/2;
  }
  vx[i+1]=el->x+el->w/2;
  vy[i+1]=el->y+el->h/2;
  filledPolygonColor(win->display,&vx[0],&vy[0],i+2,el->bgc);
  
  polygonColor(win->display,&vx[0],&vy[0],i+2,el->agc);
if(el->min<0 && el->max>0) {
  double phi0=(0-el->min)/(el->max-el->min);
  phi0=phi0*PI/180*(el->amax-el->amin)+PI/180*el->amin;
  vx[0]=el->w/2*cos(phi0)+el->x+el->w/2;
  vy[0]=-el->h/2*sin(phi0)+el->y+el->h/2;
  vx[1]=el->w/2*0.8*cos(phi0)+el->x+el->w/2;
  vy[1]=-el->h/2*0.8*sin(phi0)+el->y+el->h/2;
  lineColor(win->display,vx[0],vy[0],vx[1],vy[1],el->fgc);
}

if(!isnan(v)) {
  for(i=0;i<7;i++) {
    /* Rotate */
    vx[i]=(Sint16)( cos(phi)*(double)zeigerx[i]+sin(phi)*(double)zeigery[i]);
    vy[i]=(Sint16)(-sin(phi)*(double)zeigerx[i]+cos(phi)*(double)zeigery[i]);
    /* Scale */
    vx[i]=(Sint16)((double)vx[i]*(double)el->w/1100.0);
    vy[i]=(Sint16)((double)vy[i]*(double)el->h/1100.0);
    /* translate*/
    vx[i]+=el->x+el->w/2;
    vy[i]+=el->y+el->h/2;
  }
  
  
  filledPolygonColor(win->display,&vx[0],&vy[0],7,el->fgc);
}
  filledEllipseColor(win->display,el->x+el->w/2,el->y+el->h/2,el->w/2/10,el->h/2/10,el->agc);
//  ellipseColor(win->display,el->x+el->w/2,el->y+el->h/2,el->w/2,el->h/2,el->agc);
}
void u_thmeter(ELEMENT *el,WINDOW *win, char *message) {
  double v=myatof(message);
  int x=el->x+35*el->h/120;
  int y=el->y;
  int w=el->w-2*35*el->h/120;
  int h=el->h;
  
  int x0=(int)((0-el->min)*(double)w/(el->max-el->min));
  int x1=(int)((v-el->min)*(double)w/(el->max-el->min));
  boxColor(win->display,el->x,el->y,el->x+el->w-1,el->y+el->h-1,el->bgc);
  x1=min(x1,w);
  x0=min(x0,w);
  if(x1<0) x1=0;
  if(x0<0) x0=0;
  
  int i,f;
  double vv;
  char buf[32];
  char *p,*q;
  double scale=1;
  while(fabs(scale*el->min)<1 && fabs(scale*el->min)<1 && scale<1e6) scale*=10; 
  for(i=x;i<x+w;i++) {
    vv=(el->max-el->min)*(i-x)/(double)w+el->min;
    sprintf(buf,"%g",vv*scale); 
    q=p=buf;
    while(*p) {
      if(isdigit(*p)) *q++=*p;
      p++;
    }
    *q=0;
    f=(buf[0])&1;
    if(f)
      lineColor(win->display,i,el->y,i,el->y+el->h/4,el->agc);
    else 
      lineColor(win->display,i,el->y,i,el->y+el->y/4,el->bgc);
    f=(buf[1])&1;
    if(f)
      lineColor(win->display,i,el->y+el->h/4,i,el->y+el->h/2,el->agc);
    else 
      lineColor(win->display,i,el->y+el->h/4,i,el->y+el->h/2,el->bgc);
  }
  rectangleColor(win->display,x,y,x+w,y+h/2,el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,x+x0,y,x+x0,y+h/2,el->fgc);
  if(!isnan(v)) {
  Sint16 vx[7],vy[7];
  vy[0]=-10;
  vx[0]=0;

  vy[1]=33;
  vx[1]=20;

  vy[2]=50;
  vx[2]=35;

  vy[3]=59;
  vx[3]=0;

  vy[4]=50;
  vx[4]=-35;

  vy[5]=33;
  vx[5]=-20;

  vy[6]=-10;
  vx[6]=0;

  for(i=0;i<7;i++) {
    /* Scale */
    vx[i]=(Sint16)((double)vx[i]*(double)h/120.0);
    vy[i]=(Sint16)((double)vy[i]*(double)h/120.0);
    /* translate*/
    vx[i]+=x+x1;
    vy[i]+=y+h/2;
  }
  filledPolygonColor(win->display,&vx[0],&vy[0],7,el->fgc);
  }
}
void u_tvmeter(ELEMENT *el,WINDOW *win, char *message) {
  double v=myatof(message);
  int x=el->x;
  int y=el->y+35*el->w/120;
  int w=el->w;
  int h=el->h-2*35*el->w/120;
  int y0=h-1-(int)((0-el->min)*(double)h/(el->max-el->min));
  int y1=h-1-(int)((v-el->min)*(double)h/(el->max-el->min));
  boxColor(win->display,el->x,el->y,el->x+el->w-1,el->y+el->h-1,el->bgc);
  y1=min(y1,h);
  y0=min(y0,h);
  if(y1<0) y1=0;
  if(y0<0) y0=0;
  
  int i,f;
  double vv;
  char buf[32];
  char *p,*q;
  double scale=1;
  while(fabs(scale*el->min)<1 && fabs(scale*el->min)<1 && scale<1e6) scale*=10; 
  for(i=y;i<y+h;i++) {
    vv=(el->max-el->min)*(y+h-i)/(double)h+el->min;
    sprintf(buf,"%g",vv*scale);
    q=p=buf;
    while(*p) {
      if(isdigit(*p)) *q++=*p;
      p++;
    }
    *q=0;
// printf("%03d: %s\n",i,buf);
    f=(buf[0])&1;
    if(f)
      lineColor(win->display,el->x,i,el->x+el->w/4,i,el->agc);
    else 
      lineColor(win->display,el->x,i,el->x+el->w/4,i,el->bgc);
    f=(buf[1])&1;
    if(f)
      lineColor(win->display,el->x+el->w/4,i,el->x+el->w/2,i,el->agc);
    else 
      lineColor(win->display,el->x+el->w/4,i,el->x+el->w/2,i,el->bgc);
  
  }
  
  rectangleColor(win->display,x,y,x+w/2,y+h,el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,x,y+y0,x+w/2,y+y0,el->fgc);
  if(!isnan(v)) {
    Sint16 vx[7],vy[7];
  vx[0]=-10;
  vy[0]=0;

  vx[1]=33;
  vy[1]=20;

  vx[2]=50;
  vy[2]=35;

  vx[3]=59;
  vy[3]=0;

  vx[4]=50;
  vy[4]=-35;

  vx[5]=33;
  vy[5]=-20;


  vx[6]=-10;
  vy[6]=0;

  for(i=0;i<7;i++) {
    /* Scale */
    vx[i]=(Sint16)((double)vx[i]*(double)w/120.0);
    vy[i]=(Sint16)((double)vy[i]*(double)w/120.0);

    
    /* translate*/
    vx[i]+=x+w/2;
    vy[i]+=y+y1;
  }
  filledPolygonColor(win->display,&vx[0],&vy[0],7,el->fgc);
}
}


/* Click Functions */
int c_panel(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==2) {
    printf("Panel clicked: %d\n",b);
    /*TODO: open a file selector to select a new dash to display.*/
message_dialog("MQTT Hyperdash Info","MQTT Hyperdash Version 1.00\nby Markus Hoffmann",1);
    return(1);
  } else if(b==3) {
    char newdash[256];
    char buf[256];

    printf("Panel clicked: %d\n",b);
    newdash[0]=0;
    /* open a file selector to select a new dash to display.*/
    int rc=fileselect_dialog(newdash,dashboarddir,"*.dash");
    if(rc && newdash[0]) {
      if(exist(newdash))  sprintf(buf,"hyperdash %s &",newdash);
      else sprintf(buf,"hyperdash %s/%s.dash &",dashboarddir,newdash);
      printf("Dash start: <%s>\n",newdash);
      if(system(buf)==-1) printf("Error: system\n");  
    }
    return(1);
  }
  return(0);
}
int c_tinstring(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    printf("input string for topic <%s>\n",el->topic);
    STRING a;
    char buf[256];
    char *def=subscriptions[el->subscription].last_value.pointer;
    int rc=input_dialog(el->topic,buf,def);
    if(rc>0) {
      a.pointer=buf;
      a.len=strlen(a.pointer);
      mqtt_publish(el->topic,a,el->revert,1);
    }
    return(1);
  }
  return(0);
}
int c_tinnumber(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    printf("input number for topic <%s> with format <%s>\n",el->topic,el->format);
    STRING a;
    char buf[256];
    char *def=subscriptions[el->subscription].last_value.pointer;
    int rc=input_dialog(el->topic,buf,def);
    if(rc>0) {
      double v=myatof(buf);
      if(v>el->max) v=el->max;
      if(v<el->min) v=el->min;
      STRING format;
      format.pointer=el->format;
      format.len=strlen(format.pointer);
      a=do_using(v,format);
      mqtt_publish(el->topic,a,el->revert,1);
      free(a.pointer);
    }
    return(1);
  }
  return(0);
}

int c_tinarea(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    printf("topic value cmd : <%s>\n",el->text);
    STRING a;
    a.pointer=el->text;
    a.len=strlen(a.pointer);
    mqtt_publish(el->topic,a,el->revert,1);
   // return(1); Do not consume it. There my be other action.... 
  }
  return(0);
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
 {EL_BROKER,                     "BROKER", i_broker,NULL,NULL},
 {EL_VISIBLE,                    "BOX",    i_box,d_box,NULL},
 {EL_VISIBLE,                    "CIRCLE", i_box,d_circle,NULL},
 {EL_VISIBLE,                    "LINE",   i_line,d_line,NULL},
 {EL_PANEL|EL_VISIBLE|EL_INPUT,  "PANEL",  i_panel,d_panel,NULL,c_panel},
 {EL_VISIBLE,                    "PBOX",   i_pbox,d_pbox,NULL},
 {EL_VISIBLE,                    "PCIRCLE",i_pbox,d_pcircle,NULL},
 {EL_VISIBLE,                    "FRAME",  i_frame,d_frame,NULL},
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"FRAMETOGGLE",i_frame,d_frame,NULL,c_frame},
 {EL_VISIBLE,                    "BITMAP", i_bitmap,d_bitmap,NULL},
 {EL_VISIBLE,                    "ICON",   i_icon,d_icon,NULL},
 {EL_VISIBLE,                    "TEXT",   i_string,d_string,NULL},
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICSTRING",i_tstring,d_tstring,u_tstring},
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICNUMBER",i_tnumber,d_tnumber,u_tnumber},
 {EL_VISIBLE|EL_DYNAMIC,         "HBAR",   i_bar,d_hbar,u_hbar,NULL},
 {EL_VISIBLE|EL_DYNAMIC,         "VBAR",         i_bar,d_vbar,u_vbar,NULL},
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICMETER"   ,i_meter,d_meter,u_meter,NULL}, /* obsolete */
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICVMETER"  ,i_bar,d_tvmeter,u_tvmeter,NULL},  /* obsolete */
 {EL_VISIBLE|EL_DYNAMIC,         "TOPICHMETER"  ,i_bar,d_thmeter,u_thmeter,NULL},  /* obsolete */
 {EL_VISIBLE|EL_DYNAMIC,         "METER"        ,i_meter,d_meter,u_meter,NULL},
 {EL_VISIBLE|EL_DYNAMIC,         "VMETER"       ,i_bar,d_tvmeter,u_tvmeter,NULL}, 
 {EL_VISIBLE|EL_DYNAMIC,         "HMETER"       ,i_bar,d_thmeter,u_thmeter,NULL}, 
 {EL_VISIBLE|EL_DYNAMIC,         "TEXTLABEL"    ,i_textlabel,d_textlabel,u_textlabel,NULL},
 {EL_VISIBLE|EL_DYNAMIC,         "BITMAPLABEL"  ,i_bitmaplabel,d_bitmaplabel,u_bitmaplabel,NULL},
 {EL_VISIBLE|EL_DYNAMIC,         "FRAMELABEL"   ,i_framelabel,d_framelabel,u_framelabel,NULL},
 {EL_INPUT,                      "SHELLCMD"     ,i_shellcmd,NULL,NULL,c_shellcmd},
 {EL_INPUT,                      "DASH"         ,i_subdash ,NULL,NULL,c_subdash},
 {EL_INPUT|EL_DYNAMIC,           "TOPICINAREA",  i_tinarea ,NULL,NULL,c_tinarea},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TOPICINSTRING",i_tinstring,d_subscribe,NULL,     c_tinstring},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TOPICINNUMBER",i_tinnumber,d_subscribe,NULL,     c_tinnumber},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TOPICHSCALER", i_scaler,   d_hscaler,  u_hscaler,c_hscaler}, /* obsolete */
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TOPICVSCALER", i_scaler,   d_vscaler,  u_vscaler,c_vscaler}, /* obsolete */
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"HSCALER",      i_scaler,   d_hscaler,  u_hscaler,c_hscaler},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"VSCALER",      i_scaler,   d_vscaler,  u_vscaler,c_vscaler},
 {EL_VISIBLE|EL_INPUT|EL_DYNAMIC,"TICKER",       i_tticker,  d_subscribe,NULL,     c_tticker},
 {EL_VISIBLE|EL_DYNAMIC,         "PLOT",         i_plot,     d_plot,     u_plot,   NULL},

};
const int anzeltyp=sizeof(eltyps)/sizeof(ELDEF);


static int click_element(ELEMENT *el, WINDOW *win, int x, int y,int b) {
  int j=(el->type&0xff);
  if(verbose>0) printf("click element %d with %d. %s\n",j,b,eltyps[j].name);
  if(eltyps[j].click) return( (eltyps[j].click)(el,win,x,y,b));
  return(0);
}


static void update_element(ELEMENT *el, WINDOW *win, STRING message) {
  int j=(el->type&0xff);
  if(eltyps[j].update) (eltyps[j].update)(el,win,message.pointer);
}
static void draw_element(ELEMENT *el, WINDOW *win) {
  int j=(el->type&0xff);
  if(eltyps[j].draw) (eltyps[j].draw)(el,win);
}

DASH *global_dash;
WINDOW *global_window;

void init_dash(DASH *dash) {
  /*
   Alle element typen aus Zeilen bestimen. 
   Broker finden und verbinden (client-id = panel name)
   x,y,w,h Parameter rausfinden.
   Alle topics subscribieren.
   
  */
  int i;
  char a[256];
  char b[256];
  global_dash=dash;

  for(i=0;i<dash->anzelement;i++) {
    xtrim(dash->tree[i].line,1,dash->tree[i].line);
    if(dash->tree[i].line[0]==0) {
      dash->tree[i].type=EL_IGNORE;    
    } else if(dash->tree[i].line[0]=='#') {
      dash->tree[i].type=EL_IGNORE;    
    } else {
      if(wort_sep(dash->tree[i].line,':',1,a, b)==2) {
        xtrim(a,1,a);
        xtrim(b,1,b);
    //    printf("Keyword: <%s> <%s>\n",a,b);
	int j;
        for(j=0;j<anzeltyp;j++) {
	  if(!strcmp(eltyps[j].name,a)) {
	    dash->tree[i].type=eltyps[j].opcode|(j&0xff);
	    // printf("found...%x\n",dash->tree[i].type);
	    if((dash->tree[i].type&EL_DYNAMIC)==EL_DYNAMIC) 
	      dash->tree[i].topic=strdup(key_value(b,"TOPIC","TOPIC"));
	    if((dash->tree[i].type&EL_VISIBLE)==EL_VISIBLE ||
	       (dash->tree[i].type&EL_INPUT)==EL_INPUT) {
	      dash->tree[i].x=atoi(key_value(b,"X","0"));
	      dash->tree[i].y=atoi(key_value(b,"Y","0"));
            }
	    if(eltyps[j].init) (eltyps[j].init)(&(dash->tree[i]),b);
	    break;
	  }
	}
        if(j==anzeltyp) printf("Unknown element #%d <%s>\n",i,dash->tree[i].line);
        else if((dash->tree[i].type&EL_PANEL)==EL_PANEL) dash->panelelement=i;
      } else printf("Unknown element #%d <%s>\n",i,dash->tree[i].line);
    }
  }
  
  /* Now do the subscriptions */
  
  
}
void free_element(ELEMENT *el) {
  free(el->topic);
  el->topic=NULL;
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


void draw_dash(DASH *dash, WINDOW *win) {
  int i;
  open_all_fonts();
  for(i=0;i<dash->anzelement;i++) {
    if((dash->tree[i].type&EL_VISIBLE)==EL_VISIBLE) draw_element(&(dash->tree[i]),win); 
  }
  SDL_Flip(win->display); 
  mqtt_subscribe_all();
}
void update_topic_message(int sub, STRING message) {
  DASH *dash=global_dash;
  WINDOW *win=global_window;
  int i;
 // printf("update_dash: <%s>...\n",topic);
  for(i=0;i<dash->anzelement;i++) {
    if((dash->tree[i].type&EL_DYNAMIC)==EL_DYNAMIC) {
      if(dash->tree[i].subscription==sub) update_element(&(dash->tree[i]),win,message);
    } 
  }
  SDL_Flip(win->display);
}




int handle_dash(DASH *dash, WINDOW *win) {
  int x,y,b,s;
  int a;
  while(1) {
    a=mouseevent(win,&x,&y,&b,&s);
    if(a==-1) return(0);
    else if(a==-2) {
      printf("Connection lost!\n");
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

DASH *load_dash(const char *filename) {
  DASH *dash=calloc(sizeof(DASH),1);
  return(merge_dash(dash,filename));
}
DASH *merge_dash(DASH *dash, const char *fname) {
  char b[256];
  FILE *dptr=fopen(fname,"rb"); 
  int len=lof(dptr);   /* Filelaenge rauskriegen */
  fclose(dptr);
  if(verbose>0) printf("<-- %s ",fname);
  dash->buffer=realloc(dash->buffer,dash->bufferlen+len+1);
  if(dash->name) {
    sprintf(b,"%s+%s",dash->name,fname);
    free(dash->name);
    dash->name=strdup(b);
  } else {
    dash->name=strdup(fname);
  }
  bload(fname,dash->buffer+dash->bufferlen,len);
  dash->bufferlen+=len;
  
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
    bzero(dash->tree+oldanz*sizeof(ELEMENT),(dash->anzelement-oldanz)*sizeof(ELEMENT));
 

    /* Zweiter Durchgang */

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
  if(verbose>0) printf("(%d elements)\n",dash->anzelement);
  return(dash);
}
