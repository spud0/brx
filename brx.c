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



// XXX: Temporarily one giant function, will split into many functions
void print_interface_metadata(const char *interface_name) {
    int ctrl_sock;
    struct ifreq ifr;

    // 1. Open a temporary socket specifically for control plane commands
    ctrl_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctrl_sock < 0) {
        perror("Failed to open control socket");
        return;
    }

    // Target the specific interface name we are inspecting
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ);

    // 2. Fetch the Maximum Transmission Unit (MTU)
    if (ioctl(ctrl_sock, SIOCGIFMTU, &ifr) >= 0) {
        printf("Interface: %s | MTU Size: %d bytes\n", interface_name, ifr.ifr_mtu);
    }

    // 3. Fetch the Hardware (MAC) Address
    if (ioctl(ctrl_sock, SIOCGIFHWADDR, &ifr) >= 0) {
        unsigned char *mac = (unsigned char *) ifr.ifr_hwaddr.sa_data;
        printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    // 4. Fetch Operational Interface Flags (Up/Down, Running)
    if (ioctl(ctrl_sock, SIOCGIFFLAGS, &ifr) >= 0) {
        printf("Status: %s\n", (ifr.ifr_flags & IFF_UP) ? "UP" : "DOWN");
    }

    close(ctrl_sock);
	return;
}

// XXX: This can be moved when the interface is made persistent
volatile sig_atomic_t keep_running = 1;
void _signal_handler (int signal) { keep_running = 0; }


int main (int argc, char * argv[]) {

	signal (SIGHUP , _signal_handler);
	signal (SIGTERM, _signal_handler);
	signal (SIGINT , _signal_handler);


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

	// XXX: Once the interface is made persistent, this can be moved ...
	while (keep_running) { }

	badbye:
		show_usage();
		return 1; 

	goodbye: 
		return 0;
}
