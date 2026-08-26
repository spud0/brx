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

brx_control_message * create_message (
	message_type control_type, 
	device_type dev_type, 
	size_t port_count,
	char * bridge_name,
	char * tap_name
) {

	brx_control_message * message = malloc (sizeof (brx_control_message));
	if (!message) return NULL; 

	message->control_type = control_type;
	message->dev_type = dev_type;
	message->tap_name = tap_name;
	message->bridge_name = bridge_name;
	message->length = sizeof(brx_control_message);
	return message; 
}

void free_message (brx_control_message * message) {
	free (message);
	return;
}

// TODO: Reimplement this. 
int handle_bridge (char * arguments[], int length, int client_fd) {

	if ((!arguments) || (length == 0))  return 1; 

	// Need to have some sort of Control Plane Message 
	// int txed = write (client_fd, ); 

	const char* object = arguments[0];
	message_type type = ERROR; 

	if (strncmp(object, "create", MAX_BUFFER) == 0) {

		brx_control_message * message = create_message (
			CREATE,
			BRIDGE,
			(size_t) atoi (object[2]),
			object[0],
			NULL
		);

		int txed = write (client_fd, message, sizeof(brx_control_message));
		if (txed != sizeof(brx_control_message)) {
			// ERROR 
			printf("something weird with transmitting");
			return 1;
		}

		int rxed = read (client_fd, message, sizeof(brx_control_message));
		if (rxed != sizeof (brx_control_message)) {
			// ERROR
			printf("something weird with receiving");
			free_message(message);
			return 1;
		}

		if (message->control_type == SUCCESS) printf("good\n");
		if (message->control_type == ERROR) printf("not good\n");

		free_message(message);
		return 0; 

	} else if (strncmp (object, "show", MAX_BUFFER) == 0) {
		// return show_bridge (); 
	} else if (strncmp (object, "delete", MAX_BUFFER) == 0) {
		// return remove_bridge ();
	}  else if (strncmp (object, "add", MAX_BUFFER) == 0) {
		// return add_device_to_bridge ();
	}

	return 0; 
}


int handle_tap (char * arguments[], int length, int client_fd) {

	if ((!arguments) || (length == 0))  return -1; 

	const char* object = arguments[0];

	// XXX: Implement these
	if (strncmp(object, "create", MAX_BUFFER) == 0) {

		return  create_device (arguments[1]) == NULL; 
	} else if (strncmp (object, "show", MAX_BUFFER) == 0) {
		char * formatted_str = show_device (arguments[1]);
		printf ("%s\n", formatted_str); 
		return 0;
	} else if (strncmp (object, "delete", MAX_BUFFER) == 0) {
		// return remove_device (); 
	} else return 1; 

	return 1; 
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
