/* BITMAP.C (c) Markus Hoffmann  */

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
#ifdef WINDOWS
  #include <windows.h>
#endif
#include "bitmap.h"
#include "lodepng.h"

/* We need these because ARM has 32 Bit alignment (and the compiler has a bug)*/
static void writeint(unsigned char *p,unsigned int n) {
  *p++=(unsigned char)(n&0xff);
  *p++=(unsigned char)((n&0xff00)>>8);
  *p++=(unsigned char)((n&0xff0000)>>16);
  *p++=(unsigned char)((n&0xff000000)>>24);
}
static void writeshort(unsigned char *p,unsigned short n) {
  *p++=(unsigned char)(n&0xff);
  *p++=(unsigned char)((n&0xff00)>>8);
}


STRING pngtobmp(unsigned char *pngdata,size_t pngsize) {
  STANDARDBITMAP stdbmp;
  STRING ergebnis;
  ergebnis.pointer=NULL;
  ergebnis.len=0;
  unsigned int error = lodepng_decode32(&stdbmp.image, &stdbmp.w, &stdbmp.h,pngdata,pngsize);
  if(error) printf("PNGDECODE: error %u\n", error);
  else ergebnis.pointer=(char *)stdbmtobmp(stdbmp,(int *)&ergebnis.len);
  free(stdbmp.image);
  return(ergebnis);
}

unsigned char *stdbmtobmp(STANDARDBITMAP bmp, int *len) {
  int l=BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN+bmp.h*bmp.w*4;
  unsigned char *data=malloc(l);
  BITMAPFILEHEADER *header=(BITMAPFILEHEADER *)data;
  BITMAPINFOHEADER *iheader=(BITMAPINFOHEADER *)(data+BITMAPFILEHEADERLEN);
  header->bfType=BF_TYPE;
  writeint((unsigned char *)&data[10],BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN);
  writeint((unsigned char *)&(iheader->biSize),BITMAPINFOHEADERLEN);
  writeint((unsigned char *)&(iheader->biWidth),bmp.w);
  writeint((unsigned char *)&(iheader->biHeight),bmp.h);
  writeshort((unsigned char *)&(iheader->biPlanes),1);
  writeshort((unsigned char *)&(iheader->biBitCount),32);
  writeint((unsigned char *)&(iheader->biCompression),BI_RGB);
  writeint((unsigned char *)&(iheader->biSizeImage),0);
  writeint((unsigned char *)&(iheader->biXPelsPerMeter),0);
  writeint((unsigned char *)&(iheader->biYPelsPerMeter),0);
  writeint((unsigned char *)&(iheader->biClrUsed),0);
  writeint((unsigned char *)&(iheader->biClrImportant),0);
  unsigned char *buf2=data+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN;
  unsigned char *buf3=buf2;
  unsigned char *scanline;
  int i,j;
  for(i=bmp.h-1;i>=0;i--) {
    scanline=(unsigned char *)bmp.image+i*bmp.w*4;
    for(j=0;j<bmp.w;j++) {
      *buf2++=scanline[2];  /*  B  */
      *buf2++=scanline[1];  /*  G  */
      *buf2++=scanline[0];  /*  R  */
      *buf2++=scanline[3];  /*  A  */
      scanline+=4;
    }    
    buf2=buf3+(((buf2-buf3)+3)&0xfffffffc); /* align to 4 */
  }
  l=(buf2-buf3)+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN;
  writeint(&data[2],l);
  if(len) *len=l;
  return(data);
}


