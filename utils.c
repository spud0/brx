#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "utils.h"
#include "brx.h"

// Create a tap device
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

	// XXX: For a persistent tap interface, note that if brx ends, the virtual bridge 
	// disappears so the tap interfaces are left in kernel state, so they must be 
	// cleaned up !!!
	// if (ioctl(fd, TUNSETPERSIST, 1) < 0) goto error;

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
		// return create_bridge (); 
	} else if (strncmp (object, "show", MAX_BUFFER) == 0) {
		// return show_bridge (); 
	} else if (strncmp (object, "delete", MAX_BUFFER) == 0) {
		// return remove_bridge ();
	}  else if (strncmp (object, "add", MAX_BUFFER) == 0) {
		// return add_device_to_bridge ();
	}

	return 1; 
}


int handle_tap (char * arguments[], int length) {

	if ((!arguments) || (length == 0))  return -1; 

	const char* object = arguments[0];

	// XXX: Implement these
	if (strncmp(object, "create", MAX_BUFFER) == 0) {
		return  create_device (arguments[1]) == NULL; 
	} else if (strncmp (object, "show", MAX_BUFFER) == 0) {
		return show_device (arguments[1]); 
	} else if (strncmp (object, "delete", MAX_BUFFER) == 0) {
		// return remove_device (); 
	} else return 1; 

	return 1; 
}


