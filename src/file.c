/* FILE.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "file.h"
#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif
/* Returns the length of the open file n */

size_t lof(FILE *n) {	
  long position=ftell(n);
  if(position==-1) {
    printf("I/O-Error: %d %s\n",errno,"lof");
    return(0);
  }
  if(fseek(n,0,SEEK_END)==0) {
    long laenge=ftell(n);
    if(laenge<0) printf("I/O-Error: %d %s\n",errno,"ftell"); 
    if(fseek(n,position,0)<0) printf("I/O-Error: %d %s\n",errno,"fseek"); 
    return(laenge);
  } else printf("I/O-Error: %d %s\n",errno,"fseek");
  return(0);
}
/* Returns the eof condition of an open file n */

int myeof(FILE *n) {
  int c=fgetc(n);
  ungetc(c,n);
  return c==EOF;
}
#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef S_IRGRP
#define S_IRGRP 0
#endif


/* Saves an area in memory starting at adr with length len to a file
   with filename name.
   RETURNS: 0 on success and -1 on error */

int bsave(const char *name, char *adr, size_t len) { 
  int fdis=open(name,O_CREAT|O_BINARY|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP);
  if(fdis==-1) return(-1);
  if(write(fdis,adr,len)==-1) {close(fdis); return(-1);}
  return(close(fdis));
}

/* loads the file with filename name to a memory location at address adr.
   a maximum of len bytes are read. if len==-1 the whole file is read. 
   RETURNS the number of read bytes or 0 on error.
   */

size_t bload(const char *name, char *adr, size_t len) {	
  FILE *fdis=fopen(name,"rb");
  if(fdis==NULL) return(0);
  if(len==-1) len=lof(fdis);
  if(len>0) len=fread(adr,1,len,fdis);
  fclose(fdis);
  return(len);
}

STRING get_file(const char *name) {
  STRING ret;
  FILE *fdis=fopen(name,"rb");
  if(fdis==NULL) {
    ret.pointer=NULL;
    ret.len=0;
    return(ret);
  }
  size_t len=lof(fdis);
  if(len>0) {
    ret.pointer=malloc(len+1);
    ret.len=fread(ret.pointer,1,len,fdis);
    ret.pointer[ret.len]=0;
  }
  fclose(fdis);
  return(ret);
}


int exist(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) return(FALSE);
  return(TRUE);
}
/* read a whole line from an open (ASCII) file n 
 * reads until \n or EOF 
 * reads at maximum size bytes. */

char *lineinput(FILE *n, char *line,int size) {
  int c; int i=0;
  while((c=fgetc(n))!=EOF) {
    if(c==(int)'\n') {
      line[i]=(int)'\0';
      return line;
    } else line[i++]=(char)c;
    if(i>=size-1) break;
  }
  line[i]='\0';
  return line;
}
