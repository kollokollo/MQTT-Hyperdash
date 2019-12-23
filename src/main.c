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
#include <unistd.h>

#include "config.h"
#include "file.h"
#include "graphics.h"

#include "hyperdash.h"
#if defined WINDOWS || defined ANDROID
  #define EX_OK 0
#else
  #include <sysexits.h>
#endif


char ifilename[100]="in.dash";
char icondir[256]=".";
char bitmapdir[256]=".";
int verbose=0;

int dofullscreen=0;

static void intro() {
  printf("**********************************************************\n"
         "*    %10s                     V." VERSION "               *\n"
         "*                       by Markus Hoffmann 2019-2020 (c) *\n"
         "* version date:           %30s *\n"
         "**********************************************************\n\n",
	     "MQTT-Hyperdash",VERSION_DATE);
}

static void usage() {
  printf(
    "Usage: %s [-h] [<filename>] --- display dashboard [%s]\n\n"
    " -h --help\t--- Usage\n"
    " --help <topic>\t--- Print help on topic\n"
    " --iconpath <path>\t--- set path for icon files [%s]\n"
    " --bitmappath <path>\t--- set path for bitmap files [%s]\n"
    " --fullscreen \t--- enable fullscreen graphics mode \n"
    ,"hyperdash",ifilename,icondir,bitmapdir);
}
static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h")) {
      intro();
      usage();
      quitflag=1;
    } else if(!strcmp(argumente[count],"--help")) {
      intro();
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"-v"))	     verbose++;
    else if(!strcmp(argumente[count],"-q"))	     verbose--;
    else if(!strcmp(argumente[count],"--fullscreen")) dofullscreen++;
    else if(!strcmp(argumente[count],"--iconpath"))  strncpy(icondir,      argumente[++count],256);
    else if(!strcmp(argumente[count],"--bitmappath"))strncpy(bitmapdir,   argumente[++count],256);
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the BASIC program*/
    else {
      strcpy(ifilename,argumente[count]);
    }
  }
  if(quitflag) exit(EX_OK);
}

extern WINDOW *global_window;  /* TODO */
int main(int anzahl, char *argumente[]) {
  DASH *maindash;
  WINDOW *mainwindow;
  if(anzahl<2) {
    intro();
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(exist(ifilename)) {
      maindash=load_dash(ifilename);
      init_dash(maindash);
      mainwindow=create_window(maindash->tree[maindash->panelelement].text,"MQTT-Hyperdash",0,0,maindash->tree[maindash->panelelement].w,maindash->tree[maindash->panelelement].h,dofullscreen);
      global_window=mainwindow; /* TODO */
      draw_dash(maindash,mainwindow);
      handle_dash(maindash,mainwindow);
      close_dash(maindash);
      close_window(mainwindow);
      free_dash(maindash);
    } else printf("ERROR: %s not found !\n",ifilename);
  }
  return(EX_OK);
}