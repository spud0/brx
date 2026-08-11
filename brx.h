#pragma once

#include <stdlib.h>

#define MAC_ADDR_SIZE 6
#define IP_ADDR_SIZE 4

// The bridge will have a list of devices, list of ports, port_count &
// a forwarding table
typedef struct brx_bridge  {
	struct brx_port * port_ptr;
	struct brx_forwarding_table * table_ptr; 
	struct brx_device_info * device_info_ptr;
	size_t port_count; 
} brx_bridge; 

// A port can have a device "plugged in or not". A port must have
// a state of "is plugged into",  a port number and possibly other stuff. 
typedef struct brx_port {
	struct brx_device * device_ptr; 

	size_t port_number; 
	char has_device;
} brx_port; 

typedef struct brx_device_info {
	char mac_address [MAC_ADDR_SIZE];
	char * interface_name; 	
	char ip_address [IP_ADDR_SIZE]; 
	size_t mtu; 
} brx_device_info;


// A device is a linux interface. It'll be assigned a MAC address, 
// and a name.
typedef struct brx_device {
	struct brx_device_info info;
	// TODO: Think of more stuff to add	
} brx_device; 


// A forwarding table relates a list of hosts to a list of ports
// i.e, you can find Host A on port N, and Host B on port M

// XXX: Figure out 
typedef struct brx_forwarding_table {
	struct brx_port * port_ptr; 	
	char * mac_address_hosts_ptr [MAC_ADDR_SIZE]; 

	size_t entry_count; 
} brx_forwarding_table; 


#if 0
brx_forwarding_table * get_forwarding_table (void); 
brx_forwarding_table * add_route (brx_forwarding_table * table); 

brx_port * get_ports   (brx_bridge *bridge) 
brx_device * get_devices (brx_bridge *bridge) 

brx_device * create_device (const char mac_addr [], char * iface_name )
void connect_device (brx_port *port, brx_device *dev, brx_bridge * bridge) 
void remove_device (brx_port *port, brx_device *device)

bool has_device (brx_port port)
size_t get_port_number (brx_port port) 
brx_device * get_device_on_port (brx_port port)

#endif 


