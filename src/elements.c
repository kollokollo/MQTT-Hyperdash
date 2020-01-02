/* ELEMENTS.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
 
 /*TODO: 
   1. Ticker elements (frametoggle) HTIC,VTIC: MIN,MAX,TIC=
   right-click opens a box where one can choose 10x or 1x tics.
   2. Scaler elements HSCALER, VSCALER, MIN, MAX, RANGE, TIC
   right-click opens a box where one can choose different ranges
   (full, medium (30%), fine (10%)) and where one can set the value 
   explicitly. Also the arrow keys should be active for tics.


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

/* Initialize the broker and try to connect... */

void i_broker(ELEMENT *el,char *pars) {
  el->filename=strdup(key_value(pars,"URL","tcp://localhost:1883"));
  el->x=0;
  el->y=0;
  el->w=0;
  el->h=0;
  /* connect to mqtt broker */
  int rc=mqtt_broker(el->filename,NULL,NULL);
  if(rc==-1) message_dialog("MQTT Hyperdash Error","ERROR: Could not connect to the MQTT broker.", 1);
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

/* Initialize Static drawing elements */
#define ELEMENT_FONT() el->fontnr=add_font(el->font,el->fontsize)
#define ELEMENT_SUBSCRIBE() el->subscription=add_subscription(el->topic)


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
  char buf[32];
  el->text=strdup(key_value(pars,"TEXT","TEXT"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  sprintf(buf,"%d",(int)strlen(el->text)*fonts[el->fontnr].height+5);
  el->w=atoi(key_value(pars,"W",buf));
  sprintf(buf,"%d",fonts[el->fontnr].height);
  el->h=atoi(key_value(pars,"H",buf));
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_bitmap(ELEMENT *el,char *pars) {
  char f[256];
  int w=32,h=32;
  el->filename=strdup(key_value(pars,"BITMAP","bitmap.bmp"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
  sprintf(f,"%s/%s",bitmapdir,el->filename);
  if(exist(f)) {
    el->data[0]=get_bitmap(f,&w,&h);
    if(verbose>0) printf("Bitmap: %dx%d\n",w,h);
  } else {
    printf("Error: Bitmap %s not found!\n",f);
  }
  sprintf(f,"%d",w);
  el->w=atoi(key_value(pars,"W",f));
  sprintf(f,"%d",h);
  el->h=atoi(key_value(pars,"H",f));
}
void i_icon(ELEMENT *el,char *pars) {
  char f[256];
  int w=32,h=32;
  el->filename=strdup(key_value(pars,"ICON","icon.png"));
  el->agc=(long)myatof(key_value(pars,"TGC","$00"));  /* Transparent color */
  sprintf(f,"%s/%s",icondir,el->filename);
  if(exist(f)) {
    el->data[0]=get_icon(f,&w,&h);
  } else {
    printf("Error: Icon %s not found!\n",f);
  }
  sprintf(f,"%d",w);
  el->w=atoi(key_value(pars,"W",f));
  sprintf(f,"%d",h);
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
void i_textlabel(ELEMENT *el,char *pars) {
  int i;
  char p[256];
  char *a;
  char w1[256],w2[256],w3[256];

  for(i=0;i<10;i++) {
    sprintf(p,"TEXT[%d]",i);
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
  sprintf(p,"%d",fonts[el->fontnr].height);
  el->h=atoi(key_value(pars,"H",p));
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
    sprintf(p,"BITMAP[%d]",i);
    a=key_value(pars,p,p);
    wort_sep(a,'|',0,w1,w2);
    wort_sep(w2,'|',0,w2,w3);
    el->label[i].pointer=strdup(w1);
    el->label[i].len=strlen(el->label[i].pointer);
    if(*w2) {
    sprintf(f,"%s/%s",bitmapdir,w2);
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
  sprintf(f,"%d",w);
  el->w=atoi(key_value(pars,"W",f));
  sprintf(f,"%d",h);
  el->h=atoi(key_value(pars,"H",f));
}

void i_tstring(ELEMENT *el,char *pars) {
  char buf[32];
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  sprintf(buf,"%d",(int)strlen(el->topic)*fonts[el->fontnr].height+5);
  el->w=atoi(key_value(pars,"W",buf));
  sprintf(buf,"%d",fonts[el->fontnr].height);
  el->h=atoi(key_value(pars,"H",buf));
  /* FGC BGC  */  
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_tnumber(ELEMENT *el,char *pars) {
  char buf[32];
  el->format=strdup(key_value(pars,"FORMAT","###.###"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  ELEMENT_FONT();
  sprintf(buf,"%d",(int)strlen(el->format)*fonts[el->fontnr].height+5);
  el->w=atoi(key_value(pars,"W",buf));
  sprintf(buf,"%d",fonts[el->fontnr].height);
  el->h=atoi(key_value(pars,"H",buf));
  /* FGC BGC  */
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
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
void i_tinnumber(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","20"));
  el->h=atoi(key_value(pars,"H","20"));
  el->revert=atoi(key_value(pars,"QOS","0"));
  el->format=strdup(key_value(pars,"FORMAT","###.###"));
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
void d_vbar(ELEMENT *el,WINDOW *win) {
  int y=el->h-1-(int)(0-el->min)*el->h/(el->max-el->min);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->agc);
  if(el->min<0 && el->max>0) lineColor(win->display,el->x,el->y+y,el->x+el->w,el->y+y,el->agc);
  ELEMENT_SUBSCRIBE();
}
void d_tvmeter(ELEMENT *el,WINDOW *win) {
  u_tvmeter(el,win,"NaN");
  ELEMENT_SUBSCRIBE();
}
void d_thmeter(ELEMENT *el,WINDOW *win) {
  u_thmeter(el,win,"NaN");
  ELEMENT_SUBSCRIBE();
}
void d_meter(ELEMENT *el,WINDOW *win) {
  u_meter(el,win,"NaN");
  ELEMENT_SUBSCRIBE();
}
void d_tstring(ELEMENT *el,WINDOW *win) {
 // put_font_text(win,el->font,el->fontsize,el->topic,el->x,el->y,el->fgc);
  ELEMENT_SUBSCRIBE();
}
void d_textlabel(ELEMENT *el,WINDOW *win) {
  if(el->data[0].pointer) {
    put_font_text(win->display,el->fontnr,el->data[0].pointer,el->x,el->y,el->labelcolor[0],el->h);
  }
  ELEMENT_SUBSCRIBE();
}
void d_tnumber(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  put_font_text(win->display,el->fontnr,el->format,el->x,el->y,el->fgc,el->h);
  ELEMENT_SUBSCRIBE();
}


/* Update drawing functions for all dynamic elements....*/


void u_tstring(ELEMENT *el,WINDOW *win,char *message) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  put_font_text(win->display,el->fontnr,message,el->x,el->y,el->fgc,el->h);
}
void u_tnumber(ELEMENT *el,WINDOW *win, char *message) {
  STRING format;
  format.pointer=el->format;
  format.len=strlen(format.pointer);
  double v=myatof(message);
  STRING a=do_using(v,format);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  put_font_text(win->display,el->fontnr,a.pointer,el->x,el->y,el->fgc,el->h);
  free(a.pointer);
}
void u_textlabel(ELEMENT *el,WINDOW *win, char *message) {
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
void u_framelabel(ELEMENT *el,WINDOW *win, char *message) {
  el->revert=0;
  if(el->label[0].pointer) {
    if(!strcmp(el->label[0].pointer,message)) el->revert=1;   
  }
  d_frame(el,win);
}
void u_bitmaplabel(ELEMENT *el,WINDOW *win, char *message) {
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



void u_hbar(ELEMENT *el,WINDOW *win, char *message) {
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
void u_vbar(ELEMENT *el,WINDOW *win, char *message) {
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



/* User Input (click) functions for all elements....*/



