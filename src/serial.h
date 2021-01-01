/* SERIAL.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* Helper functions to manage a serial device */


int device_setup(char *device,int baud);
int prepare_device(char *device);
int open_device(char *device,int baud);
int device_loop();
int checkstate(int fd);
void device_close();
int set_interface_attribs(int fd, int speed, int parity);
void set_blocking(int fd, int should_block);


extern int device_fd;   /* serial device file descriptor (or -1 if not yet opened) */
extern int device_init_success;
