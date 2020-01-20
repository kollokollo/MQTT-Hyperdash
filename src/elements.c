/* ELEMENTS.C (c) Markus Hoffmann  */

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

#include "config.h" 
 
 /*TODO: 

   1. TOPICINNUMBER: round to TIC value. 
                     allow to set the QoS in the input dialog.
   2. Scaler elements
   right-click opens a box where one can choose different ranges
   (full, medium (30%), fine (10%)) and where one can set the value 
   explicitly. Also the arrow keys should be active for tics.

   3. SOUNDLABEL element --> trigger different sounds

   4. Regular Expressions for MATCH

  */

#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "mqtt.h"
#include "input.h"
#include "elements.h"

/* Initialize the broker and try to connect... */

extern char *broker_override;
extern char *broker_user;
extern char *broker_passwd;

void i_broker(ELEMENT *el,char *pars) {
  int rc;
  if(broker_override) el->filename=strdup(broker_override);
  else                el->filename=strdup(key_value(pars,"URL",DEFAULT_BROKER));
  if(broker_user) el->text=strdup(broker_user);
  else            el->text=strdup(key_value(pars,"USER",""));
  if(broker_passwd) el->format=strdup(broker_passwd);
  else              el->format=strdup(key_value(pars,"PASSWD",""));
  if(!el->text[0]) {
    free(el->text);
    el->text=NULL;
  }
  if(!el->format[0]) {
    free(el->format);
    el->format=NULL;
  }
  el->x=0;
  el->y=0;
  el->w=0;
  el->h=0;
  /* connect to mqtt broker */
  rc=mqtt_broker(el->filename,el->text,el->format,NULL);
  while(rc==-1) {
    char buffer[256];
    snprintf(buffer,sizeof(buffer),"ERROR:\nCould not connect to the MQTT broker:\n"
                                   "%s\n\nUser=%s\n\nTry again?\n",
				   el->filename,el->text);
    if(message_dialog("MQTT Hyperdash Error",buffer,2)==1) {
      rc=mqtt_broker(el->filename,el->text,el->format,NULL);
    } else rc=0;
  }
  free(el->format);  /* such that the password will not persist in memory
                        longer than necessary
		      */
  el->format=NULL;
}

/* Initialoize the panel/window, get title and default values... */

void i_panel(ELEMENT *el,char *pars) {
  el->text=strdup(key_value(pars,"TITLE","Dashboard"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->w=atoi(key_value(pars,"W","640"));
  el->h=atoi(key_value(pars,"H","400"));
  /* FGC BGC */
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}

extern char *topic_prefix;

static char *make_topic(const char *n) {
  static char buf[256];
  if(topic_prefix && *topic_prefix) {
    strncpy(buf,topic_prefix,sizeof(buf));
    strncat(buf,"/",sizeof(buf)-strlen(buf)-1);
  } else *buf=0;
  strncat(buf,n,sizeof(buf)-strlen(buf)-1);
  return(buf);
}

/* Initialize Static drawing elements */
#define ELEMENT_FONT() el->fontnr=add_font(el->font,el->fontsize)
#define ELEMENT_SUBSCRIBE() el->subscription=add_subscription(make_topic(el->topic),0)


void i_line(ELEMENT *el,char *pars) {
  el->x2=atoi(key_value(pars,"X2","10"));
  el->y2=atoi(key_value(pars,"Y2","10"));
  el->w=el->x2-el->x;
  el->h=el->y2-el->y;
  el->linewidth=atoi(key_value(pars,"LINEWIDTH","1"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_box(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->linewidth=atoi(key_value(pars,"LINEWIDTH","1"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_pbox(ELEMENT *el,char *pars) {
  i_box(el,pars);
  el->bgc=(long)myatof(key_value(pars,"BGC","$000000ff"));
}
void i_string(ELEMENT *el,char *pars) {
  el->text=strdup(key_value(pars,"TEXT","TEXT"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  el->w=atoi(key_value(pars,"W","-1"));
  el->h=atoi(key_value(pars,"H","-1"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_bitmap(ELEMENT *el,char *pars) {
  char f[256];
  int w=32,h=32;
  el->filename=strdup(key_value(pars,"BITMAP","bitmap.bmp"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
  snprintf(f,sizeof(f),"%s/%s",bitmapdir,el->filename);
  if(exist(f)) {
    el->data[0]=get_bitmap(f,&w,&h);
    if(verbose>0) printf("Bitmap: %dx%d\n",w,h);
  } else {
    printf("Error: Bitmap %s not found!\n",f);
  }
  snprintf(f,sizeof(f),"%d",w);
  el->w=atoi(key_value(pars,"W",f));
  snprintf(f,sizeof(f),"%d",h);
  el->h=atoi(key_value(pars,"H",f));
}
void i_icon(ELEMENT *el,char *pars) {
  char f[256];
  int w=32,h=32;
  el->filename=strdup(key_value(pars,"ICON","icon.png"));
  el->agc=(long)myatof(key_value(pars,"TGC","$00"));  /* Transparent color */
  snprintf(f,sizeof(f),"%s/%s",icondir,el->filename);
  if(exist(f)) {
    el->data[0]=get_icon(f,&w,&h);
  } else {
    printf("Error: Icon %s not found!\n",f);
  }
  snprintf(f,sizeof(f),"%d",w);
  el->w=atoi(key_value(pars,"W",f));
  snprintf(f,sizeof(f),"%d",h);
  el->h=atoi(key_value(pars,"H",f));
}



void i_frame(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->revert=atoi(key_value(pars,"REVERT","0"));
}

void i_shellcmd(ELEMENT *el,char *pars) {
  el->text=strdup(key_value(pars,"CMD","xload &"));
  el->w=atoi(key_value(pars,"W","32"));
  el->h=atoi(key_value(pars,"H","32"));
}
void i_subdash(ELEMENT *el,char *pars) {
  el->text=strdup(key_value(pars,"DASH","main"));
  el->w=atoi(key_value(pars,"W","32"));
  el->h=atoi(key_value(pars,"H","32"));
}

/* Initialize dynamic drawing elements */
void i_framelabel(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->label[0].pointer=strdup(key_value(pars,"MATCH","0"));
  el->label[0].len=strlen(el->label[0].pointer);
}
void i_bar(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  
  el->bgc=(long)myatof(key_value(pars,"BGC","$000000ff"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff00ff"));
  el->agc=(long)myatof(key_value(pars,"AGC","$ffffffff"));
  /* MIN MAX */
  el->min=myatof(key_value(pars,"MIN","-1"));
  el->max=myatof(key_value(pars,"MAX","1"));
}
void i_meter(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  
  el->bgc=(long)myatof(key_value(pars,"BGC","$000000ff"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff00ff"));
  el->agc=(long)myatof(key_value(pars,"AGC","$ffffffff"));
  /* MIN MAX */
  el->min=myatof(key_value(pars,"MIN","-1"));
  el->max=myatof(key_value(pars,"MAX","1"));
  el->amin=myatof(key_value(pars,"AMIN","-225"));
  el->amax=myatof(key_value(pars,"AMAX","45"));
}
void i_plot(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","64"));
  el->h=atoi(key_value(pars,"H","32"));
  
  el->bgc=(long)myatof(key_value(pars,"BGC","$000000ff"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff00ff"));
  el->agc=(long)myatof(key_value(pars,"AGC","$ffffffff"));
  /* MIN MAX */
  el->min=myatof(key_value(pars,"MIN","-1"));
  el->max=myatof(key_value(pars,"MAX","1"));
  el->amin=myatof(key_value(pars,"AMIN","-1"));
  el->amax=myatof(key_value(pars,"AMAX","-1"));
  el->x2=myatof(key_value(pars,"N","-1"));
  el->y2=myatof(key_value(pars,"OFFSET","0"));
  el->revert=myatof(key_value(pars,"TYPE","0"));
  if(el->amin<0 || el->amax<0) {
    el->amin=0;
    el->amax=el->w;
  }
  if(el->x2<0) el->x2=el->w;
}



void i_textlabel(ELEMENT *el,char *pars) {
  int i;
  char p[256];
  char *a;
  char w1[256],w2[256],w3[256];

  for(i=0;i<10;i++) {
    snprintf(p,sizeof(p),"TEXT[%d]",i);
    a=key_value(pars,p,p);
    wort_sep(a,'|',0,w1,w2);
    wort_sep(w2,'|',0,w2,w3);
    el->label[i].pointer=strdup(w1);
    el->label[i].len=strlen(el->label[i].pointer);
    el->data[i].pointer=strdup(w2);
    el->data[i].len=strlen(el->data[i].pointer);
    el->labelcolor[i]=(long)myatof(w3);
  }

  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  el->h=atoi(key_value(pars,"H","-1"));
  el->w=atoi(key_value(pars,"W","32"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
}
void i_bitmaplabel(ELEMENT *el,char *pars) {
  int i;
  char p[256];
  char *a;
  char w1[256],w2[256],w3[256];
  char f[256];
  int w=32,h=32;
  for(i=0;i<10;i++) {
    snprintf(p,sizeof(p),"BITMAP[%d]",i);
    a=key_value(pars,p,p);
    wort_sep(a,'|',0,w1,w2);
    wort_sep(w2,'|',0,w2,w3);
    el->label[i].pointer=strdup(w1);
    el->label[i].len=strlen(el->label[i].pointer);
    if(*w2) {
    snprintf(f,sizeof(f),"%s/%s",bitmapdir,w2);
    if(exist(f)) {
      el->data[i]=get_bitmap(f,&w,&h);
      if(verbose>0) printf("Bitmap: <%s> %dx%d\n",f,w,h);
    } else {
      printf("Error: Bitmap %s not found!\n",f);
    }
    }
    el->labelcolor[i]=(long)myatof(w3);
  }

  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  snprintf(f,sizeof(f),"%d",w);
  el->w=atoi(key_value(pars,"W",f));
  snprintf(f,sizeof(f),"%d",h);
  el->h=atoi(key_value(pars,"H",f));
}

/* Shell command label. This is currently very universal and flexible, 
   you can call mpg123 and have a soundlabel.
 */
void i_scmdlabel(ELEMENT *el,char *pars) {
  int i;
  char p[256];
  char *a;
  char w1[256],w2[256];

  for(i=0;i<10;i++) {
    snprintf(p,sizeof(p),"CMD[%d]",i);
    a=key_value(pars,p,p);
    wort_sep(a,'|',1,w1,w2);
    el->label[i].pointer=strdup(w1);
    el->label[i].len=strlen(el->label[i].pointer);
    el->data[i].pointer=strdup(w2);
    el->data[i].len=strlen(el->data[i].pointer);
  }
  el->h=atoi(key_value(pars,"H","32"));
  el->w=atoi(key_value(pars,"W","32"));
}

void i_tstring(ELEMENT *el,char *pars) {
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  el->w=atoi(key_value(pars,"W","-1"));
  el->h=atoi(key_value(pars,"H","-1"));
  /* FGC BGC  */  
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_textarea(ELEMENT *el,char *pars) {
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  el->w=atoi(key_value(pars,"W","128"));
  el->h=atoi(key_value(pars,"H","64"));
  /* FGC BGC  */  
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
  /* ALIGN */
  el->format=strdup(key_value(pars,"ALIGN","TOP"));
}
void i_tnumber(ELEMENT *el,char *pars) {
  el->format=strdup(key_value(pars,"FORMAT","###.###"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  el->w=atoi(key_value(pars,"W","-1"));
  el->h=atoi(key_value(pars,"H","-1"));
  /* FGC BGC  */
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}

void i_timage(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","20"));
  el->h=atoi(key_value(pars,"H","20"));
}



void i_tinarea(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","20"));
  el->h=atoi(key_value(pars,"H","20"));
  el->text=strdup(key_value(pars,"VALUE","0"));
  el->revert=atoi(key_value(pars,"QOS","0"));
}
void i_tinstring(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","20"));
  el->h=atoi(key_value(pars,"H","20"));
  el->revert=atoi(key_value(pars,"QOS","0"));
}
void i_scaler(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","20"));
  el->h=atoi(key_value(pars,"H","20"));
  el->revert=atoi(key_value(pars,"QOS","0"));
  el->min=myatof(key_value(pars,"MIN","0"));
  el->max=myatof(key_value(pars,"MAX","1"));
  el->amin=myatof(key_value(pars,"AMIN","-1"));
  el->amax=myatof(key_value(pars,"AMAX","-1"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$000000ff"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$808080ff"));
  el->agc=(long)myatof(key_value(pars,"AGC","$ffffffff"));
  el->increment=myatof(key_value(pars,"TIC","0"));
  el->format=strdup(key_value(pars,"FORMAT","%g"));
}
void i_tinnumber(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","20"));
  el->h=atoi(key_value(pars,"H","20"));
  el->revert=atoi(key_value(pars,"QOS","0"));
  el->format=strdup(key_value(pars,"FORMAT","%g"));
  el->min=myatof(key_value(pars,"MIN","-1"));
  el->max=myatof(key_value(pars,"MAX","1"));
  el->increment=myatof(key_value(pars,"TIC","0.1"));
  el->format=strdup(key_value(pars,"FORMAT","%g"));
}

void i_tticker(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","20"));
  el->h=atoi(key_value(pars,"H","20"));
  el->revert=atoi(key_value(pars,"QOS","0"));
  /* MIN MAX */
  el->min=myatof(key_value(pars,"MIN","-1"));
  el->max=myatof(key_value(pars,"MAX","1"));
  el->increment=myatof(key_value(pars,"TIC","0"));
  el->format=strdup(key_value(pars,"FORMAT","%g"));
}
void d_subscribe(ELEMENT *el,WINDOW *win) {
  ELEMENT_SUBSCRIBE();
}

/* Drawing functions for all elements....*/

void d_panel(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
}
void d_line(ELEMENT *el,WINDOW *win) {
  lineColor(win->display,el->x,el->y,el->x2,el->y2,el->fgc);
}
void d_box(ELEMENT *el,WINDOW *win) {
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->fgc);
}
void d_pbox(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->fgc);
}
void d_circle(ELEMENT *el,WINDOW *win) {
  ellipseColor(win->display,el->x+el->w/2,el->y+el->h/2,el->w/2,el->h/2,el->fgc);
}
void d_pcircle(ELEMENT *el,WINDOW *win) {
  filledEllipseColor(win->display,el->x+el->w/2,el->y+el->h/2,el->w/2,el->h/2,el->bgc);
  ellipseColor(win->display,el->x+el->w/2,el->y+el->h/2,el->w/2,el->h/2,el->fgc);
}

void d_string(ELEMENT *el,WINDOW *win) {
  if(el->w<0) el->w=(int)strlen(el->text)*fonts[el->fontnr].height;
  if(el->h<0) el->h=fonts[el->fontnr].height;
  put_font_text(win->display,el->fontnr,el->text,el->x,el->y,el->fgc,el->h);
}

void d_frame(ELEMENT *el,WINDOW *win) {
  unsigned long ac,bc;
  if(el->revert==1) {
    ac=0x1f1f1fff;
    bc=0xafafafff;
  } else {
    bc=0x1f1f1fff;
    ac=0xafafafff;
  }
  lineColor(win->display,el->x,el->y,el->x+el->w,el->y,ac);
  lineColor(win->display,el->x,el->y+1,el->x+el->w-1,el->y+1,ac);

  lineColor(win->display,el->x  ,el->y,el->x,el->y+el->h  ,ac);
  lineColor(win->display,el->x+1,el->y,el->x+1,el->y+el->h-1,ac);

  lineColor(win->display,el->x+el->w  ,el->y  ,el->x+el->w,el->y+el->h    ,bc);
  lineColor(win->display,el->x+el->w-1,el->y+1,el->x+el->w-1,el->y+el->h-2,bc);

  lineColor(win->display,el->x,el->y+el->h  ,el->x+el->w  ,el->y+el->h  ,bc);
  lineColor(win->display,el->x+1,el->y+el->h-1,el->x+el->w-2,el->y+el->h-1,bc);
}
void d_bitmap(ELEMENT *el,WINDOW *win) {
  if(el->data[0].pointer)
  put_bitmap(win,el->data[0].pointer,el->x,el->y,el->w,el->h,el->fgc);
}
void d_icon(ELEMENT *el,WINDOW *win) {
  put_graphics(win,el->data[0],el->x,el->y,el->w,el->h,el->agc);
}

/* Drawing Dynamic elements*/


void d_bitmaplabel(ELEMENT *el,WINDOW *win) {
  if(el->data[0].pointer) {
    put_bitmap(win,el->data[0].pointer,el->x,el->y,el->w,el->h,el->labelcolor[0]);
  }
  ELEMENT_SUBSCRIBE();
}
void d_framelabel(ELEMENT *el,WINDOW *win) {
  d_frame(el,win);
  ELEMENT_SUBSCRIBE();
}

void d_hbar(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->agc);
  int x=(int)(0-el->min)*el->w/(el->max-el->min);
  if(el->min<0 && el->max>0) lineColor(win->display,el->x+x,el->y,el->x+x,el->y+el->h,el->agc);
  ELEMENT_SUBSCRIBE();
}
void d_textarea(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  ELEMENT_SUBSCRIBE();
}



void d_vbar(ELEMENT *el,WINDOW *win) {
  int y=el->h-1-(int)(0-el->min)*el->h/(el->max-el->min);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,el->x,el->y+y,el->x+el->w,el->y+y,el->agc);
  ELEMENT_SUBSCRIBE();
}
void d_tvmeter(ELEMENT *el,WINDOW *win) {
  u_tvmeter(el,win,"NaN",3);
  ELEMENT_SUBSCRIBE();
}
void d_thmeter(ELEMENT *el,WINDOW *win) {
  u_thmeter(el,win,"NaN",3);
  ELEMENT_SUBSCRIBE();
}
void d_meter(ELEMENT *el,WINDOW *win) {
  u_meter(el,win,"NaN",3);
  ELEMENT_SUBSCRIBE();
}
void d_hscaler(ELEMENT *el,WINDOW *win) {
  u_hscaler(el,win,"NaN",3);
  ELEMENT_SUBSCRIBE();
}
void d_vscaler(ELEMENT *el,WINDOW *win) {
  u_vscaler(el,win,"NaN",3);
  ELEMENT_SUBSCRIBE();
}
void d_tstring(ELEMENT *el,WINDOW *win) {
  if(el->w<0) el->w=(int)strlen(el->topic)*fonts[el->fontnr].height;
  if(el->h<0) el->h=fonts[el->fontnr].height;
  ELEMENT_SUBSCRIBE();
}
void d_textlabel(ELEMENT *el,WINDOW *win) {
  if(el->h<0) el->h=fonts[el->fontnr].height;
  if(el->data[0].pointer) {
    put_font_text(win->display,el->fontnr,el->data[0].pointer,el->x,el->y,el->labelcolor[0],el->h);
  }
  ELEMENT_SUBSCRIBE();
}
void d_tnumber(ELEMENT *el,WINDOW *win) {
  if(el->w<0) el->w=(int)strlen(el->format)*fonts[el->fontnr].height;
  if(el->h<0) el->h=fonts[el->fontnr].height;

  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  put_font_text(win->display,el->fontnr,el->format,el->x,el->y,el->fgc,el->h);
  ELEMENT_SUBSCRIBE();
}
void d_plot(ELEMENT *el,WINDOW *win) {
  u_plot(el,win,"",0);
  ELEMENT_SUBSCRIBE();
}


/* Update drawing functions for all dynamic elements....*/


void u_tstring(ELEMENT *el,WINDOW *win,char *message, int len) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  put_font_text(win->display,el->fontnr,message,el->x,el->y,el->fgc,el->h);
}
void u_tnumber(ELEMENT *el,WINDOW *win, char *message, int len) {
  STRING format;
  format.pointer=el->format;
  format.len=strlen(format.pointer);
  double v=myatof(message);
  STRING a=do_using(v,format);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  put_font_text(win->display,el->fontnr,a.pointer,el->x,el->y,el->fgc,el->h);
  free(a.pointer);
}
void u_textlabel(ELEMENT *el,WINDOW *win, char *message, int len) {
  int i;
  int found=-1;
  for(i=0;i<10;i++) {
    if(el->label[i].pointer) {
      if(!strcmp(el->label[i].pointer,message)) found=i;   
    }
  }
  if(found>=0 && found<10 && el->data[found].pointer) {
    boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
    put_font_text(win->display,el->fontnr,el->data[found].pointer,el->x,el->y,el->labelcolor[found],el->h);
  }
}

/* TODO: */

void u_textarea(ELEMENT *el,WINDOW *win,char *message, int len) {
 // printf("Received long text. Len=%d\n",len);
 // printf("This is the text:\n%s\n",message);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  put_font_long_text(win->display,el->fontnr,message,el->x,el->y,el->fgc,el->w,el->h);
}
void u_scmdlabel(ELEMENT *el,WINDOW *win, char *message, int len) {
  int i;
  int found=-1;
  for(i=0;i<10;i++) {
    if(el->label[i].pointer) {
      if(!strcmp(el->label[i].pointer,message)) found=i;   
    }
  }
  if(found>=0 && found<10 && el->data[found].pointer) {
    if(verbose>0) printf("Shell cmd : <%s>\n",el->data[found].pointer);
    if(system(el->data[found].pointer)==-1) printf("Error: system\n");
  }
}
void u_framelabel(ELEMENT *el,WINDOW *win, char *message, int len) {
  el->revert=0;
  if(el->label[0].pointer) {
    if(!strcmp(el->label[0].pointer,message)) el->revert=1;   
  }
  d_frame(el,win);
}
void u_bitmaplabel(ELEMENT *el,WINDOW *win, char *message, int len) {
  int i;
  int found=-1;
  for(i=0;i<10;i++) {
    if(el->label[i].pointer) {
      if(!strcmp(el->label[i].pointer,message)) found=i;   
    }
  }
  if(found>=0 && found<10 && el->data[found].pointer) {
    boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
    put_bitmap(win,el->data[found].pointer,el->x,el->y,el->w,el->h,el->labelcolor[found]);
  }
}
/* TODO: We should clip the image to a defined size or scale it....
   maybe we should also allow .jpeg image data via SDL_image or so...*/

void u_timage(ELEMENT *el,WINDOW *win,char *message, int len) {
  // printf("Update with image data: len=%d\n",len);
  STRING ret;
  ret=pngtobmp((unsigned char *)message,(size_t)len);
  // printf("After decoding: len=%d\n",ret.len);
  put_graphics(win,ret,el->x,el->y,el->w,el->h,el->agc);
}


void u_hbar(ELEMENT *el,WINDOW *win, char *message, int len) {
  double v=myatof(message);
  int x0=(int)((0-el->min)*(double)el->w/(el->max-el->min));
  int x=(int)((v-el->min)*(double)el->w/(el->max-el->min));
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  x=min(x,el->w);
  x0=min(x0,el->w);
  if(x<0) x=0;
  if(x0<0) x0=0;
  if(x>x0)
    boxColor(win->display,el->x+x0,el->y,(el->x)+x-1,(el->y)+(el->h)-1,el->fgc);
  else
    boxColor(win->display,el->x+x,el->y,(el->x)+x0-1,(el->y)+(el->h)-1,el->fgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,el->x+x0,el->y,el->x+x0,el->y+el->h,el->agc);
}
void u_vbar(ELEMENT *el,WINDOW *win, char *message, int len) {
  double v=myatof(message);
  int y0=el->h-1-(int)((0-el->min)*(double)el->h/(el->max-el->min));
  int y=el->h-1-(int)((v-el->min)*(double)el->h/(el->max-el->min));
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  y=min(y,el->h);
  y0=min(y0,el->h);
  if(y<0) y=0;
  if(y0<0) y0=0;
  
  if(y>y0)
    boxColor(win->display,el->x,el->y+y0,(el->x)+el->w-1,(el->y)+y-1,el->fgc);
  else
    boxColor(win->display,el->x,el->y+y,(el->x)+el->w-1,(el->y)+y0-1,el->fgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,el->x,el->y+y0,el->x+el->w,el->y+y0,el->agc);
}
void u_hscaler(ELEMENT *el,WINDOW *win, char *message, int len) {
  double v=myatof(message);
  int scalerbw=el->w/10;
  if(scalerbw<el->h)  scalerbw=el->h;
  if(scalerbw<8) scalerbw=8;
  boxColor(win->display,el->x,el->y,el->x+el->w,el->y+el->h,el->bgc);
  rectangleColor(win->display,el->x,el->y,el->x+el->w,el->y+el->h,el->agc);
  if(!isnan(v)) {
    double m=(v-el->min)/(el->max-el->min);
    if(m<0) m=0;
    if(m>1) m=1;
    int x=(int)(m*(double)(el->w-scalerbw)+0.5);
    boxColor(win->display,el->x+x,el->y,el->x+x+scalerbw,el->y+el->h,el->fgc);
    ELEMENT e;
    e.x=el->x+x;
    e.y=el->y;
    e.w=scalerbw;
    e.h=el->h;
    e.revert=0;
    d_frame(&e,win);
    e.revert=1;
    e.x=el->x+x+scalerbw/3+1;
    e.y=el->y+3;
    e.h=el->h-6;
    e.w=scalerbw/3;
    d_frame(&e,win);
  }
}

void u_vscaler(ELEMENT *el,WINDOW *win, char *message, int len) {
  double v=myatof(message);
  int scalerbh=el->h/10;
  if(scalerbh<el->w)  scalerbh=el->w;
  if(scalerbh<8) scalerbh=8;
  boxColor(win->display,el->x,el->y,el->x+el->w,el->y+el->h,el->bgc);
  rectangleColor(win->display,el->x,el->y,el->x+el->w,el->y+el->h,el->agc);
  if(!isnan(v)) {
    double m=(v-el->min)/(el->max-el->min);
    if(m<0) m=0;
    if(m>1) m=1;
    int y=(int)((1-m)*(double)(el->h-scalerbh)+0.5);
    boxColor(win->display,el->x,el->y+y,el->x+el->w,el->y+y+scalerbh,el->fgc);
    ELEMENT e;
    e.x=el->x;
    e.y=el->y+y;
    e.h=scalerbh;
    e.w=el->w;
    e.revert=0;
    d_frame(&e,win);
    e.revert=1;
    e.y=el->y+y+scalerbh/3+1;
    e.x=el->x+3;
    e.w=el->w-6;
    e.h=scalerbh/3;
    d_frame(&e,win);
  }
}

void u_plot(ELEMENT *el,WINDOW *win, char *message, int len) {
  boxColor(win->display,el->x,el->y,el->x+el->w,el->y+el->h,el->bgc);
  rectangleColor(win->display,el->x,el->y,el->x+el->w,el->y+el->h,el->agc);
  if(message && len>0) {
    char *p1=message;
    char *p2;
    int i=0;
    double v;
    int x,y=el->y+el->h,ox=0,oy=el->y+el->h;
//    int data[el->x2];
//    printf("Message: <%s>\n",message);
    while(*p1) {
      while(*p1 && isspace(*p1)) p1++;
      if(*p1==0) break;
      p2=p1;
      while(*p2 && !isspace(*p2)) p2++;
      if(*p2) *p2++=0;
      if(i>=el->y2+el->x2) break;
      x=i-el->y2;
      if(x>=0) { /* offset */
	x=(int)round((double)x/(el->amax-el->amin)*(double)el->w);
	if(x>=0 && x<=el->w) {
          v=myatof(p1);
	  y=el->y+el->h-v/(el->max-el->min)*(double)el->h;
	  if(y<el->y) y=el->y;
	  if(y>el->y+el->h) y=el->y+el->h;
	
	  if((el->revert&0xf)==1) {
            lineColor(win->display,el->x+x,el->y+el->h,el->x+x,y,el->fgc);
	  } else if((el->revert&0xf)==2) {
	    lineColor(win->display,el->x+ox,oy,el->x+ox,y,el->fgc);
	    lineColor(win->display,el->x+ox,y,el->x+x,y,el->fgc);
          } else if((el->revert&0xf)==3){
	    lineColor(win->display,el->x+ox,oy,el->x+x,y,el->fgc);
	  }
	  if((el->revert&0x70)==0x40) circleColor(win->display,el->x+x,y,2,el->fgc);
          else if((el->revert&0x70)==0x0) lineColor(win->display,el->x+x,y,el->x+x,y,el->fgc);
          if((el->revert&0x10)==0x10) lineColor(win->display,el->x+x-1,y,el->x+x+1,y,el->fgc);
          if((el->revert&0x20)==0x20) lineColor(win->display,el->x+x,y-1,el->x+x,y+1,el->fgc);
        }
      }
      ox=x;
      oy=y;
      i++;
      p1=p2;
    }
  }
}

/* This makes a nice looking vertical meter with automatic scala */

void u_tvmeter(ELEMENT *el,WINDOW *win, char *message, int len) {
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
    snprintf(buf,sizeof(buf),"%g",vv*scale);
    q=p=buf;
    while(*p) {
      if(isdigit(*p)) *q++=*p;
      p++;
    }
    *q=0;
    f=(buf[0])&1;
    if(f) lineColor(win->display,el->x,i,el->x+el->w/4,i,el->agc);
    else  lineColor(win->display,el->x,i,el->x+el->w/4,i,el->bgc);
    f=(buf[1])&1;
    if(f) lineColor(win->display,el->x+el->w/4,i,el->x+el->w/2,i,el->agc);
    else  lineColor(win->display,el->x+el->w/4,i,el->x+el->w/2,i,el->bgc);
  }
  
  rectangleColor(win->display,x,y,x+w/2,y+h,el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,x,y+y0,x+w/2,y+y0,el->fgc);
  if(!isnan(v)) {
    Sint16 vx[7]={-10,33,50,59, 50, 33,-10};
    Sint16 vy[7]={  0,20,35, 0,-35,-20,  0};
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
void u_thmeter(ELEMENT *el,WINDOW *win, char *message, int len) {
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
    snprintf(buf,sizeof(buf),"%g",vv*scale); 
    q=p=buf;
    while(*p) {
      if(isdigit(*p)) *q++=*p;
      p++;
    }
    *q=0;
    f=(buf[0])&1;
    if(f) lineColor(win->display,i,el->y,i,el->y+el->h/4,el->agc);
    else  lineColor(win->display,i,el->y,i,el->y+el->y/4,el->bgc);
    f=(buf[1])&1;
    if(f) lineColor(win->display,i,el->y+el->h/4,i,el->y+el->h/2,el->agc);
    else  lineColor(win->display,i,el->y+el->h/4,i,el->y+el->h/2,el->bgc);
  }
  rectangleColor(win->display,x,y,x+w,y+h/2,el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,x+x0,y,x+x0,y+h/2,el->fgc);
  if(!isnan(v)) {
    Sint16 vy[7]={-10,33,50,59, 50, 33,-10};
    Sint16 vx[7]={  0,20,35, 0,-35,-20,  0};
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


/*   The (round) Meter element.
 *  This element simulates an analog meter.
 *  TODO: auto scala, maybe different style options
 */

const Sint16 zeigerx[7]={0, 200,400,500, 400,200,  0};
const Sint16 zeigery[7]={25, 25,100,  0,-100,-25,-25};

void u_meter(ELEMENT *el,WINDOW *win, char *message, int len) {
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
  // ellipseColor(win->display,el->x+el->w/2,el->y+el->h/2,el->w/2,el->h/2,el->agc);
}





/* User Input (click) functions for all elements....*/

void element_publish(ELEMENT *el, double v,double old_v) {
  if(v>el->max) v=el->max;
  if(v<el->min) v=el->min;
  if(v!=old_v) {
    if(verbose>0) printf("New value is: %g\n",v);
    STRING format;
    format.pointer=el->format;
    format.len=strlen(format.pointer);
    STRING a=do_using(v,format);
    mqtt_publish(el->topic,a,el->revert,1);
    free(a.pointer);
  }
}

/* If the click was not consumed before the panel (also) gets it */

int c_panel(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==2) {
    message_dialog(PACKAGE_NAME " Info",
                   PACKAGE_NAME "\n===============\n\n Version " VERSION "\n\n" 
		   VERSION_DATE "\n\n (c) by Markus Hoffmann et. al.\n",1);
    return(1);
  } else if(b==3) {
    char newdash[256];
    char buf[256];

    newdash[0]=0;
    /* open a file selector to select a new dash to display.*/
    int rc=fileselect_dialog(newdash,dashboarddir,"*.dash");
    if(rc && newdash[0]) {
      if(exist(newdash))  snprintf(buf,sizeof(buf),MQTT_HYPERDASH_EXECUTABLE_NAME " %s &",newdash);
      else snprintf(buf,sizeof(buf),MQTT_HYPERDASH_EXECUTABLE_NAME " %s/%s.dash &",dashboarddir,newdash);
      if(verbose>0) printf("Dash start: <%s>\n",newdash);
      if(verbose>0) printf("call: <%s>\n",buf);
      if(system(buf)==-1) printf(MQTT_HYPERDASH_EXECUTABLE_NAME " ERROR: system\n");  
    }
    return(1);
  }
  return(0);
}







/* get last known value, add increment, format it and then publish it. */
int c_tticker(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    char *def=subscriptions[el->subscription].last_value.pointer;
    double v=myatof(def);
    double old_v=v;
//    printf("ticker <%s>\n",el->topic);
//    printf("subscription <%s>\n",subscriptions[el->subscription].topic);
//    printf("minmax %g/%g\n",el->min,el->max);
//    printf("increment: %g\n",el->increment);
//    printf("last value is: <%s> : %g\n",def,v);
    v+=el->increment;
    element_publish(el,v,old_v);  /* Final publish */
   // return(1); Do not consume it. There may be other action.... 
  }
  return(0);
}


int c_hscaler(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  char *def=subscriptions[el->subscription].last_value.pointer;
  double v=myatof(def);
  double old_v=v;
  double old2_v;
  int d=0;
  double diff=0;
  int scalerbw=el->w/10;
  if(scalerbw<el->h)  scalerbw=el->h;
  if(scalerbw<8) scalerbw=8;
  double m=(v-el->min)/(el->max-el->min);
  if(m<0) m=0;
  if(m>1) m=1;
  int x0=(int)(m*(double)(el->w-scalerbw)+0.5);
  if(b==4) {           /* Scrollwheel up --> Ticker up */
    v+=el->increment;
  } else if(b==5) {    /* Scrollwheel down --> Ticker down */
    v-=el->increment;
  } else if(x>el->x+x0 && x<el->x+x0+scalerbw) {
    ELEMENT lose=*el;
    lose.revert=0;   /* Make QoS = 0 for intermediate pushes */
    SDL_Event event;
    while(SDL_WaitEvent(&event)!=0) { 
      if(event.type!=SDL_MOUSEBUTTONUP) { 
        // printf(" at %d,%d : %d %d.\n",event.button.x,event.button.y,event.button.button,event.button.state);
        d=event.button.x-x; /* rel. movement */
	diff=(double)d/(double)(el->w-scalerbw)*(el->max-el->min);
	/* Now shall we allow only ticker resolution ?*/
	diff=round(diff/el->increment)*el->increment;
	old2_v=v;
	v=old_v+diff;
        if(v>el->max) v=el->max;
        if(v<el->min) v=el->min;
	element_publish(&lose,v,old2_v);
      } else {
        if(verbose>0) printf("Button was released at %d,%d.\n",event.button.x,event.button.y);
	d=event.button.x-x; /* rel. movement */
	diff=(double)d/(double)(el->w-scalerbw)*(el->max-el->min);
	diff=round(diff/el->increment)*el->increment;
	v=old_v+diff;
        break;
      }
    }
  } else if(b==1) {    /* Click outside knob --> input */
    return(c_tinnumber(el,win,x,y,b));
  }
  element_publish(el,v,old_v);  /* Final publish */
  return(0);
}
int c_vscaler(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  char *def=subscriptions[el->subscription].last_value.pointer;
  double v=myatof(def);
  double old_v=v;
  double old2_v;
  int d=0;
  double diff=0;
  int scalerbh=el->h/10;
  if(scalerbh<el->w)  scalerbh=el->w;
  if(scalerbh<8) scalerbh=8;
  double m=(v-el->min)/(el->max-el->min);
  if(m<0) m=0;
  if(m>1) m=1;
  int y0=(int)((1-m)*(double)(el->h-scalerbh)+0.5);
    
  if(b==4) {    /* Scrollwheel up --> Ticker up */
    v+=el->increment;
  } else if(b==5) {    /* Scrollwheel down --> Ticker down */
    v-=el->increment;
  } else if(y>el->y+y0 && y<el->y+y0+scalerbh) { 
    ELEMENT lose=*el;
    lose.revert=0;   /* Make QoS = 0 for intermediate pushes */
    SDL_Event event;
    while(SDL_WaitEvent(&event)!=0) { 
      if(event.type!=SDL_MOUSEBUTTONUP) { 
        // printf(" at %d,%d : %d %d.\n",event.button.x,event.button.y,event.button.button,event.button.state);
        d=-(event.button.y-y); /* rel. movement */
	diff=(double)d/(double)(el->h-scalerbh)*(el->max-el->min);
	/* Now shall we allow only ticker resolution ?*/
	diff=round(diff/el->increment)*el->increment;
	old2_v=v;
	v=old_v+diff;
        if(v>el->max) v=el->max;
        if(v<el->min) v=el->min;
	element_publish(&lose,v,old2_v);
      } else {
        if(verbose>0) printf("Button was released at %d,%d.\n",event.button.x,event.button.y);
	d=-(event.button.y-y); /* rel. movement */
	diff=(double)d/(double)(el->h-scalerbh)*(el->max-el->min);
	diff=round(diff/el->increment)*el->increment;
	v=old_v+diff;
        break;
      }
    }
  } else if(b==1) {    /* Click outside knob --> input */
    return(c_tinnumber(el,win,x,y,b));
  }
  element_publish(el,v,old_v);  /* Final publish */
  return(0);
}
int c_frame(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    el->revert=1;
    d_frame(el,win);
    SDL_Flip(win->display);
    /* Wait for mouse releasse*/
    waitmouse(win);
    el->revert=0;
    d_frame(el,win);
    SDL_Flip(win->display);
   // return(1); Do not consume it. There my be other action.... 
  }
  return(0);
}

extern char call_options[];


int c_subdash(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  char filename[256];
  char buf[256];
  if(b==1) {
    snprintf(filename,sizeof(filename),"%s.dash",el->text);
    snprintf(buf,sizeof(buf),MQTT_HYPERDASH_EXECUTABLE_NAME "%s %s &",call_options,filename);
    if(verbose>=0) printf("Dash start: <%s>\n",filename);
    if(verbose>=0) printf("call: <%s>\n",buf);
    if(system(buf)==-1) printf("Error: system\n");  
    return(1);
  }
  return(0);
}
int c_shellcmd(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    if(verbose>=0) printf("Shell cmd : <%s>\n",el->text);
    if(system(el->text)==-1) printf("Error: system\n");
    return(1);
  }
  return(0);
}
int c_tinarea(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    if(verbose>=0) printf("topic value cmd : <%s>\n",el->text);
    STRING a;
    a.pointer=el->text;
    a.len=strlen(a.pointer);
    mqtt_publish(el->topic,a,el->revert,1);
   // return(1); Do not consume it. There my be other action.... 
  }
  return(0);
}
int c_tinnumber(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    if(verbose>0) printf("input number for topic <%s> with format <%s>\n",el->topic,el->format);
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
int c_tinstring(ELEMENT *el,WINDOW *win,int x, int y, int b) {
  if(b==1) {
    if(verbose>0) printf("input string for topic <%s>\n",el->topic);
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
