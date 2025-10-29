#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 8080

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    
    // Parse command line arguments
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Error: Invalid port number. Must be between 1 and 65535.\n");
            return 1;
        }
    }
    
    // Start the server
    int result = start_http_server(port);
    
    return result == 0 ? 0 : 1;
}