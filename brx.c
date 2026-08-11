#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/if_tun.h>


#include "brx.h"


// TODO: Actually implement
void show_usage () {
	printf (
		"Usage: brx <tap | bridge> <show | set | get> <address | table | ...> \n"
	);

	return; 
}


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

	#if 0
	printf ("mac addr: %s\n", ifr.ifr_hwaddr.sa_data);
	printf ("ip addr: %s\n", ifr.ifr_addr.sa_data);
	printf ("mtu: %d\n", ifr.ifr_mtu);
	printf ("interface index %d\n", ifr.ifr_ifindex);
	#endif

	return fd;

	error: 
		close (fd); return -1;
}             

// Temporarily one giant function, will split into many functions
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

int tap_fd;
volatile sig_atomic_t keep_running = 1;

void _signal_handler (int signal) { keep_running = 0; }


int main (int argc, char * argv[]) {

	if (argc < 2) {
		show_usage ();
		return 1; 
	} 

	const char *_short_options = ":h:";

	struct option _long_options [] = {
		{"help", no_argument, NULL, 'h' },
		{"verbose", no_argument, NULL, 'h' },
		{NULL, 0, NULL, 0 },
	}

	// TODO: Add in getoptlong and proper commands
	int opt;
	int verbose = 0; 

	if ((opt = getopt_long (argc, argv, short_opts, long_opts, NULL)) < 0) { // XXX; IDK
		switch (opt) {
			case 'h': 
				show_usage(); 
				break; 
	
			default:
				printf ("ERROR: Unexpected value parsing command-line arguments");
				break;

		}
	}



	char *tap_name = argv[1];
	printf("tap-name: %s\n", tap_name);
	
	tap_fd = tap_alloc (tap_name);
	if (tap_fd < 0) {
		printf ("failed to allocate tap device"); 
		return 1;
	}

	signal (SIGHUP , _signal_handler);
	signal (SIGTERM, _signal_handler);
	signal (SIGINT , _signal_handler);

	// XXX: remove this ...
	print_interface_metadata (tap_name);

	// XXX: Add some brx stuff to create a bridge
	while (keep_running) {
	
	}

	close (tap_fd);
	return 0;
}
