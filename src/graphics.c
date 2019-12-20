/* GRAPHICS.C (c) Markus Hoffmann  */

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

int init_sdl() {
  static int done=0;
  if(done) return(0);
  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) return -1;
  atexit(SDL_Quit);
  /* Enable Unicode translation */
  SDL_EnableUNICODE( 1 );

  done=1;
  return(0);
}

void close_window(WINDOW *win) {
  free(win);
}

#define WINDOW_DEFAULT_W 512
#define WINDOW_DEFAULT_H 256
WINDOW *create_window(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h) {
  WINDOW *nw;
  nw=calloc(sizeof(WINDOW),1);
  init_sdl();
  
  if(!(nw->display=SDL_SetVideoMode(WINDOW_DEFAULT_W, 
       WINDOW_DEFAULT_H, 32,
    // SDL_FULLSCREEN |
       SDL_HWSURFACE|SDL_SRCALPHA))) {
      printf("cannot open SDL surface \n");
      SDL_Quit();
      return(NULL);
  }
  nw->x=nw->y=0;
  nw->w=nw->display->w;
  nw->h=nw->display->h;
  nw->fcolor=0xffffffff;
  nw->bcolor=0;
  SDL_WM_SetCaption(title,info);
  if(nw->title) free(nw->title);
  nw->title=strdup(title);
  if(nw->info) free(nw->info);
  nw->info=strdup(info); 
  return(nw);
}
