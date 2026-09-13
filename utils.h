#pragma once

#include <stdlib.h>

#define MAX_BUFFER 16

#define SOCKET_PATH "/tmp/brx-control-uds.sock"
#define MAX_CONNS 32

int handle_bridge (char * arguments[], int length, int client_fd); 
int handle_tap (char * arguments[], int length, int client_fd); 

int tap_alloc (char * tap_name); 

char * brx_show_ip (unsigned char * ip);
char * brx_show_mac (unsigned char * ip);
char * show_device (char *tap_name);

void print_interface_metadata(const char *interface_name);

static int close_non_standard_fds (void);
static int write_full(int fd, const void *buf, size_t count);
static int read_full(int fd, void *buf, size_t count); 
