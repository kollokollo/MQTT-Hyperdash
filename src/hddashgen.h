/* HDDASHGEN.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


/*
   Define the geometric layout of all elements 
 */
 
#define HDDASHGEN_VERSION "1.02"
#define DEFAULT_BROKER "tcp://localhost:1883"
#define MAX_ANZ_TOPICS 4096

#define ADVANCE_Y      30 
#define ADVANCE_X     260
#define MAX_Y         600 
#define FIELD_START_Y  70

/* Shortcut definitions */

#define XY  "X=%d Y=%d "
#define XYWH  "X=%d Y=%d W=%d H=%d "
#define XYWHT "X=%d Y=%d W=%d H=%d TOPIC=\"%s\" "
#define XYWHR "X=%d Y=%d W=%d H=%d REVERT=1 "
#define XYHT  "X=%d Y=%d H=%d TEXT=\"%s\" "

#define DIGIFORMAT "FORMAT=\"######\" "
#define DIGIMM "MIN=0 MAX=1000 "

#define ANAFORMAT "FORMAT=\"######.###\" "
#define ANAMM "MIN=0 MAX=4 "

#define FONT_SMALL   "FONT=\"Courier_New_Bold\" FONTSIZE=10 "
#define FONT_TINY    "FONT=\"Arial\" FONTSIZE=10 "
#define FONT_BUTTON  "FONT=\"Arial\" FONTSIZE=16 "
#define FONT_PAR     "FONT=\"Courier_New_Bold\" FONTSIZE=16 "
#define FONT_PARNAME "FONT=\"Courier_New\" FONTSIZE=16 "

/* Color definitions */

#define WHITE   "FGC=$FFFFFFFF "
#define BGBLUE  "BGC=$40FF "
#define BGBLACK "BGC=$FF "
#define GRAY    "FGC=$808080FF "
#define BGGRAY  "BGC=$808080FF "
#define BGGRAY2 "BGC=$8080C0FF "
#define BGDARKGRAY  "BGC=$404040FF "

#define YELLOW  "FGC=$FFFF00FF "
#define ORANGE  "FGC=$FF4500FF "
#define MAGENTA "FGC=$FF00FFFF "
#define CYAN    "FGC=$00FFFFFF "
#define RED     "FGC=$FF0000FF "
#define GREEN   "FGC=$00FF00FF "
