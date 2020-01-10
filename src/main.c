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
#ifndef WINDOWS
#include <gtk/gtk.h>
#endif
//#ifdef WINDOWS
//  #include <windows.h>
//#endif
#include "config.h"
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"

#if defined WINDOWS || defined ANDROID
  #define EX_OK 0
#else
  #include <sysexits.h>
#endif


char ifilename[100]="main.dash";
int verbose=0;

int dofullscreen=0;

static void intro() {
  printf("**********************************************************\n"
         "*    %10s                    V." VERSION "            *\n"
         "*                       by Markus Hoffmann 2019-2020 (c) *\n"
         "* version date:           %30s *\n"
         "**********************************************************\n\n",
	     PACKAGE_NAME,VERSION_DATE);
}

static void usage() {
  printf(
    "Usage: %s [-h] [<filename>] --- display dashboard [%s]\n\n"
    " -h --help\t\t--- usage\n"
    " --help <topic>\t\t--- print help on topic\n"
    " --iconpath <path>\t--- set path for icon files [%s]\n"
    " --bitmappath <path>\t--- set path for bitmap files [%s]\n"
    " --fontpath <path>\t--- set path for bitmap files [%s]\n"
    " --fullscreen \t\t--- enable fullscreen graphics mode \n"
    ,MQTT_HYPERDASH_EXECUTABLE_NAME,ifilename,icondir,bitmapdir,fontdir);
}
static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      intro();
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"-v"))	     verbose++;
    else if(!strcmp(argumente[count],"-q"))	     verbose--;
    else if(!strcmp(argumente[count],"--fullscreen")) dofullscreen++;
    else if(!strcmp(argumente[count],"--iconpath"))  strncpy(icondir,      argumente[++count],256);
    else if(!strcmp(argumente[count],"--bitmappath"))strncpy(bitmapdir,   argumente[++count],256);
    else if(!strcmp(argumente[count],"--fontpath"))strncpy(fontdir,   argumente[++count],256);
    else if(!strcmp(argumente[count],"--dashboardpath"))strncpy(dashboarddir,   argumente[++count],256);
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the BASIC program*/
    else {
      strncpy(ifilename,argumente[count],sizeof(ifilename));
    }
  }
  if(quitflag) exit(EX_OK);
}

extern WINDOW *global_window;  /* TODO */

#ifdef WINDOWS
#include <windows.h>
int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow) {
int anzahl=0;
char **argumente= (char **)CommandLineToArgvW(GetCommandLineW(), &anzahl);
 if( NULL == argumente )
   {
      wprintf(L"CommandLineToArgvW failed\n");
      return 0;
   }
#else
int main(int anzahl, char *argumente[]) {
#endif
  DASH *maindash;
  WINDOW *mainwindow;
#ifndef WINDOWS
  gtk_init (&anzahl, &argumente);
#endif
    kommandozeile(anzahl, argumente);    /* process command line */
    if(!exist(ifilename)) {
      char buf[strlen(ifilename)+1];
      strcpy(buf,ifilename);
      snprintf(ifilename,sizeof(ifilename),"%s/%s",dashboarddir,buf);
    }
    if(exist(ifilename)) {
      int rc=0;
      maindash=load_dash(ifilename);
      init_dash(maindash);
      mainwindow=create_window(maindash->tree[maindash->panelelement].text,PACKAGE_NAME,0,0,maindash->tree[maindash->panelelement].w,maindash->tree[maindash->panelelement].h,dofullscreen);
      global_window=mainwindow; /* TODO */
      draw_dash(maindash,mainwindow);
again:
      rc=handle_dash(maindash,mainwindow);
      if(rc==0) {
        close_window(mainwindow);
        close_dash(maindash);
      } else if(rc==-1) {
        close_dash(maindash);
	printf(PACKAGE_NAME ": Reconnect in 5 Sec.\n");
        sleep(5);
	printf(PACKAGE_NAME ": Reconnect: \n");
	init_dash(maindash);
	draw_dash(maindash,mainwindow);
	goto again;
      }
      free_dash(maindash);
    } else printf(PACKAGE_NAME " ERROR: %s not found !\n",ifilename);
  return(EX_OK);
}
