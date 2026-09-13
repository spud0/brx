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
	brx_message_type control_type, 
	brx_device_type dev_type, 
	size_t port_count,
	char * bridge_name,
	char * tap_name
) {

	brx_control_message * message = calloc (1, sizeof (brx_control_message));
	if (!message) return NULL; 

	message->control_type = control_type;
	message->dev_type = dev_type;

	if (control_type == CREATE || message->control_type == DELETE) {
		if (dev_type == TAP) {
			strncpy (message->payload.req.interface_name, tap_name, BRX_NAME_MAX - 1);
			message->payload.req.port_count = 0; 
		} 

		if (dev_type == BRIDGE) {
			strncpy (message->payload.req.interface_name, bridge_name, BRX_NAME_MAX - 1);
			message->payload.req.port_count = port_count; 
		} 
	}


	if (message->control_type == SHOW && dev_type == TAP)
		strncpy(message->payload.tap_info.info.interface_name, tap_name, BRX_NAME_MAX);

	if (message->control_type == SHOW && dev_type == BRIDGE)
		strncpy(message->payload.bridge_info.info.interface_name, bridge_name, BRX_NAME_MAX);

	return message; 
}

void free_message (brx_control_message * message) {
	free (message);
	return;
}


static int _write_full(int fd, const void *buf, size_t count) {
    size_t total = 0;
    const char *p = buf;
    while (total < count) {
        ssize_t n = write(fd, p + total, count - total);
        if (n <= 0) return -1; // error or peer closed mid-write
        total += (size_t) n;
    }
    return 0;
}

static int _read_full(int fd, void *buf, size_t count) {
    size_t total = 0;
    char *p = buf;
    while (total < count) {
        ssize_t n = read(fd, p + total, count - total);
        if (n <= 0) return -1; // error or peer closed
        total += (size_t) n;
    }
    return 0;
}

int send_brx_message(int fd, brx_control_message *msg) {
    return _write_full(fd, msg, sizeof(brx_control_message));
}

int recv_brx_message(int fd, brx_control_message *msg) {
    return _read_full(fd, msg, sizeof(brx_control_message));
}

// TODO: Reimplement this. 
int handle_bridge (char * arguments[], int length, int client_fd) {

	if ((!arguments) || (length == 0))  return 1; 

	const char* object = arguments[0];
	if (strncmp(object, "create", MAX_BUFFER) == 0) {

		if (length < 2) {
			perror ("Usage: brx bridge create <name> [ports]\n"); 	
			return 1; 
		} 

		// Defaults to 4 ports ...
		size_t port_count = (length >= 3) ? (size_t) atoi (arguments[2]) : 4; 

		brx_control_message * message = create_message (
			CREATE,
			BRIDGE,
			port_count,
			arguments[1],
			NULL
		);

		if (!message) {
			perror ("Failed to allocate control plane message\n");
			return 1; 
		} 

		if (send_brx_message(client_fd, message) < 0 || recv_brx_message(client_fd, message) < 0) {
			perror("brx: communication with daemon failed\n");
			free_message(message);
			return 1;
		}

		int result = (message->control_type == SUCCESS) ? 0 : 1;
		if (result != 0) { 
			fprintf(stderr, "brx: %s\n", message->error);
			free_message(message);
			return result; 
		} 

		printf("good\n");
		free_message(message);
		return result;

	} else if (strncmp (object, "show", MAX_BUFFER) == 0) {

		if (length < 2) {
			perror ("Usage: brx bridge create <name> [ports]\n"); 	
			return 1; 
		} 

		// Defaults to 4 ports ...
		size_t port_count = (length >= 3) ? (size_t) atoi (arguments[2]) : 4; 

		brx_control_message * message = create_message (
			SHOW,
			BRIDGE,
			port_count,
			arguments[1],
			NULL
		);

		if (!message) {
			perror ("Failed to allocate control plane message\n");
			return 1; 
		} 

		if (send_brx_message(client_fd, message) < 0 || recv_brx_message(client_fd, message) < 0) {
			perror("brx: communication with daemon failed\n");
			free_message(message);
			return 1;
		}

		int result = (message->control_type == SUCCESS) ? 0 : 1;
		if (result != 0) { 
			fprintf(stderr, "brx: %s\n", message->error);
			free_message(message);
			return result; 
		} 

		// Do something with the message ... 
		printf("interface name: %s\n", message->payload.bridge_info.info.interface_name);
		printf("port count: %zu\n", message->payload.bridge_info.info.port_count);
		printf("mac address: \n", message->payload.bridge_info.info.mac_address);
		printf("ip address: \n", message->payload.bridge_info.info.ip_address);
		free_message(message);
		return result;

	} else if (strncmp (object, "delete", MAX_BUFFER) == 0) {

		if (length < 2) {
			perror ("Usage: brx bridge create <name> [ports]\n"); 	
			return 1; 
		} 

		// Defaults to 4 ports ...
		size_t port_count = (length >= 3) ? (size_t) atoi (arguments[2]) : 4; 

		// TODO
		brx_control_message * message = create_message (
			DELETE,
			BRIDGE,
			port_count,
			arguments[1],
			NULL
		);

		if (!message) {
			perror ("Failed to allocate control plane message\n");
			return 1; 
		} 

		if (send_brx_message(client_fd, message) < 0 || recv_brx_message(client_fd, message) < 0) {
			perror("brx: communication with daemon failed\n");
			free_message(message);
			return 1;
		}

		int result = (message->control_type == SUCCESS) ? 0 : 1;
		if (result != 0) { 
			fprintf(stderr, "brx: %s\n", message->error);
			free_message (message);
			return result; 
		}
	
		printf("Deleted interface ... \n");
		free_message (message);
		return result;
		
	} else {
		perror ("Unknown bridge command or weird state reached somehow");
		return 1; 
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
