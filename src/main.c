/* MAIN.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hyperdash.h"
#if defined WINDOWS || defined ANDROID
  #define EX_OK 0
#else
  #include <sysexits.h>
#endif


char ifilename[100]="in.dash";
int verbose=0;



int main(int anzahl, char *argumente[]) {
  if(anzahl<2) {
    intro();
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(exist(ifilename)) {
      int dasherrors=loaddash(ifilename);
      if(dasherrors) {
        printf("Errors detected in %s. Can not run.\n",ifilename);
      }
    } else printf("ERROR: %s not found !\n",ifilename);
  }
  return(EX_OK);
}
