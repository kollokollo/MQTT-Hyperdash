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
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "util.h"
#include "mqtt.h"

/*TODO: FrameLabel*/


char icondir[256]="/usr/share/hyperdash/icons";
char bitmapdir[256]="/usr/share/hyperdash/bitmaps";
char fontdir[256]="/usr/share/hyperdash/fonts";

char dashborddir[256]="/usr/share/hyperdash/dashbords";

void i_broker(ELEMENT *el,char *pars) {
  el->filename=strdup(key_value(pars,"URL","tcp://localhost:1883"));
  el->x=0;
  el->y=0;
  el->w=0;
  el->h=0;
  /* connect to mqtt broker */
  mqtt_broker(el->filename,NULL,NULL);
}
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
void i_frame(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->revert=atoi(key_value(pars,"REVERT","0"));
}
void i_framelabel(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->label[0].pointer=strdup(key_value(pars,"MATCH","0"));
  el->label[0].len=strlen(el->label[0].pointer);
}

void i_pbox(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->linewidth=atoi(key_value(pars,"LINEWIDTH","1"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$000000ff"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff00ff"));
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


void i_string(ELEMENT *el,char *pars) {
  char buf[32];
  el->text=strdup(key_value(pars,"TEXT","TEXT"));
  sprintf(buf,"%d",(int)strlen(el->text)*20+5);
  el->w=atoi(key_value(pars,"W",buf));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->fontsize=atoi(key_value(pars,"FONTSIZE","16"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
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

  el->w=atoi(key_value(pars,"W","32"));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
}





STRING get_icon(const char *name, int *w, int *h) {
  STRING ret;
  STRING a;
  a=get_file(name);
  ret=pngtobmp((unsigned char *)a.pointer,(size_t)a.len);
  free(a.pointer);
  return(ret);
}
#define SmallDisc_width 11
#define SmallDisc_height 11

static char SmallDisc_bits[] = {
   0xfc, 0x01, 0x02, 0x02, 0x07, 0x07, 0x8f, 0x07, 0xdf, 0x07, 0xff, 0x07,
   0xdf, 0x07, 0x8f, 0x07, 0x07, 0x07, 0x02, 0x02, 0xfc, 0x01};

STRING get_bitmap(const char *name, int *w, int *h) {
  int i=0;
  int e;
  STRING ret;
  char *odummy;
  char dummy[256];
  STRING a=get_file(name);
  if(a.pointer && a.len) {
    ret.pointer=malloc(a.len);
    ret.len=0;
    wort_sep(a.pointer,'_',0,dummy,a.pointer);
    wort_sep(a.pointer,' ',0,dummy,a.pointer);
    wort_sep(a.pointer,'\n',0,dummy,a.pointer);
    *w=(atoi(dummy)+7)&(~7);
    printf("w=%d\n",*w);
    wort_sep(a.pointer,'_',0,dummy,a.pointer);
    wort_sep(a.pointer,' ',0,dummy,a.pointer);
    wort_sep(a.pointer,'\n',0,dummy,a.pointer);
    *h=atoi(dummy);
    printf("h=%d\n",*h);
    e=wort_sep(a.pointer,'{',0,dummy,a.pointer);
    e=wort_sep(a.pointer,',',0,dummy,a.pointer);
    while(e>0) {
    odummy=dummy;
      while(odummy[0]<=' ') odummy++;
     // printf("%d:<%s> <%s>: %x\n",i,dummy,odummy,(int)myatof(odummy));
      ret.pointer[i++]=(int)myatof(odummy);
      e=wort_sep(a.pointer,',',0,dummy,a.pointer);
    }
    ret.len=i;
//    exit(0);
    
    free(a.pointer);
  } else {
    ret.pointer=SmallDisc_bits;
    ret.len=sizeof(SmallDisc_bits);
    *w=(SmallDisc_width+7)&0xfffffc;
    *h=SmallDisc_height;
  }
  return(ret);
}
void i_bitmap(ELEMENT *el,char *pars) {
  char f[256];
  int w=32,h=32;
  el->filename=strdup(key_value(pars,"BITMAP","bitmap.bmp"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
  sprintf(f,"%s/%s",bitmapdir,el->filename);
  if(exist(f)) {
    el->data[0]=get_bitmap(f,&w,&h);
    printf("Bitmap: %dx%d\n",w,h);
  } else {
    printf("Error: Bitmap %s not found!\n",f);
  }
  sprintf(f,"%d",w);
  el->w=atoi(key_value(pars,"W",f));
  sprintf(f,"%d",h);
  el->h=atoi(key_value(pars,"H",f));
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
      printf("Bitmap: <%s> %dx%d\n",f,w,h);
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
void i_tstring(ELEMENT *el,char *pars) {
  char buf[32];
  sprintf(buf,"%d",(int)strlen(el->topic)*20+5);
  el->w=atoi(key_value(pars,"W",buf));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
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

void i_tnumber(ELEMENT *el,char *pars) {
  char buf[32];
  el->format=strdup(key_value(pars,"FORMAT","###.###"));
  sprintf(buf,"%d",(int)strlen(el->format)*20+5);
  el->w=atoi(key_value(pars,"W",buf));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  /* FGC BGC  */
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
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


#define ELEMENT_SUBSCRIBE() mqtt_subscribe(el->topic,0)


void d_panel(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
}
void d_line(ELEMENT *el,WINDOW *win) {
  lineColor(win->display,el->x,el->y,el->x2,el->y2,el->fgc);
}
void d_box(ELEMENT *el,WINDOW *win) {
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->fgc);
}


void d_bitmap(ELEMENT *el,WINDOW *win) {
  put_bitmap(win,el->data[0].pointer,el->x,el->y,el->w,el->h,el->fgc);
}
void d_bitmaplabel(ELEMENT *el,WINDOW *win) {
  if(el->data[0].pointer) {
    put_bitmap(win,el->data[0].pointer,el->x,el->y,el->w,el->h,el->labelcolor[0]);
  }
  ELEMENT_SUBSCRIBE();
}
void d_icon(ELEMENT *el,WINDOW *win) {
  put_graphics(win,el->data[0],el->x,el->y,el->w,el->h,el->agc);
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
void d_framelabel(ELEMENT *el,WINDOW *win) {
  d_frame(el,win);
  ELEMENT_SUBSCRIBE();
}


void d_pbox(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->h),el->fgc);
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


/* Update Functions */


void u_hbar(ELEMENT *el,WINDOW *win, char *message) {
  double v=atof(message);
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
    stringColor(win->display,el->x,el->y,el->data[found].pointer,el->labelcolor[found]);
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


void u_vbar(ELEMENT *el,WINDOW *win, char *message) {
  double v=atof(message);
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
void d_string(ELEMENT *el,WINDOW *win) {
  put_font_text(win,el->font,el->fontsize,el->text,el->x,el->y,el->fgc,el->h);
}

void d_tstring(ELEMENT *el,WINDOW *win) {
 // put_font_text(win,el->font,el->fontsize,el->topic,el->x,el->y,el->fgc);
  ELEMENT_SUBSCRIBE();
}
void d_textlabel(ELEMENT *el,WINDOW *win) {
  if(el->data[0].pointer) {
    put_font_text(win,el->font,el->fontsize,el->data[0].pointer,el->x,el->y,el->labelcolor[0],el->h);
  }
  ELEMENT_SUBSCRIBE();
}
void u_tstring(ELEMENT *el,WINDOW *win,char *message) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  stringColor(win->display,el->x,el->y,message,el->fgc);
}

void d_tnumber(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  stringColor(win->display,el->x,el->y,el->format,el->fgc);
  ELEMENT_SUBSCRIBE();
}
void u_tnumber(ELEMENT *el,WINDOW *win, char *message) {
  double v;
  STRING a,format;
  format.pointer=el->format;
  format.len=strlen(format.pointer);
  v=atof(message);
  a=do_using(v,format);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);

  stringColor(win->display,el->x,el->y,a.pointer,el->fgc);
  free(a.pointer);
}



/* Click Functions */

void c_shellcmd(ELEMENT *el,WINDOW *win,int x, int y) {
  printf("Shell cmd : <%s>\n",el->text);
  if(system(el->text)==-1) printf("Error: system\n");  
}
void c_tinarea(ELEMENT *el,WINDOW *win,int x, int y) {
  printf("topic value cmd : <%s>\n",el->text);
  STRING a;
  a.pointer=el->text;
  a.len=strlen(a.pointer);
  mqtt_publish(el->topic,a,el->revert,1);
}

void c_subdash(ELEMENT *el,WINDOW *win,int x, int y) {
  char buf[256];
  if(exist(el->text))  sprintf(buf,"hyperdash %s.dash &",el->text);
  else sprintf(buf,"hyperdash %s/%s.dash &",dashborddir,el->text);
  printf("Dash start: <%s>\n",el->text);
  if(system(buf)==-1) printf("Error: system\n");  
}
void c_frame(ELEMENT *el,WINDOW *win,int x, int y) {
  el->revert=1;
  d_frame(el,win);
  SDL_Flip(win->display);
  /* Wait for mouse releasse*/
  waitmouse(win);
  el->revert=0;
  d_frame(el,win);
  SDL_Flip(win->display);
}




const ELDEF eltyps[]= {
// {EL_IGNORE,"#",NULL,NULL,NULL},
 {EL_BROKER,"BROKER",i_broker,NULL,NULL},
 {EL_VISIBLE,"BOX",i_box,d_box,NULL},
 {EL_VISIBLE,"LINE",i_line,d_line,NULL},
 {EL_PANEL|EL_VISIBLE, "PANEL",i_panel,d_panel,NULL},
 {EL_VISIBLE,"PBOX",i_pbox,d_pbox,NULL},
 {EL_VISIBLE,"FRAME",i_frame,d_frame,NULL},
 {EL_VISIBLE|EL_DYNAMIC|EL_INPUT,"FRAMETOGGLE",i_frame,d_frame,NULL,c_frame},
 {EL_VISIBLE,"BITMAP",i_bitmap,d_bitmap,NULL},
 {EL_VISIBLE,"ICON",i_icon,d_icon,NULL},
 {EL_VISIBLE,"TEXT",i_string,d_string,NULL},
 {EL_VISIBLE|EL_DYNAMIC,"TOPICSTRING",i_tstring,d_tstring,u_tstring},
 {EL_VISIBLE|EL_DYNAMIC,"TOPICNUMBER",i_tnumber,d_tnumber,u_tnumber},
 {EL_VISIBLE|EL_DYNAMIC,"HBAR",i_bar,d_hbar,u_hbar,NULL},
 {EL_VISIBLE|EL_DYNAMIC,"VBAR",i_bar,d_vbar,u_vbar,NULL},
 {EL_VISIBLE|EL_DYNAMIC,"TEXTLABEL",i_textlabel,d_textlabel,u_textlabel,NULL},
 {EL_VISIBLE|EL_DYNAMIC,"BITMAPLABEL",i_bitmaplabel,d_bitmaplabel,u_bitmaplabel,NULL},
 {EL_VISIBLE|EL_DYNAMIC,"FRAMELABEL",i_framelabel,d_framelabel,u_framelabel,NULL},
 {EL_INPUT,"SHELLCMD",i_shellcmd,NULL,NULL,c_shellcmd},
 {EL_INPUT,"DASH"    ,i_subdash ,NULL,NULL,c_subdash},
 {EL_INPUT|EL_DYNAMIC,"TOPICINAREA"    ,i_tinarea ,NULL,NULL,c_tinarea},
};
const int anzeltyp=sizeof(eltyps)/sizeof(ELDEF);


static void click_element(ELEMENT *el, WINDOW *win, int x, int y) {
  int j=(el->type&0xff);
  printf("click element.\n");
  if(eltyps[j].click) (eltyps[j].click)(el,win,x,y);
}


static void update_element(ELEMENT *el, WINDOW *win, STRING message) {
  int j=(el->type&0xff);
//  printf("update element. <%s>\n",message.pointer);
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
	    if((dash->tree[i].type&EL_DYNAMIC)==EL_DYNAMIC) dash->tree[i].topic=strdup(key_value(b,"TOPIC","TOPIC"));
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
}

void close_dash(DASH *dash) {
  /*  TODO:
    unsubscribe alle parameter.
    Verbindung zum Broker trennen. 
  
  */
}


void draw_dash(DASH *dash, WINDOW *win) {
  int i;
  for(i=0;i<dash->anzelement;i++) {
    if((dash->tree[i].type&EL_VISIBLE)==EL_VISIBLE) draw_element(&(dash->tree[i]),win); 
  }
  SDL_Flip(win->display); 
}
void update_dash(char *topic, STRING message) {
  DASH *dash=global_dash;
  WINDOW *win=global_window;
  int i;
 // printf("update_dash: <%s>...\n",topic);
  for(i=0;i<dash->anzelement;i++) {
    if((dash->tree[i].type&EL_DYNAMIC)==EL_DYNAMIC) {
      if(!strcmp(topic,dash->tree[i].topic)) update_element(&(dash->tree[i]),win,message);
    } 
  }
  SDL_Flip(win->display);
}




void handle_dash(DASH *dash, WINDOW *win) {
  int x,y,b,s;
  int a;
  while(1) {
    a=mouseevent(win,&x,&y,&b,&s);
    if(a==-1) return;
    else if(a==1) {
      if(s==1 && b==1) {
        int i;
        for(i=0;i<dash->anzelement;i++) {
          if((dash->tree[i].type&EL_INPUT)==EL_INPUT) {
	    if(dash->tree[i].x<=x && dash->tree[i].y<=y &&
	       dash->tree[i].x+dash->tree[i].w>x &&
	       dash->tree[i].y+dash->tree[i].h>y)
               click_element(&(dash->tree[i]),win,x,y);
          } 
        }
      }
    }
  }
}

void free_element(ELEMENT *el) {
  free(el->topic);
  free(el->font);
  free(el->text);
  free(el->filename);
  free(el->format);
}


/* remove dash from memory and clear dashbuffer */
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
