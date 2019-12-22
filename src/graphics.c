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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
int init_sdl() {
  static int done=0;
  if(done) return(0);
  XInitThreads();
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
  
  if(!(nw->display=SDL_SetVideoMode(w,h, 32,
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



static void *memrevcpy(char *dest, const char *src, size_t n) {
  unsigned char a,b;
  int i;
  while(n--) {
    a=src[n];
    b=0;
    for(i=0;i<8;i++) {
      b<<=1;
      b|=(a&1);
      a>>=1;
    }   
    dest[n]=b;
  }
  return(dest);
}


/*Eine einfache Bitmap auf den Screen bringen, hierbei gibt es 
  fordergrundfarbe, hintergrundfarbe und Graphmodi zu berücksichtigen.
  */

void put_bitmap(const char *adr,int x,int y,unsigned int w, unsigned int h, WINDOW *window) {
  SDL_Surface *data;
  SDL_Surface *image;
  int bpl=(w+1)>>3;
  data=SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,1, 0,0,0,0);
  memrevcpy(data->pixels,adr,bpl*h);
  data->pitch=bpl;
//  if(global_graphmode==GRAPHMD_TRANS) {
    SDL_SetColorKey(data,SDL_SRCCOLORKEY,0);
//  }
//  memdump((char *)(data->format),sizeof(SDL_PixelFormat));
//  memdump((char *)(data->format->palette),sizeof(SDL_Palette));
//  memdump((char *)(data->format->palette->colors),2*sizeof(SDL_Color));
  unsigned char *pal=(unsigned char *)(data->format->palette->colors);
  uint32_t col=window->bcolor;
  col>>=8;
  pal[2]=(col&0xff); col>>=8;
  pal[1]=(col&0xff); col>>=8;
  pal[0]=(col&0xff);
  col=window->fcolor; col>>=8;
  pal[6]=(col&0xff); col>>=8;
  pal[5]=(col&0xff); col>>=8;
  pal[4]=(col&0xff);
   
  image=SDL_DisplayFormat(data);
  SDL_FreeSurface(data);
  SDL_Rect a={0,0,image->w,image->h};
  SDL_Rect b={x,y,image->w,image->h};
//  printf("putbitmap: %dx%d %d %d\n",image->w,image->h,bpl,image->pitch);
  SDL_BlitSurface(image, &a,window->display, &b);
  SDL_FreeSurface(image);
}
/* NAME="BIG"
        "SMALL"
        "LARGE"
	"8x16"
	"8x8"
	"5x7"

*/

extern const unsigned char *spat_a816;
extern const unsigned char *asciiTable;

void set_font(char *name, WINDOW *window) {
  if(strcmp(name,"BIG")==0 || strcmp(name,"8x16")==0) {
    window->chw=8;
    window->chh=16;
    window->baseline=window->chh-2; 
   // gfxPrimitivesSetFont(spat_a816,8,16); 	
  } else if(strcmp(name,"MEDIUM")==0 || strcmp(name,"8x8")==0) {
    window->chw=8;
    window->chh=8;
    window->baseline=window->chh-0;
  //  gfxPrimitivesSetFont(NULL,8,8); 	
  } else if(strcmp(name,"SMALL")==0 || strcmp(name,"5x7")==0) {
    window->chw=5;
    window->chh=7;
    window->baseline=window->chh-0;
   // gfxPrimitivesSetFont(asciiTable,5,7);
  }
}
