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
#include "util.h"
#include "mqtt.h"

#ifndef WINDOWS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

static int init_sdl() {
  static int done=0;
  if(done) return(0);
#ifndef WINDOWS
  XInitThreads();
#endif
//  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) return -1;
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0 ) return -1;
  
  /* Initialize SDL_ttf  */
    if(TTF_Init()==-1) return -1;
    
  /* Enable Unicode translation */
  SDL_EnableUNICODE(1);
  done=1;
  return(0);
}

void close_window(WINDOW *win) {
  SDL_Quit();
  if(win->title) free(win->title);
  if(win->info) free(win->info);
  free(win);
}

#define WINDOW_DEFAULT_W 512
#define WINDOW_DEFAULT_H 256
WINDOW *create_window(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h, int fullscreen) {
  WINDOW *nw=calloc(sizeof(WINDOW),1);
  init_sdl();
  
  if(!(nw->display=SDL_SetVideoMode(w,h, 32,
       SDL_FULLSCREEN*fullscreen |
       SDL_HWSURFACE|SDL_SRCALPHA))) {
      printf("cannot open SDL surface \n");
      close_window(nw);
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
        if(event->active.gain) {
            if(verbose>0) printf("App activated\n");
        } else {
            if(verbose>0) printf("App iconified\n");
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
  if(data.len==0 || !data.pointer) return;
  SDL_RWops *rw=SDL_RWFromMem(data.pointer,data.len);
  SDL_Surface *bmpdata=SDL_LoadBMP_RW(rw,1);
  
  
  
  SDL_Surface *image=SDL_DisplayFormat(bmpdata);
  if(image) {
    SDL_SetColorKey(image,SDL_SRCCOLORKEY,tgc); /*set transparent color*/
    SDL_FreeSurface(bmpdata);
  } else {  
    if(verbose>0) printf("Error: SDL_DisplayFormat\n");
    image=bmpdata;
  }

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
  if(image) {
    SDL_FreeSurface(data);
  } else {
    if(verbose>0) printf("Error: SDL_DisplayFormat\n");
    image=data;
  }
    SDL_Rect a={0,0,image->w,image->h};
    SDL_Rect b={x,y,image->w,image->h};
//  printf("putbitmap: %dx%d %d %d\n",image->w,image->h,bpl,image->pitch);
    SDL_BlitSurface(image, &a,window->display, &b);
  SDL_FreeSurface(image);
}

void put_font_text(SDL_Surface *display, int fidx, char *text, int x,int y,unsigned long int fgc, int h) {
  SDL_Surface *message = NULL;
  TTF_Font *ttffont = NULL;
  SDL_Color textColor = { (fgc&0xff000000)>>24, (fgc&0xff0000)>>16, (fgc&0xff00)>>8 };
  if(fidx>=0) ttffont=fonts[fidx].font;
  else ttffont=NULL;
  if(ttffont==NULL) {
    stringColor(display,x,y+(h-8)/2,text,fgc);
    return;
  }
  int fontheight=fonts[fidx].height;

//  message = TTF_RenderText_Solid(ttffont,text, textColor);
  message = TTF_RenderUTF8_Blended(ttffont,text, textColor);

  /* If there was an error in rendering the text, use the primitive font */
  
  if(message==NULL) {
    stringColor(display,x,y+(h-8)/2,text,fgc);
    return;
  }

  SDL_Rect a={0,0,message->w,message->h};
  SDL_Rect b={x,y+(h-fontheight)/2,message->w,message->h};
  SDL_BlitSurface(message, &a,display, &b);
  SDL_FreeSurface(message);
}

static int get_textline(char *text,TTF_Font *ttffont, int w) {
  if(!text || !*text) return(-1);
  char buffer[256];
  int tw=0, th=8, i=0;
  while(text[i] && text[i]!='\n' && i<sizeof(buffer)-1) {
    buffer[i]=text[i];
    buffer[++i]=0;
    if(ttffont) TTF_SizeUTF8(ttffont, buffer, &tw, &th);
    else tw=i*8;
    if(tw>=w) return(i-1);
  }
  return(i);
}

void put_font_long_text(SDL_Surface *display, int fidx, char *text, int x,int y,unsigned long int fgc, int w, int h) {
  TTF_Font *ttffont=fonts[fidx].font;
  int fontheight=fonts[fidx].height;
  char buffer[256];
  int z,z0=0;
  int yy=y;
  z=get_textline(text,ttffont,w);
  while(z>=0) {
    if(z>=sizeof(buffer)) z=sizeof(buffer)-1;
    memcpy(buffer,&text[z0],z);
    buffer[z]=0;
    if(z==0) z0++;
    else z0+=z;
    if(text[z0]=='\n') z0++;
    put_font_text(display,fidx,buffer, x,yy,fgc,fontheight);
    yy+=fontheight;
    if(yy>y+h-fontheight) break;
    z=get_textline(&text[z0],ttffont,w);
  }
}
FONTDEF fonts[256];
int anzfonts=0;
int find_font(const char *name,int size) {
  int ret=-1;
  int i;
  if(anzfonts>0) {
    for(i=0;i<anzfonts;i++) {
      if(!strcmp(name,fonts[i].name) && size==fonts[i].size) return(i); 
    }
  }
  return(ret);
}
void clear_font(int idx) {
  if(idx>=0 && idx<anzfonts) {
    fonts[idx].anz=0;
    free(fonts[idx].name);
    fonts[idx].name=NULL;
    TTF_CloseFont(fonts[idx].font);
    fonts[idx].font=NULL;
    fonts[idx].size=0;
    if(idx==anzfonts-1) anzfonts--;
  }
  while(anzfonts>0 && fonts[anzfonts-1].anz==0) {
     anzfonts--;
  }
}
void clear_all_fonts() {
  int i;
  if(anzfonts>0) {
    for(i=0;i<anzfonts;i++) {
      fonts[i].anz=0;
      free(fonts[i].name);
      fonts[i].name=NULL;
      TTF_CloseFont(fonts[i].font);
      fonts[i].font=NULL;
      fonts[i].size=0;
    }
  }
  anzfonts=0;
}
int add_font(const char *name, int size) {
  int i=find_font(name,size);
  if(i>=0) fonts[i].anz++;
  else {
    i=anzfonts;
    anzfonts++;
    fonts[i].anz=1;
    fonts[i].size=size;
    fonts[i].name=strdup(name);
    fonts[i].font=NULL;
  }
  return(i);
}

void open_font(int i) {
  char fontname[256];
  if(fonts[i].anz>0) {
    snprintf(fontname,sizeof(fontname),"%s/%s.ttf",fontdir,fonts[i].name);
    if(!exist(fontname)) {
      if(strcmp(fonts[i].name,"SMALL")) printf("ERROR: font not found: <%s>\n",fontname);
      fonts[i].font=NULL;
      fonts[i].height=8;
    } else {
      if(verbose>0) printf("%d: %s : %d\n",i,fontname,fonts[i].size);
      fonts[i].font=TTF_OpenFont(fontname,fonts[i].size);
      if(fonts[i].font==NULL) {
    	printf("ERROR: could not open font <%s> size=%d\n",fontname,fonts[i].size);
  	fonts[i].height=8;
      } else fonts[i].height=TTF_FontHeight(fonts[i].font);
    }
  }
}

void open_all_fonts() {
  if(verbose>0) printf("Open_all_fonts:\nwe have %d fonts:\n",anzfonts);
  if(anzfonts>0) {
    int i;
    for(i=0;i<anzfonts;i++) open_font(i);
  }
}
STRING get_icon(const char *name, int *w, int *h) {
  STRING ret;
  STRING a;
  a=get_file(name);
  ret=pngtobmp((unsigned char *)a.pointer,(size_t)a.len);
  free(a.pointer);
  return(ret);
}

#define SmallDisc_width 11
#define SmallDisc_height 11

static char SmallDisc_bits[] = {
   0xfc, 0x01, 0x02, 0x02, 0x07, 0x07, 0x8f, 0x07, 0xdf, 0x07, 0xff, 0x07,
   0xdf, 0x07, 0x8f, 0x07, 0x07, 0x07, 0x02, 0x02, 0xfc, 0x01};

STRING get_bitmap(const char *name, int *w, int *h) {
  int i=0;
  int e;
  STRING ret;
  char *odummy;
  char dummy[256];
  STRING a=get_file(name);
  if(a.pointer && a.len) {
    ret.pointer=malloc(a.len);
    ret.len=0;
    wort_sep(a.pointer,'_',0,dummy,a.pointer);
    wort_sep(a.pointer,' ',0,dummy,a.pointer);
    wort_sep(a.pointer,'\n',0,dummy,a.pointer);
    *w=(atoi(dummy)+7)&(~7);
    wort_sep(a.pointer,'_',0,dummy,a.pointer);
    wort_sep(a.pointer,' ',0,dummy,a.pointer);
    wort_sep(a.pointer,'\n',0,dummy,a.pointer);
    *h=atoi(dummy);
    e=wort_sep(a.pointer,'{',0,dummy,a.pointer);
    e=wort_sep(a.pointer,'}',0,a.pointer,dummy);
    e=wort_sep(a.pointer,',',0,dummy,a.pointer);
    while(e>0) {
      odummy=dummy;
      while(odummy[0]<=' ') odummy++;
      // printf("%d:<%s> <%s>: %x\n",i,dummy,odummy,(int)myatof(odummy));
      ret.pointer[i++]=(int)myatof(odummy);
      e=wort_sep(a.pointer,',',0,dummy,a.pointer);
    }
    ret.len=i;
    free(a.pointer);
  } else {
    ret.pointer=SmallDisc_bits;
    ret.len=sizeof(SmallDisc_bits);
    *w=(SmallDisc_width+7)&0xfffffc;
    *h=SmallDisc_height;
  }
  return(ret);
}

