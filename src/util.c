/* UTIL.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 


/* Diese Funktion gibt zurueck
   0 Wenn der String leer ist
     w1$=w2$=""
   1 wenn der String t$ nur aus einem Wort
     besteht. w2$="" w1$=t$
   2 wenn der String t$ aus mehreren Worten
     besteht. w1$=erstes Wort
     w2$= Rest von t$
  ##########################################
  der String in t wird nicht veraendert,
  w1 und w2 sind Stringbuffer, die mindestens so gross wie t sein muessen
  
   Befindet sich ein Teil des Strings in 
   AnfÅhrungszeichen ("), so wird dieser Teil
   nicht berÅcksichtigt.
   wird klam! mit True angegeben, wird
   auch der Inhalt von Klammern () nicht berÅcksichtigt

   Die Wîrter werden als solche erkannt, wenn
   sie durch das Zeichen c getrennt sind.

   wort_sep2 hat einen string als separator
   wort_sepr und wort_sepr2 suchen rueckwaerts

   (c) Markus Hoffmann 1997

   2012: memories now may overlap and t==w2 or t==w1 is allowed.


  */


int wort_sep(const char *t,char c,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0;

  /* klamb=0 : keine Klammern werden beruecksichtigt
     klamb=1 : normale Klammern werden beruecksichtigt
     klamb=2 : eckige Klammern werden beruecksichtigt
     klamb=4 : geschweifte Klammern werden beruecksichtigt
  */

  if(!(*t)) return(*w1=*w2=0);

  while(*t && (*t!=c || f || klam>0)) {
    if(*t=='"') f=!f;
    else if(!f && (((klamb&1) && *t=='(') || ((klamb&2) && *t=='[') || ((klamb&4) && *t=='{'))) klam++;
    else if(!f && (((klamb&1) && *t==')') || ((klamb&2) && *t==']') || ((klamb&4) && *t=='}'))) klam--;
    *w1++=*t++;
  }
  if(!(*t)) {
    *w2=*w1=0;
    return(1);
  } else {
    *w1=0;
    ++t;
    while(*t) *w2++=*t++;
    *w2=0; 
    return(2);
  }
}






/*  (c) Markus Hoffmann
  ' Teile in AnfÅhrungszeichen werden nicht verÑndert
  ' Ersetzt Tabulator durch Leerzeichen
  ' mehrfache Leerzeichen zwischen Wîrtern werden entfernt
  ' wenn fl&=1 ,  gib Groûbuchstaben zurÅck
 */
 
void xtrim(const char *t,int f, char *w ) {
  register int a=0,u=0,b=0;

  while(*t) {
    while(*t && (!isspace(*t) || a)) {
      if(*t=='"') a=!a;
      u=1; if(b==1) {*w++=' '; b=0;}
      if(f && !a) *w++=toupper(*t++); else *w++=*t++;
    }
    if(u && !b) b=1;
    if(*t) t++;
  } *w=0;
}
