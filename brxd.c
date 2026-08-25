#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <syslog.h>
#include <sys/un.h>

#include "utils.h"

#define SOCKET_PATH "/tmp/brx-control-uds.sock"
#define MAX_CONNS 32

volatile sig_atomic_t keep_running = 1;

// Signal handler for clean termination
void handle_signal (int sig) {
    keep_running = 0;
}


int main (int argc, char * argv[]) {
        
    // Register signal handlers

    signal (SIGTERM, handle_signal);
    signal (SIGINT, handle_signal);

    openlog ("brxd", LOG_PID, LOG_DAEMON);

    time_t current_time = time(NULL);
    syslog (LOG_INFO, "Starting the brxd daemon; Current Time: %s", ctime(&current_time));


    // Creating the UDS, Unix Domain Socket
    int server_fd = socket (AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        current_time = time(NULL);
        syslog (LOG_ERR, "Failed to create the server's socket; Current Time: %s", ctime(&current_time)); 
        return 1; 
    }


    struct sockaddr_un address;
    memset (&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy (address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    // Unlink the path if it already exists to avoid "Address already in use"
    unlink (SOCKET_PATH);


    if (bind (server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        current_time = time(NULL);
        syslog (LOG_ERR, "Failed to bind socket; Current Time: %s", ctime(&current_time)); 
        close (server_fd);
        return 1; 
    }


    // Up to 32 incoming connections ...
    if (listen(server_fd, MAX_CONNS) < 0 ) {
        current_time = time(NULL);
        syslog (LOG_ERR, "Failed to listen on socket; Current Time: %s", ctime(&current_time)); 
        close (server_fd);
    }

    current_time = time(NULL);
    syslog (LOG_INFO, "Listening on %s; Current Time: %s", SOCKET_PATH, ctime(&current_time)); 

    while (keep_running) {
        // TODO: Implement message passing between client command line & daemon
    }


    // Cleanup 
    syslog (LOG_INFO, "Shutting down brx daemon; Bye!");
    close (server_fd);
    unlink (SOCKET_PATH);
    closelog (); 
    return 0;
}