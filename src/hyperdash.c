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
#include "util.h"
#include "graphics.h"
#include "file.h"
#include "hyperdash.h"



char *key_value(const char *a, const char *b, const char *def) {
  static char value[256]; 
  char par[256];
  char kv[256];
  char key[256];
  char val[256];
  strcpy(par,a);
  int e=wort_sep(par,' ',1,kv,par);
  int e2;
  while(e>0) {
    e2=wort_sep(kv,'=',1,key,val);
    if(e2==2) {
       if(!strcmp(key,b)) {
         if(val[0]=='\"') {
	   if(strlen(val)>0) val[strlen(val)-1]=0;
	   strcpy(value,val+1);
	 } else strcpy(value,val);
	 return(value);
       }
    }  
    e=wort_sep(par,' ',1,kv,par);
  }
  strcpy(value,def);
  return(value);
}




void i_broker(ELEMENT *el,char *pars) {
  el->filename=strdup(key_value(pars,"URL","tcp://localhost:1883"));
  el->x=0;
  el->y=0;
  el->w=0;
  el->h=0;
  /* connect to mqtt broker */
}
void i_panel(ELEMENT *el,char *pars) {
  el->text=strdup(key_value(pars,"TITLE","Dashboard"));
  el->font=strdup(key_value(pars,"FONT","*-HELVETICA-BOLD-R-*-20-*-*-*-*-*-*-*"));
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->w=atoi(key_value(pars,"W","256"));
  el->h=atoi(key_value(pars,"H","256"));
  /* FGC BGC */
}
void i_line(ELEMENT *el,char *pars) {
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->x2=atoi(key_value(pars,"X2","10"));
  el->y2=atoi(key_value(pars,"Y2","10"));
  el->w=el->x2-el->x;
  el->h=el->y2-el->y;
  el->linewidth=atoi(key_value(pars,"LINEWIDTH","1"));
  /* FGC */
}
void i_box(ELEMENT *el,char *pars) {
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->linewidth=atoi(key_value(pars,"LINEWIDTH","1"));
  /* FGC */
}
void i_frame(ELEMENT *el,char *pars) {
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->revert=atoi(key_value(pars,"REVERT","0"));
}
void i_pbox(ELEMENT *el,char *pars) {
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->w=atoi(key_value(pars,"W","10"));
  el->h=atoi(key_value(pars,"H","10"));
  el->linewidth=atoi(key_value(pars,"LINEWIDTH","1"));
  /* FGC BGC */
}
void i_string(ELEMENT *el,char *pars) {
  char buf[32];
  el->text=strdup(key_value(pars,"TEXT","TEXT"));
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  sprintf(buf,"%d",(int)strlen(el->text)*20+5);
  el->w=atoi(key_value(pars,"W",buf));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","*-HELVETICA-BOLD-R-*-20-*-*-*-*-*-*-*"));
  /* FGC BGC  */
}
void i_bitmap(ELEMENT *el,char *pars) {
  el->filename=strdup(key_value(pars,"BITMAP","bitmap.bmp"));
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->w=atoi(key_value(pars,"W","32"));
  el->h=atoi(key_value(pars,"H","32"));
  /* FGC */
}
void i_icon(ELEMENT *el,char *pars) {
  el->filename=strdup(key_value(pars,"ICON","icon.png"));
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  el->w=atoi(key_value(pars,"W","32"));
  el->h=atoi(key_value(pars,"H","32"));
}
void i_tstring(ELEMENT *el,char *pars) {
  char buf[32];
  el->topic=strdup(key_value(pars,"TOPIC","TOPIC"));
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  sprintf(buf,"%d",(int)strlen(el->topic)*20+5);
  el->w=atoi(key_value(pars,"W",buf));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","*-HELVETICA-BOLD-R-*-20-*-*-*-*-*-*-*"));
  /* FGC BGC  */
}
void i_tnumber(ELEMENT *el,char *pars) {
  char buf[32];
  el->topic=strdup(key_value(pars,"TOPIC","TOPIC"));
  el->format=strdup(key_value(pars,"USING","###.###"));
  el->x=atoi(key_value(pars,"X","0"));
  el->y=atoi(key_value(pars,"Y","0"));
  sprintf(buf,"%d",(int)strlen(el->topic)*20+5);
  el->w=atoi(key_value(pars,"W",buf));
  el->h=atoi(key_value(pars,"H","20"));
  el->font=strdup(key_value(pars,"FONT","*-HELVETICA-BOLD-R-*-20-*-*-*-*-*-*-*"));
  /* FGC BGC  */
}


void d_line(ELEMENT *el,WINDOW *win) {
  lineColor(win->display,el->x,el->y,el->x2,el->y2,win->fcolor);
  
}
void d_box(ELEMENT *el,WINDOW *win) {
  rectangleColor(win->display,el->x,el->y,(el->x)+(el->w),(el->y)+(el->w),win->fcolor);
}
void d_pbox(ELEMENT *el,WINDOW *win) {
  boxColor(win->display,el->x,el->y,(el->x)+(el->w)-1,(el->y)+(el->w)-1,win->fcolor);
}
void d_string(ELEMENT *el,WINDOW *win) {
  stringColor(win->display,el->x,el->y,el->text,win->fcolor);
}

void d_tstring(ELEMENT *el,WINDOW *win) {
  stringColor(win->display,el->x,el->y,el->topic,win->fcolor);
}
void u_tstring(ELEMENT *el,WINDOW *win,char *message) {
  stringColor(win->display,el->x,el->y,message,win->fcolor);
}

void d_tnumber(ELEMENT *el,WINDOW *win) {
  stringColor(win->display,el->x,el->y,el->format,win->fcolor);
}
void u_tnumber(ELEMENT *el,WINDOW *win, char *message) {}


const ELDEF eltyps[]= {
 {EL_IGNORE,"#",NULL,NULL,NULL},
 {EL_BROKER,"BROKER",i_broker,NULL,NULL},
 {EL_PANEL, "PANEL",i_panel,NULL,NULL},
 {EL_VISIBLE,"LINE",i_line,d_line,NULL},
 {EL_VISIBLE,"BOX",i_box,d_box,NULL},
 {EL_VISIBLE,"PBOX",i_pbox,d_pbox,NULL},
 {EL_VISIBLE,"FRAME",i_frame,NULL,NULL},
 {EL_VISIBLE,"BITMAP",i_bitmap,NULL,NULL},
 {EL_VISIBLE,"ICON",i_icon,NULL,NULL},
 {EL_VISIBLE,"TEXT",i_string,d_string,NULL},
 {EL_VISIBLE|EL_DYNAMIC,"TOPICSTRING",i_tstring,d_tstring,u_tstring},
 {EL_VISIBLE|EL_DYNAMIC,"TOPICNUMBER",i_tnumber,d_tnumber,u_tnumber},
 {EL_IGNORE,"#",NULL,NULL,NULL},
 {EL_IGNORE,"#",NULL,NULL,NULL},
};
const int anzeltyp=sizeof(eltyps)/sizeof(ELDEF);




void update_element(ELEMENT *el, WINDOW *win, STRING message) {


}
void draw_element(ELEMENT *el, WINDOW *win) {
  int j=(el->type&0xff);
  if(eltyps[j].draw) (eltyps[j].draw)(el,win);
}

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
	    if(eltyps[j].init) (eltyps[j].init)(&(dash->tree[i]),b);
	    break;
	  }
	}
        if(j==anzeltyp) printf("Unknown element #%d <%s>\n",i,dash->tree[i].line);
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
void handle_dash(DASH *dash, WINDOW *win) {

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
