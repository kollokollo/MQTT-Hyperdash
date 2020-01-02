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
#include "basics.h"
#include "graphics.h"
#include "hyperdash.h"
#include "file.h"
#include "mqtt.h"

#ifndef WINDOWS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#include <SDL/SDL.h>
//#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

int init_sdl() {
  static int done=0;
  if(done) return(0);
#ifndef WINDOWS
  XInitThreads();
#endif
//  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) return -1;
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0 ) return -1;
  
  /* Initialize SDL_ttf  */
    if(TTF_Init()==-1) return -1;
  
  atexit(SDL_Quit);
  
  /* Enable Unicode translation */
  SDL_EnableUNICODE(1);
  done=1;
  return(0);
}

void close_window(WINDOW *win) {
  free(win);
}

#define WINDOW_DEFAULT_W 512
#define WINDOW_DEFAULT_H 256
WINDOW *create_window(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h, int fullscreen) {
  WINDOW *nw;
  nw=calloc(sizeof(WINDOW),1);
  init_sdl();
  
  if(!(nw->display=SDL_SetVideoMode(w,h, 32,
       SDL_FULLSCREEN*fullscreen |
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


int handle_event(WINDOW *w,SDL_Event *event) {
  switch (event->type) {
    /* Das Redraw-Event */
/* wahrscheinlich muessen wir gar nix tun...*/
  case SDL_QUIT:
    /* OOps, window close request. What should I do? */
    return(-1);
    break;
  case SDL_ACTIVEEVENT: 
    if(event->active.state&SDL_APPACTIVE) {
        if ( event->active.gain ) {
            printf("App activated\n");
        } else {
            printf("App iconified\n");
        }
    }
  #ifdef SDL_WINDOWEVENT
  case SDL_WINDOWEVENT:
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN:
      printf("Window %d shown", event->window.windowID);break;
    case SDL_WINDOWEVENT_HIDDEN:
      printf("Window %d hidden", event->window.windowID);break;
    case SDL_WINDOWEVENT_EXPOSED:
      printf("Window %d exposed", event->window.windowID);break;
    case SDL_WINDOWEVENT_MOVED:
      printf("Window %d moved to %d,%d",event->window.windowID, event->window.data1,event->window.data2);break;
    case SDL_WINDOWEVENT_RESIZED:
      printf("Window %d resized to %dx%d",event->window.windowID, event->window.data1,event->window.data2);break;
    case SDL_WINDOWEVENT_MINIMIZED:
      printf("Window %d minimized", event->window.windowID);break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      printf("Window %d maximized", event->window.windowID);break;
    case SDL_WINDOWEVENT_RESTORED:
      printf("Window %d restored", event->window.windowID);break;
    case SDL_WINDOWEVENT_ENTER:
      printf("Mouse entered window %d",event->window.windowID);break;
    case SDL_WINDOWEVENT_LEAVE:
      printf("Mouse left window %d", event->window.windowID);break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      printf("Window %d gained keyboard focus",event->window.windowID);break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      printf("Window %d lost keyboard focus",event->window.windowID);break;
    case SDL_WINDOWEVENT_CLOSE:
      printf("Window %d closed", event->window.windowID);break;
    default:
      printf("Window %d got unknown event %d",event->window.windowID, event->window.event);break;
    }
  #endif
  }
  return(0);
}





int waitmouse(WINDOW *window) {
  SDL_Event event;
  if(SDL_WaitEvent(&event)==0) return(0);
  while(event.type!=SDL_MOUSEBUTTONUP) { 
     if(handle_event(window,&event)==-1) return(-1);
     if(SDL_WaitEvent(&event)==0) return(0);
  }
  return(1);
}
int mouseevent(WINDOW *window, int *x, int *y, int *b, int *s) {
  SDL_Event event;
  if(SDL_WaitEvent(&event)==0) return(0);
  while(event.type!=SDL_MOUSEBUTTONDOWN && event.type!=SDL_MOUSEBUTTONUP) { 
     if(handle_event(window,&event)==-1) return(-1);
     if(SDL_WaitEvent(&event)==0) return(0);
//     printf("Event-Loop\n");
     /* Here we can check if the connection to he broker is still ok.*/
     if(!mqtt_isconnected) return(-2); /*Connection lost */  
  }
  *x=event.button.x;
  *y=event.button.y;
  *b=event.button.button;
  *s=event.button.state;
  return(1);
}




void put_graphics(WINDOW *window, STRING data,int x,int y,unsigned int w, unsigned int h, unsigned long int tgc) {
  SDL_RWops *rw=SDL_RWFromMem(data.pointer,data.len);
  SDL_Surface *bmpdata=SDL_LoadBMP_RW(rw,1);
  SDL_Surface *image=SDL_DisplayFormat(bmpdata);
  SDL_SetColorKey(image,SDL_SRCCOLORKEY,tgc); /*set transparent color*/

  SDL_FreeSurface(bmpdata);
/*

  SDL_Surface *bmpdata;
  SDL_Surface *image;
  bmpdata=bpmtosurface(plist[2].pointer,scale);
  image=SDL_DisplayFormat(bmpdata);
  SDL_FreeSurface(bmpdata);
*/
  int sx=0;
  int sy=0;
  int sw=w;
  int sh=h;
  sw=min(image->w-sx,sw);
  sh=min(image->h-sy,sh);
  if(sw>0 && sh>0) {
    SDL_Rect a={sx,sy,image->w,image->h};
    SDL_Rect b={x,y,0,0};
    SDL_BlitSurface(image, &a,window->display, &b);
  }
  SDL_FreeSurface(image);
}





/*Eine einfache Bitmap auf den Screen bringen, hierbei gibt es 
  fordergrundfarbe, hintergrundfarbe und Graphmodi zu berücksichtigen.
  */

void put_bitmap(WINDOW *window, const char *adr,int x,int y,unsigned int w, unsigned int h, unsigned long int fgc) {
  SDL_Surface *data;
  SDL_Surface *image;
  int bpl=(w+1)>>3;
  data=SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,1, 0,0,0,0);
  memrevcpy(data->pixels,adr,bpl*h);
  // memcpy(data->pixels,adr,bpl*h);
  data->pitch=bpl;
  
    SDL_SetColorKey(data,SDL_SRCCOLORKEY,0);
 
//  memdump((char *)(data->format),sizeof(SDL_PixelFormat));
//  memdump((char *)(data->format->palette),sizeof(SDL_Palette));
//  memdump((char *)(data->format->palette->colors),2*sizeof(SDL_Color));
  unsigned char *pal=(unsigned char *)(data->format->palette->colors);
  uint32_t col=window->bcolor;
  col>>=8;
  pal[2]=(col&0xff); col>>=8;
  pal[1]=(col&0xff); col>>=8;
  pal[0]=(col&0xff);
  col=fgc; col>>=8;
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

void put_font_text(WINDOW *window, const char *font, int size,char *text, int x,int y,unsigned long int fgc, int h) {
  SDL_Surface *message = NULL;
  TTF_Font *ttffont = NULL;
  SDL_Color textColor = { (fgc&0xff000000)>>24, (fgc&0xff0000)>>16, (fgc&0xff00)>>8 };
  char fontname[256];
  sprintf(fontname,"%s/%s.ttf",fontdir,font);
  if(!exist(fontname)) {
    if(strcmp(font,"SMALL")) printf("ERROR: font not found: <%s>\n",fontname);
    stringColor(window->display,x,y+(h-8)/2,text,fgc);
    return;
  }
  ttffont=TTF_OpenFont(fontname,size);
  
  if(ttffont==NULL) {
    stringColor(window->display,x,y+(h-8)/2,text,fgc);
    return;
  }
  int fontheight=TTF_FontHeight(ttffont);

//  message = TTF_RenderText_Solid(ttffont,text, textColor);
  message = TTF_RenderUTF8_Blended(ttffont,text, textColor);

  /* If there was an error in rendering the text, use the primitive font */
  
  if(message==NULL) {
    TTF_CloseFont(ttffont);
    stringColor(window->display,x,y+(h-8)/2,text,fgc);
    return;
  }

  SDL_Rect a={0,0,message->w,message->h};
  SDL_Rect b={x,y+(h-fontheight)/2,message->w,message->h};
  SDL_BlitSurface(message, &a,window->display, &b);
  SDL_FreeSurface(message);
  TTF_CloseFont(ttffont);
}
