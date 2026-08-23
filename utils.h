#pragma once

#define MAX_BUFFER 16

int handle_bridge (char * arguments[], int length); 
int handle_tap (char * arguments[], int length); 

int tap_alloc (char * tap_name); 

char * brx_show_ip (unsigned char * ip);
char * brx_show_mac (unsigned char * ip);
char * show_device (char *tap_name);

void print_interface_metadata(const char *interface_name);