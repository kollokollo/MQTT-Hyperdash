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
#include <gtk/gtk.h>
#include "config.h"
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"

#if defined WINDOWS 
  #define EX_OK 0
  #define mkdir(a,b) mkdir(a)
#else
  #include <sysexits.h>
#endif


char ifilename[SIZEOF_IFILENAME]="main.dash";
int verbose=0;
int do_connection=1;      /* make real connections to broker */
int do_show_invisible=0;  /* Also draw invisible elements. */

int dofullscreen=0;
char *broker_override=NULL;
char *broker_user=NULL;
char *broker_passwd=NULL;
char *topic_prefix=NULL;

char call_options[SIZEOF_CALL_OPTIONS]="";

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
    " --iconpath <path>\t--- set path for icon files [%s]\n"
    " --bitmappath <path>\t--- set path for bitmap files [%s]\n"
    " --dashpath <path>\t--- set path for dash files [%s]\n"
    " --fontpath <path>\t--- set path for bitmap files [%s]\n"
    " --broker <url>\t\t--- use this broker, ignore broker in dash file.\n"
    " --user <user>\t\t--- use this username for broker.\n"
    " --passwd <passwd>\t\t--- use this password for broker.\n"
    " --prefix <prefix>\t--- set a prefix for all topics.\n"
    " --fullscreen \t\t--- enable fullscreen graphics mode.\n"
    " --noopts \t\t--- do not pass commandline options to sub-dashboards.\n"
    ,MQTT_HYPERDASH_EXECUTABLE_NAME,ifilename,icondir,bitmapdir,dashboarddir,fontdir);
}

#define COLLECT() snprintf(call_options+strlen(call_options),sizeof(call_options)-strlen(call_options)," %s",argumente[count])
#define COLLECT2() snprintf(call_options+strlen(call_options),sizeof(call_options)-strlen(call_options)," %s %s",argumente[count-1],argumente[count])

static void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters and */
  /* Collect call options used when hyperdash calls itself for a subDash: */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-h") || !strcmp(argumente[count],"--help")) {
      intro();
      usage();
      quitflag=1;
    } 
    else if(!strcmp(argumente[count],"-v"))	      {verbose++; COLLECT();}
    else if(!strcmp(argumente[count],"-q"))	      {verbose--; COLLECT();}
    else if(!strcmp(argumente[count],"--fullscreen")) {dofullscreen++; COLLECT();}
    else if(!strcmp(argumente[count],"--iconpath"))   {strncpy(icondir,     argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--bitmappath")) {strncpy(bitmapdir,   argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--fontpath"))   {strncpy(fontdir,     argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--dashpath"))   {strncpy(dashboarddir,argumente[++count],256);COLLECT2();}
    else if(!strcmp(argumente[count],"--broker"))     {broker_override=argumente[++count];COLLECT2();}
    else if(!strcmp(argumente[count],"--user"))       {broker_user=    argumente[++count];COLLECT2();}
    else if(!strcmp(argumente[count],"--passwd"))     {broker_passwd=  argumente[++count];COLLECT2();}
    else if(!strcmp(argumente[count],"--prefix"))     {topic_prefix=   argumente[++count];COLLECT2();}
    else if(!strcmp(argumente[count],"--noopts"))     {*call_options=0;}
    else if(*(argumente[count])=='-')                 {COLLECT();}
    else {
      strncpy(ifilename,argumente[count],sizeof(ifilename));
    }
  }
  if(quitflag) exit(EX_OK);
  if(verbose>0) printf("Call-Options: <%s>\n",call_options);
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
  gdk_threads_init();
  gtk_init (&anzahl, &argumente);
  hyperdash_set_defaults();
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
	if(verbose>=0) printf(PACKAGE_NAME ": Reconnect in 5 sec.\n");
        sleep(5);
	if(verbose>=0) printf(PACKAGE_NAME ": Reconnect.\n");
	init_dash(maindash);
	draw_dash(maindash,mainwindow);
	goto again;
      }
      free_dash(maindash);
    } else printf(PACKAGE_NAME " ERROR: %s not found !\n",ifilename);
  return(EX_OK);
}
