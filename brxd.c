#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifdef HAVE_SYSTEMD
#include <systemd/sd-daemon.h>
#endif 

#include "utils.h"

#define SOCKET_PATH "/tmp/brx-"

volatile sig_atomic_t keep_running = 1;

// Signal handler for clean termination
void handle_signal (int sig) {
    keep_running = 0;
}


int main (int argc, char * argv[]) {
        
    // Register signal handlers

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    printf("Starting brxcd ... \n");

    int socket = socket (AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un address;


    // 2. Clear and configure the address structure
    memset (&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy (address.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // 3. Unlink the path if it already exists to avoid "Address already in use"
    unlink (SOCKET_PATH);

    #ifdef HAVE_SYSTEMD

        while (1) {
            
            // WORK ...
        }

    #endif 

    


    // Cleanup 
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}