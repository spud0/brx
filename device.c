#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "utils.h"
#include "brx.h"

char * brx_show_ip (unsigned char * ip) {
	return NULL;
}

int show_device (char *tap_name) {

	if (!tap_name) return 1; 

	brx_device_info * info = get_device_metadata (...); 
	if (!info) return 1; 

	printf ("Device Name: [%s]\n", tap_name);
	printf ("MAC Address: [%s]\n", info->mac_address);
	printf ("IP  Address: [%s]\n", brx_show_ip(info->ip_address));
	printf ("MTU:         [%zu]\n",info->mtu);

	return 0; 
}


brx_device* create_device (char *tap_name) {

	if (!tap_name) return NULL; 

	// XXX: There is a very small chance that malloc fails	
	brx_device *dev = malloc (sizeof(brx_device));
	if (!dev) return NULL;  

	// XXX: Should be strncpy ??? 	
	dev->info.interface_name = tap_name; 	
	dev->ref_count = 1; 

	dev->tap_fd = tap_alloc (tap_name);
	if (dev->tap_fd < 0) {
		printf ("failed to allocate tap device"); 
		return NULL;
	}
	
	printf ("%s\n", tap_name); 

	// XXX: Make a series of system calls using ioctl to get 
	// information about the tap device 
	set_device_metadata (dev); 
	return dev; 
}


void set_device_metadata (brx_device *dev) {
	// TODO: Implement
	return;
}

brx_device_info * get_device_metadata (brx_device * dev) {
	return NULL;
}
