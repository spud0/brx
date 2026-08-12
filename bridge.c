#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "utils.h"
#include "brx.h"

// XXX: Actually fix this implementation so its for bridges ...
int show_bridge (char *tap_name) {

	if (!tap_name) return 1; 

	brx_device_info * info = get_device_metadata (
		get_device_by_name (tap_name, get_devices (bridge))
	); 

	if (!info) return 1; 

	printf ("Device Name: [%s]\n", info->interface_name);
	printf ("MAC Address: [%s]\n", info->mac_address);
	printf ("IP  Address: [%s]\n", brx_show_ip(info->ip_address));
	printf ("MTU:         [%zu]\n",info->mtu);

	return 0; 
}

brx_bridge * create_bridge (char * bridge_name, size_t count) {
	if (!bridge_name) return NULL; 

	// XXX: There is a very small chance that malloc fails	
	brx_bridge * bridge = malloc (sizeof (brx_bridge));
	if (!bridge) return NULL;

	bridge->bridge_dev = create_device (bridge_name);
	bridge->device_count = count;

}

brx_forwarding_table * get_forwarding_table (void); 
void add_route (brx_forwarding_table * table); 


