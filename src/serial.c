/* SERIAL.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* Helper functions to manage a serial device */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef WINDOWS
#include <termios.h>
#include <sys/ioctl.h>
#include <fnmatch.h>
#else
#define mkdir(a,b) mkdir(a)
#define O_NOCTTY 0
#define O_SYNC 0
#define B115200 0
#endif
#include <errno.h>

#include "serial.h"
#include "file.h"

int device_fd=-1;   /* serial device file descriptor (or -1 if not yet opened) */
int device_init_success=0;

extern int verbose;
extern int processinput(char *line);


/* Set up the (usb) serial device */

int device_setup(char *device,int baud) {
  /* Try to find the device */
  if(prepare_device(device)) return(-1);    /* If device not found */
  if(open_device(device,baud)) {
    perror("ERROR: could not open the USB-Device. No connection.");
    return(-8);
  }
  return(0);  
}


 /* Prepare the USB device, if everythin is OK, return 0, else return !0*/

int prepare_device(char *device) {
  int try=0;
  while(!exist(device) && try++<5) { 
    printf("WARNING: device %s existiert nicht!\n",device);
    if(*device) {
      device[strlen(device)-1]++;
      printf("Trying %s instead.\n",device);
    }
  }
  if(!exist(device)) {
    device[strlen(device)-1]='0';
    perror("ERROR: could not use any device. Giving up. Abort.\n");
    return(-1);
  }
  return 0;
}



/* open the serial device, if everything is OK, return 0, else return !0
 *
 */

int open_device(char *device, int baud) {
  if(verbose>=0) {printf("<-- %s [",device);fflush(stdout);}
  device_fd=open(device, O_RDWR | O_NOCTTY | O_SYNC);
  if(device_fd<0) {perror("opening device"); return(-1);}
  set_interface_attribs(device_fd, baud, 0);// set speed to 115,200 bps, 8n1 (no parity)
  set_blocking(device_fd,1);

  sleep(1); /* I am not sure, if this is necessary....*/
#ifndef WINDOWS
  int anz;
  ioctl(device_fd, FIONREAD, &anz);
  if(anz>0) {
    char buf[1024];
    if(verbose>=0) printf("%d can be read. --> ",anz);
    if(read (device_fd, buf, sizeof buf)<0) perror("read");
    if(verbose>=0) printf("buffer clear.");
  }
#endif
  if(verbose>=0) printf("]\n");
  return 0;
}


/* Check the state of the device. Is it still connected ?
   TODO: doesnt work yet.
 */

int checkstate(int fd) {
  int ret=0;
#if DEBUG
  pollfd a;
  a.fd=fd;
  a.events=POLLIN|POLLERR|POLLHUP;
  ret=poll(&a, 1, 100);
  printf(">>>>>> G %d %d\n",ret,a.revents);
#endif
  return(ret);
}



#ifdef WINDOWS

/* Handling the COM Port in WINDOWS needs to be implemented here....*/

int set_interface_attribs(int fd, int speed, int parity) {return(0);}
void set_blocking (int fd, int should_block) {}

#else
int set_interface_attribs(int fd, int speed, int parity) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if(tcgetattr(fd, &tty) != 0) {perror("error from tcgetattr");return -1;}

  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;	  // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
  tty.c_iflag &= ~IGNBRK;	  // disable break processing
  tty.c_lflag = 0;		  // no signaling chars, no echo,
  				  // no canonical processing
  tty.c_oflag = 0;		  // no remapping, no delays
  tty.c_cc[VMIN]  = 0;  	  // read doesn't block
  tty.c_cc[VTIME] = 5;  	  // 0.5 seconds read timeout
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
  tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
  				  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);	  // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if(tcsetattr (fd, TCSANOW, &tty) != 0) {perror("error  from tcsetattr");return -1;}
  return 0;
}

void set_blocking(int fd, int should_block) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if(tcgetattr (fd, &tty) != 0) {perror("error  from tggetattr");return;}
  tty.c_cc[VMIN]=should_block?1:0;
  tty.c_cc[VTIME]=5;  	  // 0.5 seconds read timeout
  if(tcsetattr(fd,TCSANOW,&tty)!=0)  perror("error %d setting term attributes");
}
#endif


/* The data taking loop. 
   This function is called inside an (endless) loop. 
   This could be outsourced to an independant thread.
   if the return value is <0, a read error occured.
 */

int device_loop() {   
  static char buf[512];
  static int anzbytes=0;
  int ret=0;
  int n,j;
    
    /* Here we want to monitor the state of the input device, 
     * if it is disconnected or so....
     */
    n=checkstate(device_fd);
    if(n<0) { /* This means fatal state, we should close it.*/
      perror("ERROR: Connection to device lost.");
      ret=-1;
      sleep(1);
      return(ret);
    }
    
    n=read(device_fd,&buf[anzbytes],sizeof buf-anzbytes); /* blocking read */
    /* Check for error */
    if(n<0) {
      perror("reading from device."); 
      if(errno==EIO) {
        perror("ERROR: reading from device. Connection lost.");
        printf("Probably the device was hotunplugged/disconnected.\n");
	/* The best we can do here is to close the device and try to 
	   reopen it after a while (in the server) or to just 
	   exit the program. */
	ret=-1;
      }
      sleep(1);
      return(ret);
    } else if(n==0) {
      /* This should mean:
      If no process has the pipe open for writing, read() shall return 0 to 
      indicate end-of-file. The same if the USB-Device is unplugged.
      It only works if the device was opened READ-ONLY !
      */
      perror("ERROR: reading nothing from device. Connection lost.");
      ret=-1;
      sleep(1);
      return(ret);
    }
    anzbytes+=n;
    buf[anzbytes]=0;
    #if DEBUG
      printf("read %d bytes. buf=<%s>",n,buf);
    #endif
    j=0;
    while(j<anzbytes) {
      if(buf[j]=='\n') {
	buf[j++]=0;
	processinput(buf);
	anzbytes-=j;
	memmove(buf,&buf[j],anzbytes);
	j=0;
      } else j++;
    }
  return(ret);
}


/* Close the device connection and do cleanup. */

void device_close() {
  if(device_fd!=-1) close(device_fd);
  device_fd=-1;
}
