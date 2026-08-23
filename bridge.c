#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "utils.h"
#include "brx.h"


brx_bridge * create_bridge (char * bridge_name, size_t count) {
	if (!bridge_name) return NULL; 

	// XXX: There is a very small chance that malloc fails	
	brx_bridge * bridge = malloc (sizeof (brx_bridge));
	if (!bridge) return NULL;

	bridge->bridge_dev = create_device (bridge_name);
	bridge->device_count = count;

	return bridge; 
}

brx_forwarding_table * get_forwarding_table (void); 
void add_route (brx_forwarding_table * table); 


