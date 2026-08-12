#pragma once

#define MAX_BUFFER 16

int handle_bridge (char * arguments[], int length); 
int handle_tap (char * arguments[], int length); 

int tap_alloc (char * tap_name); 
