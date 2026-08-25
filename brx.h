#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAC_ADDR_SIZE 6
#define IP_ADDR_SIZE 4


// XXX: An entry in the switch/bridge's "forwarding database", almost like a key value ...
typedef struct brx_fdb_entry {
	unsigned char mac_address[MAC_ADDR_SIZE]; 
	size_t port_no; 
	time_t last_seen;
	bool has_static_mapping; 
} brx_fdb_entry; 


// XXX: A list of entries, capacity (determined on program invocation), & current entries used
typedef struct brx_forwarding_table {
	brx_fdb_entry * entries; 
	size_t entry_count; 
	size_t capacity; 
} brx_forwarding_table; 


// A port can have a device "plugged in or not". A port must have a state of "is plugged into",  
// a port number and possibly other stuff. 

// XXX: This is why the device is a pointer, if the value of the pointer is NULL, then the port 
// doesn't have a device plugged in and has_device should also be false. 
typedef struct brx_port {
	struct brx_device * device_ptr; 
	size_t port_number; 
	bool has_device;
} brx_port; 

// Metadata about a device (tap0, etc), this is the MAC Address, IP Address, Name, MTU, State, etc...
typedef struct brx_device_info {
	char mac_address [MAC_ADDR_SIZE];
	char * interface_name; 	
	unsigned char ip_address [IP_ADDR_SIZE]; 
	size_t mtu; 
	// TODO: Should also store stuff like mode of the device
} brx_device_info;


// A device is a linux interface with metadata, a tap file descriptor & reference counting for proper 
// handling. Upon initializing a tap, the device will have a reference count of 1, and when added to 
// a port, a reference count of 2. Otherwise if not connected or initialized (post tap delete) the 
// the device will be freed, since it will have a reference count of 0

typedef struct brx_device {
	struct brx_device_info info;
	int tap_fd;
	size_t ref_count; 
	// TODO: Think of more stuff to add	
} brx_device; 


// XXX: The brx_bridge.bridge_dev is not to be used like a bridge, its really just so the "brx bridge" shows 
// when running ip link. The actual state is managed in userspace so there is nothing in kernel for bridging
// the kernel knows "br0"/"brx" as a tap, when it should be a bridge. 

// If I used the kernel's bridge, I don't get to reinvent the wheel ... 
typedef struct brx_bridge  {
	brx_device * bridge_dev; 
	struct brx_forwarding_table * table_ptr; 

	struct brx_port * ports; 
	size_t port_count; 

	struct brx_device * devices;
	size_t device_count; 
} brx_bridge; 



// Types of messages ... 
enum message_type {
	CREATE,
	DELETE,
	SHOW,
	SUCCESS,
	ERROR
}

// XXX: Passed across the UDS and is used for setting up control plane stuff ... 
typedef struct brx_control_message {
	size_t length;
	message_type type;
} brx_control_message; 


brx_device * create_device (char * name);
void free_device (brx_device * dev); 

brx_device_info * get_device_metadata (brx_device * dev); 
void set_device_metadata (brx_device * dev); 


brx_device * get_devices (brx_bridge * bridge);
brx_device * get_device_by_name (brx_device ** devices, const char * name);
// brx_device * get_device_by_name (char * name, brx_bridge * bridge);

#if 0
brx_forwarding_table * get_forwarding_table (void); 
brx_forwarding_table * add_route (brx_forwarding_table * table); 

brx_port * get_ports   (brx_bridge *bridge) 

void connect_device (brx_port *port, brx_device *dev, brx_bridge * bridge) 
void remove_device (brx_port *port, brx_device *device)

bool has_device (brx_port * port);
size_t get_port_number (brx_port * port);
brx_device * get_device_on_port (size_t port_index);
#endif 
