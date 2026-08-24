#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "brx.h"
#include "utils.h"


// TODO: Actually implement

void show_usage () {
	printf (
		"Usage: brx <tap | bridge> <show | set | get> <address | table | ...> \n"
		"When working with a tap in  brx the subcommands are: <create | show | get>  \n"
		"When working with a bridge in brx the subcommands are: <create | show | get>  \n"
 		" <address | table | peer ??? > \n" 
	);

	return; 
}            


brx_bridge * bridge = NULL; 

// XXX: Actually fix this implementation so its for bridges ...
int show_bridge (char *tap_name) {

	if (!tap_name) return 1; 

	brx_device_info * info = &bridge->bridge_dev->info;

	if (!info) return 1; 

	printf ("Device Name: [%s]\n", info->interface_name);
	printf ("MAC Address: [%s]\n", info->mac_address);
	printf ("IP  Address: [%s]\n", brx_show_ip (info->ip_address));
	printf ("MTU:         [%zu]\n",info->mtu);

	return 0; 
}



int main (int argc, char * argv[]) {

	// int socket = socket (AF_UNIX)


	const char *short_opts = "hv";

	struct option long_opts [] = {
		{"help", no_argument, NULL, 'h' },
		{"verbose", no_argument, NULL, 'h' },
		{NULL, 0, NULL, 0 },
	};

	// TODO: Add in getoptlong and proper commands
	int opt;
	int verbose = 0; 

	while ((opt = getopt_long (argc, argv, short_opts, long_opts, NULL)) != -1) { 
		switch (opt) {
			case 'h': 
				show_usage(); 
				goto goodbye; 	

			case 'v': 
				verbose = 1; 
				break;

			default:
				show_usage ();
				goto badbye; 
		}
	}

	// Get the rest of the command line arguments
	int remaining = argc - optind; 	
	char **arguments = &argv[optind]; 

	// If I have a bridge handle differently than if I have a tap OR port
	const char *object = arguments[0]; 		
	int result; 
	if (strncmp (object, "bridge", MAX_BUFFER) == 0) {
		result = handle_bridge (&arguments[1], remaining - 1);
	} else if (strncmp (object, "tap", MAX_BUFFER) == 0) {
		result = handle_tap (&arguments[1], remaining - 1); 
	} else 
		goto badbye; 

	if (result) goto badbye; 	


	badbye:
		show_usage();
		return 1; 

	goodbye: 
		return 0;

}