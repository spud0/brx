#include <stdlib.h>
#include <stdio.h>

#include <linux/if.h>

#include "utils.h"
#include "brx.h"

// Create a tap
int tap_alloc (char *name) {

	struct ifreq ifr;
	int fd;

	if ((fd = open ("/dev/net/tun", O_RDWR)) < 0 )
		return -1;  

	memset(&ifr, 0, sizeof(ifr));

	/*  Flags: 
	*	IFF_TUN   - TUN device (no Ethernet headers) 
	*   IFF_NO_PI - Do not provide packet information  
	*/ 

	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	if (*name) strncpy(ifr.ifr_name, name, IFNAMSIZ);
	if ((ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) { goto error; }

	strncpy(name, ifr.ifr_name, IFNAMSIZ); 
	return fd;

	error: 
		close (fd); 
		return -1;
} 


int handle_bridge (char * arguments[], int length) {

	if ((!arguments) || (length == 0))  return -1; 

	const char* object = arguments[0];

	if (strncmp(object, "create", MAX_BUFFER) == 0) {
		return create_bridge (); 
	} else if (strncmp (object, "show", MAX_BUFFER) == 0) {
		return show_bridge (); 
	} else if (strncmp (object, "delete", MAX_BUFFER) == 0) {
		return remove_bridge ();
	} 

	return -1; 
}


int handle_tap (char * arguments[], int length) {

	if ((!arguments) || (length == 0))  return -1; 

	const char* object = arguments[0];

	// XXX: Implement these
	if (strncmp(object, "create", MAX_BUFFER) == 0) {
		return create_device (arguments[1]) != NULL;
	} else if (strncmp (object, "show", MAX_BUFFER) == 0) {
		return show_device (); 
	} else if (strncmp (object, "delete", MAX_BUFFER) == 0) {
		return remove_device (); 
	} 

	return -1; 
}

brx_device* create_device (char *name) {

	if (!name) return NULL; 
	
	brx_device *dev = malloc (sizeof(brx_device));

	// XXX: Should be strncpy ??? 	
	dev->info.interface_name = name; 	
	dev->info.fd = tap_alloc (tap_name);
	if (dev->info.tap_fd < 0) {
		printf ("failed to allocate tap device"); 
		return NULL;
	}

	return dev; 
	
	
}
