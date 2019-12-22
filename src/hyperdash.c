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
#include "graphics.h"
#include "file.h"
#include "hyperdash.h"
#include "util.h"
#include "mqtt.h"

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
void i_pbox(ELEMENT *el,char *pars) {
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->linewidth=atoi(key_value(pars,"LINEWIDTH","1"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_string(ELEMENT *el,char *pars) {
  char buf[32];
  el->text=strdup(key_value(pars,"TEXT","TEXT"));
  sprintf(buf,"%d",(int)strlen(el->text)*20+5);
  el->w=atoi(key_value(pars,"W",buf));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","SMALL"));
  el->bgc=(long)myatof(key_value(pars,"BGC","$00000000"));
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_bitmap(ELEMENT *el,char *pars) {
  el->filename=strdup(key_value(pars,"BITMAP","bitmap.bmp"));
  el->w=atoi(key_value(pars,"W","32"));
  el->h=atoi(key_value(pars,"H","32"));
  /* FGC */
  el->fgc=(long)myatof(key_value(pars,"FGC","$00ff0000"));
}
void i_icon(ELEMENT *el,char *pars) {
  el->filename=strdup(key_value(pars,"ICON","icon.png"));
  el->w=atoi(key_value(pars,"W","32"));
  el->h=atoi(key_value(pars,"H","32"));
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


void d_panel(ELEMENT *el,WINDOW *win) {
  set_font(el->font,win);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
}
void d_line(ELEMENT *el,WINDOW *win) {
  lineColor(win->display,el->x,el->y,el->x2,el->y2,el->fgc);
}
void d_box(ELEMENT *el,WINDOW *win) {
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->w),el->fgc);
}
void d_frame(ELEMENT *el,WINDOW *win) {
  unsigned long ac,bc;
  if(el->revert==1) {
    ac=0x4f4f4fff;
    bc=0xafafafff;
  } else {
    bc=0x4f4f4fff;
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
void d_pbox(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->w),el->fgc);
}
void d_string(ELEMENT *el,WINDOW *win) {
  set_font(el->font,win);
  stringColor(win->display,el->x,el->y,el->text,el->fgc);
}

void d_tstring(ELEMENT *el,WINDOW *win) {
  set_font(el->font,win);
  stringColor(win->display,el->x,el->y,el->topic,el->fgc);
  mqtt_subscribe(el->topic,0);
}
void u_tstring(ELEMENT *el,WINDOW *win,char *message) {
  set_font(el->font,win);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);
  stringColor(win->display,el->x,el->y,message,el->fgc);
}

void d_tnumber(ELEMENT *el,WINDOW *win) {
  set_font(el->font,win);
  stringColor(win->display,el->x,el->y,el->format,el->fgc);
  mqtt_subscribe(el->topic,0);
}
void u_tnumber(ELEMENT *el,WINDOW *win, char *message) {
  double v;
  STRING a,format;
  format.pointer=el->format;
  format.len=strlen(format.pointer);
  v=atof(message);
  a=do_using(v,format);
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->h)-1,el->bgc);

  set_font(el->font,win);  
  stringColor(win->display,el->x,el->y,a.pointer,el->fgc);
  free(a.pointer);
}

void c_shellcmd(ELEMENT *el,WINDOW *win,int x, int y) {
  printf("Shell cmd : <%s>\n",el->text);
  
  
  if(system(el->text)==-1) printf("Error: system\n");  
}




const ELDEF eltyps[]= {
 {EL_IGNORE,"#",NULL,NULL,NULL},
 {EL_BROKER,"BROKER",i_broker,NULL,NULL},
 {EL_PANEL|EL_VISIBLE, "PANEL",i_panel,d_panel,NULL},
 {EL_VISIBLE,"LINE",i_line,d_line,NULL},
 {EL_VISIBLE,"BOX",i_box,d_box,NULL},
 {EL_VISIBLE,"PBOX",i_pbox,d_pbox,NULL},
 {EL_VISIBLE,"FRAME",i_frame,d_frame,NULL},
 {EL_VISIBLE,"BITMAP",i_bitmap,NULL,NULL},
 {EL_VISIBLE,"ICON",i_icon,NULL,NULL},
 {EL_VISIBLE,"TEXT",i_string,d_string,NULL},
 {EL_VISIBLE|EL_DYNAMIC,"TOPICSTRING",i_tstring,d_tstring,u_tstring},
 {EL_VISIBLE|EL_DYNAMIC,"TOPICNUMBER",i_tnumber,d_tnumber,u_tnumber},
 {EL_INPUT,"SHELLCMD",i_shellcmd,NULL,NULL,c_shellcmd},
 {EL_IGNORE,"#",NULL,NULL,NULL},
};
const int anzeltyp=sizeof(eltyps)/sizeof(ELDEF);


void click_element(ELEMENT *el, WINDOW *win, int x, int y) {
  int j=(el->type&0xff);
  printf("click element.\n");
  if(eltyps[j].click) (eltyps[j].click)(el,win,x,y);


}


void update_element(ELEMENT *el, WINDOW *win, STRING message) {
  int j=(el->type&0xff);
  printf("update element. <%s>\n",message.pointer);
  if(eltyps[j].update) (eltyps[j].update)(el,win,message.pointer);


}
void draw_element(ELEMENT *el, WINDOW *win) {
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
  /*
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
  printf("update_dash: <%s>...\n",topic);
  for(i=0;i<dash->anzelement;i++) {
    if((dash->tree[i].type&EL_DYNAMIC)==EL_DYNAMIC) {
      if(!strcmp(topic,dash->tree[i].topic)) update_element(&(dash->tree[i]),win,message);
    } 
  }
  SDL_Flip(win->display);
}



int handle_event(WINDOW *w,SDL_Event *event) {
  switch (event->type) {
    /* Das Redraw-Event */
/* wahrscheinlich muessen wir gar nix tun...*/
  case SDL_QUIT:
    printf("OOps, window close request. What should I do?\n");
    return(-1);
    break;
  case SDL_ACTIVEEVENT: 
    if ( event->active.state & SDL_APPACTIVE ) {
        if ( event->active.gain ) {
            printf("App activated\n");
        } else {
            printf("App iconified\n");
        }
    }
  #ifdef SDL_WINDOWEVENT
  case SDL_WINDOWEVENT:
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN:
      printf("Window %d shown", event->window.windowID);break;
    case SDL_WINDOWEVENT_HIDDEN:
      printf("Window %d hidden", event->window.windowID);break;
    case SDL_WINDOWEVENT_EXPOSED:
      printf("Window %d exposed", event->window.windowID);break;
    case SDL_WINDOWEVENT_MOVED:
      printf("Window %d moved to %d,%d",event->window.windowID, event->window.data1,event->window.data2);break;
    case SDL_WINDOWEVENT_RESIZED:
      printf("Window %d resized to %dx%d",event->window.windowID, event->window.data1,event->window.data2);break;
    case SDL_WINDOWEVENT_MINIMIZED:
      printf("Window %d minimized", event->window.windowID);break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      printf("Window %d maximized", event->window.windowID);break;
    case SDL_WINDOWEVENT_RESTORED:
      printf("Window %d restored", event->window.windowID);break;
    case SDL_WINDOWEVENT_ENTER:
      printf("Mouse entered window %d",event->window.windowID);break;
    case SDL_WINDOWEVENT_LEAVE:
      printf("Mouse left window %d", event->window.windowID);break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      printf("Window %d gained keyboard focus",event->window.windowID);break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      printf("Window %d lost keyboard focus",event->window.windowID);break;
    case SDL_WINDOWEVENT_CLOSE:
      printf("Window %d closed", event->window.windowID);break;
    default:
      printf("Window %d got unknown event %d",event->window.windowID, event->window.event);break;
    }
  #endif
  }
  return(0);
}


int mouseevent(WINDOW *window, int *x, int *y, int *b, int *s) {
  SDL_Event event;
  if(SDL_WaitEvent(&event)==0) return(0);
  while(event.type!=SDL_MOUSEBUTTONDOWN && event.type!=SDL_MOUSEBUTTONUP) { 
     if(handle_event(window,&event)==-1) return(-1);
     if(SDL_WaitEvent(&event)==0) return(0);
  }
  *x=event.button.x;
  *y=event.button.y;
  *b=event.button.button;
  *s=event.button.state;
  return(1);
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
