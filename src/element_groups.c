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
 {0,"Activity Indicator",groups_activity}, 
 {0,"Status Indicator",groups_statusbitmap},
 {0,"ON/OFF Button Group",groups_onoff},
 {0,"Potentiometer with tickers",groups_potentiometer},

};
const int anzgroups=sizeof(groups)/sizeof(GROUPDEF);
 
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
