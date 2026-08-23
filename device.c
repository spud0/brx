#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "utils.h"
#include "brx.h"

char * brx_show_ip (unsigned char * ip) {
	// snprint
	return NULL;
}

brx_device * get_device_by_name (brx_device ** devices, const char * name) {
	if (!devices) return NULL;
	
	size_t idx = 0; 
	brx_device * current = devices[idx]; 
	while (devices[idx] != NULL) {
		if (strncmp (name, current->info.interface_name, MAX_BUFFER) == 0) return current; 
		idx += 1; 
	}	

	return NULL;
}


char * show_device (char *tap_name) {

	#if 0
	if (!tap_name) return NULL; 

	brx_device_info * info = get_device_metadata (
		get_device_by_name (tap_name, get_devices (bridge))
	); 

	if (!info) return NULL; 

	// Use snprintf
	printf ("Device Name: [%s]\n", info->interface_name);
	printf ("MAC Address: [%s]\n", info->mac_address);
	printf ("IP  Address: [%s]\n", brx_show_ip(info->ip_address));
	printf ("MTU:         [%zu]\n",info->mtu);

	return ""; 
	#endif 
	return NULL; 
}


brx_device * create_device (char *tap_name) {

	if (!tap_name) return NULL; 

	// XXX: There is a very small chance that malloc fails	
	brx_device *dev = malloc (sizeof(brx_device));
	if (!dev) return NULL;  

	// XXX: Should be strncpy ??? 	
	dev->info.interface_name = tap_name; 	
	dev->ref_count++; 

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

void free_device (brx_device * dev) {

	if (!dev) return; 
	if (dev->ref_count > 0) return;

	if (dev->ref_count == 0) {
		// XXX: Remove from kernel data structure ...		
		close (dev->tap_fd); 
		free (dev); 
	}

	return;
}

void set_device_metadata (brx_device *dev) {
	// TODO: Implement
	return;
}

brx_device_info * get_device_metadata (brx_device * dev) {
	return NULL;
}

brx_device * get_devices (brx_bridge * bridge) {
	if (!bridge) return NULL; 
	
}

