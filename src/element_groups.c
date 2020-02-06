/* ELEMENT_GROUPS.C (c) Markus Hoffmann  */

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
#include "element_groups.h"

/* The groups functions emmit a newly allocated charakter string with 
   the element definitions. Multiple elements are separated by \n
   */



const GROUPDEF groups[]= {
 {0,"Dashboard Hyperlink Button",groups_dashbutton}, 
 {0,"Activity Indicator",groups_activity}, 
 {0,"Status Indicator",groups_statusbitmap},
 {0,"Gauge",groups_gauge},
 {0,"ON/OFF Button Group",groups_onoff},
 {0,"Topic I/O Field",groups_topicio},
 {0,"Potentiometer with tickers",groups_potentiometer},

};
const int anzgroups=sizeof(groups)/sizeof(GROUPDEF);

char *groups_gauge(int x, int y, int w, int h) {
  char buffer[256*2];
  if(h<50) h=50;
  if(w<50) w=50;
  *buffer=0;
  sprintf(buffer+strlen(buffer),
    "METER:    " XYWHT BGBLUE RED " AGC=$A0A0A0FF MIN=0 MAX=100 AMIN=225 AMAX=-45 TYPE=3\n",
    x,y,w,h,DEFAULT_TOPIC);
  sprintf(buffer+strlen(buffer),
    "TOPICNUMBER: " XYWHT YELLOW BGBLUE " FORMAT=\"###.#\" FONT=\"comicbd\" FONTSIZE=11\n",
    x+w/2-18,y+h/2-5,40,11,DEFAULT_TOPIC);
  sprintf(buffer+strlen(buffer),
    "TEXT:      " XYHT WHITE FONT_TINY "\n",x+w/5,y+4*h/5,13,"0");
  sprintf(buffer+strlen(buffer),
    "TEXT:      " XYHT WHITE FONT_TINY "\n",x+3*w/5,y+4*h/5,13,"100%");
  sprintf(buffer+strlen(buffer),
    "TEXT:      " XYHT WHITE FONT_TINY "\n",x+w/2-15,y+7*h/10,13,"Unit");
#if 0
METER: X=10 Y=100 W=100 H=100 TOPIC="LOCALHOST/SYSMEASURE/SYSMEMUSAGE_AM" MIN=0 MAX=100 AMIN=225 AMAX=-45 BGC=$40FF FGC=$FF0000FF AGC=$A0A0A0FF TYPE=3
TOPICNUMBER: X=43 Y=145 TOPIC="LOCALHOST/SYSMEASURE/SYSMEMUSAGE_AM" FORMAT="###.#" W=40 H=11 FONT="comicbd" FONTSIZE=11 BGC=$40FF FGC=$FFFF00FF
TEXT: X=30 Y=180 TEXT="0" W=13 H=13 FONT="Arial" FONTSIZE=10 FGC=$FFFFFFFF
TEXT: X=45 Y=170 TEXT="Mem" W=39 H=13 FONT="Arial" FONTSIZE=10 FGC=$FFFFFFFF
TEXT: X=70 Y=180 TEXT="100%" W=52 H=13 FONT="Arial" FONTSIZE=10 FGC=$FFFFFFFF
#endif
  return(strdup(buffer));
}


char *groups_topicio(int x, int y, int w, int h) {
  char buffer[256];
  if(h<20) h=20;
  if(w<50) w=50;
  *buffer=0;
  sprintf(buffer+strlen(buffer),
    "FRAME:    " XYWHR "\n",x-2,y-2,w+4,h+4);
  sprintf(buffer+strlen(buffer),
    "TOPICINSTRING: " XYWHT "\n",x-2,y-2,w+4,h+4,DEFAULT_TOPIC);
  sprintf(buffer+strlen(buffer),
    "TEXTAREA:      " XYWHT BGBLACK WHITE FONT_SMALL "\n",x,y,w,h,DEFAULT_TOPIC);
  return(strdup(buffer));
}

 
char *groups_dashbutton(int x, int y, int w, int h) {
  char buffer[256];
  if(h<20) h=20;
  if(w<50) w=50;
  *buffer=0;
  sprintf(buffer+strlen(buffer),
    "PBOX:        " XYWH BGDARKGRAY GRAY "\n",     x,  y,w,h);
  sprintf(buffer+strlen(buffer),
    "DASH:        " XYWH "DASH=\"%s\"\n",       x,  y,w,h,"main");
  sprintf(buffer+strlen(buffer),
    "FRAMETOGGLE: " XYWH "\n",                  x,  y,w,h);
  sprintf(buffer+strlen(buffer),
    "TEXT:        " XYHT WHITE FONT_BUTTON "\n",x+5,y+5, h-10,"Dashboard");
  return(strdup(buffer));
}
 
 
char *groups_activity(int x, int y, int w, int h) {
  char buffer[256];
  sprintf(buffer,"BITMAPLABEL: " XY BGBLUE "TOPIC=\"%s\" "
                 "BITMAP[0]=\"0|Disc1|$FFFFFFFF\" " 
                 "BITMAP[1]=\"1|Disc2|$FFFFFFFF\" "
                 "BITMAP[2]=\"2|Disc3|$FFFFFFFF\" "
                 "BITMAP[3]=\"3|Disc4|$FFFFFFFF\"",x,y,"ACTIVITY_DM");

  return(strdup(buffer));
}
char *groups_statusbitmap(int x, int y, int w, int h) {
  char buffer[256];
  sprintf(buffer,
     "BITMAPLABEL: " XY BGBLUE "TOPIC=\"%s\" "
     "BITMAP[0]=\"0|SmallCircle|$00FF00FF\" " 
     "BITMAP[1]=\"1|SmallCircle|$FF0000FF\" "
     "BITMAP[2]=\"2|SmallCircle|$FFFF00FF\" "
     "BITMAP[3]=\"3|SmallCircle|$FF00FFFF\"",x,y,"STATUS_DM");
  return(strdup(buffer));
}
char *groups_onoff(int x, int y, int w, int h) {
  char buffer[256*3];
  *buffer=0;
  sprintf(buffer+strlen(buffer),
  "TEXT:	" XYHT WHITE FONT_PARNAME "\n",x,y,20,"ONOFF_DC");
  sprintf(buffer+strlen(buffer),
  "PBOX:	" XYWH GRAY BGDARKGRAY "\n",x+100,y,45,20);
  sprintf(buffer+strlen(buffer),
  "PBOX:	" XYWH GRAY BGDARKGRAY "\n",x+155,y,45,20);
  sprintf(buffer+strlen(buffer),
  "TEXT:	" XYHT GREEN FONT_BUTTON "\n",x+110,y+1,20,"ON");
  sprintf(buffer+strlen(buffer),
  "TEXT:	" XYHT RED   FONT_BUTTON "\n",x+162,y+1,20,"OFF");
  sprintf(buffer+strlen(buffer),
  "FRAMELABEL:  " XYWHT "MATCH=\"1\"\n",x+100,y,45,20,"ONOFF_DC");
  sprintf(buffer+strlen(buffer),
  "FRAMELABEL:  " XYWHT "MATCH=\"0\"\n",x+155,y,45,20,"ONOFF_DC");
  sprintf(buffer+strlen(buffer),
  "TOPICINAREA: " XYWHT "VALUE=\"1\"\n",x+100,y,45,20,"ONOFF_DC");
  sprintf(buffer+strlen(buffer),
  "TOPICINAREA: " XYWHT "VALUE=\"0\"\n",x+155,y,45,20,"ONOFF_DC");
//  printf("strlen=%ld\n",strlen(buffer));
  return(strdup(buffer));
}
char *groups_potentiometer(int x, int y, int w, int h) {
  char buffer[256*4];
  *buffer=0;
  /* TODO:
    If W>h do horizontal
    if H>w do vertical
  */
  
  
  sprintf(buffer+strlen(buffer),
    "TEXT:    " XYHT WHITE FONT_PARNAME "\n",x+10,y,20,"SETPOINT_AC");
  sprintf(buffer+strlen(buffer),
    "TOPICNUMBER: " XYWHT BGBLUE CYAN FONT_PAR ANAFORMAT "\n",x+150,y,100,20,"SETPOINT_AC");
  y+=30;
  sprintf(buffer+strlen(buffer),
    "PBOX:    " XYWH BGGRAY GRAY "\n",x+7,y,240,20);
  sprintf(buffer+strlen(buffer),
    "FRAME:   " XYWH "\n",x+7-2,y-2,240+4,20+4);
  sprintf(buffer+strlen(buffer),
    "FRAME:   " XYWHR "\n",x+7-2+20,y+2,240+4-40,20-4-1);
  sprintf(buffer+strlen(buffer),
    "BITMAP:  " XY WHITE "BITMAP=\"TickLeft\"\n",x+7,y+2);
  sprintf(buffer+strlen(buffer),
    "BITMAP:  " XY WHITE "BITMAP=\"TickRight\"\n",x+7+220+2,y+2);
  sprintf(buffer+strlen(buffer),
    "HSCALER: " XYWHT ANAMM BGBLACK GRAY "TIC=0.05 AGC=$0\n",x+7+20,y+2,240-40,20-4-2,"SETPOINT_AC");
  sprintf(buffer+strlen(buffer),
    "TICKER:  " XYWHT ANAMM "TIC=-0.05\n",x+7,    y+2,16,16,"SETPOINT_AC");
  sprintf(buffer+strlen(buffer),
    "TICKER:  " XYWHT ANAMM "TIC=0.05\n", x+7+220+2,y+2,16,16,"SETPOINT_AC");


  printf("strlen=%ld\n",strlen(buffer));
  return(strdup(buffer));
}
