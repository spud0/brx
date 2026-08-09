#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "brx.h"

// TODO: Actually implement
void show_usage () {
	printf (
		"Usage: brx <command> <> \n"
	);

	return; 
}


int tap_alloc (char *dev) {

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
	if (*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	if ((ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) { goto error; }

	strncpy(dev, ifr.ifr_name, IFNAMSIZ); 

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

int tap_fd;
volatile sig_atomic_t keep_running = 1;

void _signal_handler (int signal) { keep_running = 0; }


int main (int argc, char * argv[]) {

	if (argc < 2) {
		show_usage ();
		return 1; 
	} 

	// TODO: Add in getoptlong and proper commands

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

	while (keep_running) {

	}

	close (tap_fd);
	return 0;
}
