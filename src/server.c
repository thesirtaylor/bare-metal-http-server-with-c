#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t server_running = 1;

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n[SERVER] Shutting down gracefully...\n");
        server_running = 0;
    }
}

int start_http_server(int port) {
    int server_fd;
    struct sockaddr_in server_addr;
    
    printf("\n========================================\n");
    printf("HTTP SERVER - Low Level Implementation\n");
    printf("========================================\n\n");
    
    // Set up signal handler for graceful shutdown
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    // Ignore SIGPIPE - handle broken pipe errors gracefully
    // SIGPIPE is sent when writing to a closed socket
    // Without this, the server crashes when a client disconnects early
    signal(SIGPIPE, SIG_IGN);
    
    /* ============================================
       STEP 1: CREATE A SOCKET
       ============================================
       socket() creates an endpoint for communication
       
       AF_INET = IPv4 Internet protocols
       SOCK_STREAM = TCP (connection-oriented, reliable)
       0 = Default protocol (TCP for SOCK_STREAM)
       
       This returns a file descriptor - in Unix, everything is a file!
       You can read/write to this socket like a file.
       ============================================ */
    printf("[SOCKET] Creating socket endpoint...\n");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[ERROR] Socket creation failed");
        return -1;
    }
    printf("[SOCKET] Socket created successfully (fd: %d)\n", server_fd);
    
    /* ============================================
       STEP 2: SET SOCKET OPTIONS
       ============================================
       SO_REUSEADDR allows reusing the address immediately
       Without this, you'd have to wait ~60 seconds after stopping
       the server before you could start it again on the same port
       ============================================ */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERROR] Setsockopt failed");
        close(server_fd);
        return -1;
    }
    printf("[SOCKET] Socket options configured\n");
    
    /* ============================================
       STEP 3: BIND SOCKET TO ADDRESS
       ============================================
       This associates the socket with a specific IP address and port
       
       struct sockaddr_in contains:
       - sin_family: Address family (AF_INET = IPv4)
       - sin_port: Port number (in network byte order - big endian)
       - sin_addr: IP address (INADDR_ANY = 0.0.0.0 = all interfaces)
       
       Network byte order vs Host byte order:
       - Different CPU architectures store multi-byte numbers differently
       - Network protocols use big-endian (most significant byte first)
       - htons() = "host to network short" converts to network byte order
       ============================================ */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all network interfaces
    server_addr.sin_port = htons(port);         // Convert port to network byte order
    
    printf("[BIND] Binding socket to 0.0.0.0:%d...\n", port);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Bind failed");
        close(server_fd);
        return -1;
    }
    printf("[BIND] Socket bound successfully\n");
    
    /* ============================================
       STEP 4: LISTEN FOR CONNECTIONS
       ============================================
       listen() marks the socket as passive - it will accept incoming connections
       
       The backlog parameter (10) is the maximum length of the queue
       of pending connections. When a client tries to connect:
       1. TCP handshake begins (SYN, SYN-ACK, ACK)
       2. Connection is queued
       3. Your accept() call retrieves it from the queue
       ============================================ */
    printf("[LISTEN] Starting to listen for connections (backlog: 10)...\n");
    if (listen(server_fd, 10) < 0) {
        perror("[ERROR] Listen failed");
        close(server_fd);
        return -1;
    }
    
    printf("\n✓ Server successfully started!\n");
    printf("✓ Listening on http://localhost:%d\n", port);
    printf("✓ Access from network: http://<your-ip>:%d\n", port);
    printf("\nEndpoints:\n");
    printf("  GET  /           - Home page\n");
    printf("  GET  /info       - Server information\n");
    printf("  GET  /image      - Serve an image\n");
    printf("  POST /echo       - Echo request body\n");
    printf("  POST /data       - Process data\n");
    printf("\nPress Ctrl+C to stop the server.\n");
    printf("========================================\n\n");
    
    /* ============================================
       MAIN SERVER LOOP
       ============================================
       This loop accepts and handles client connections
       ============================================ */
    while (server_running) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd;
        
        /* ============================================
           STEP 5: ACCEPT INCOMING CONNECTIONS
           ============================================
           accept() blocks until a client connects, then:
           1. Completes the TCP 3-way handshake
           2. Creates a NEW socket for this specific client
           3. Returns the client's address information
           
           Why a new socket?
           - server_fd stays listening for new connections
           - client_fd is for communicating with THIS client
           - This allows handling multiple clients
           
           The client_addr structure contains:
           - sin_addr: Client's IP address (e.g., 191.124.29.0)
           - sin_port: Client's source port (randomly assigned by OS)
           ============================================ */
        
        // Use select() with timeout to allow checking server_running flag
        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        int activity = select(server_fd + 1, &readfds, NULL, NULL, &tv);
        
        if (activity < 0 && errno != EINTR) {
            perror("[ERROR] Select error");
            break;
        }
        
        if (activity == 0) {
            // Timeout - check if server should continue
            continue;
        }
        
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        
        if (client_fd < 0) {
            if (errno != EINTR) {
                perror("[ERROR] Accept failed");
            }
            continue;
        }
        
        // Convert client IP address to human-readable format
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        
        printf("[CONNECTION] New connection from %s:%d\n", 
               client_ip, ntohs(client_addr.sin_port));
        
        /* ============================================
           STEP 6: HANDLE CLIENT REQUEST
           ============================================
           Now we can read the HTTP request from the client
           and send back a response
           ============================================ */
        handle_client_connection(client_fd);
        
        /* ============================================
           STEP 7: CLOSE CLIENT CONNECTION
           ============================================
           close() terminates the connection:
           1. Sends TCP FIN packet to client
           2. Releases the socket resources
           3. Client receives FIN and closes their end
           
           This completes the TCP connection lifecycle:
           SYN -> SYN-ACK -> ACK -> DATA -> FIN -> FIN-ACK
           ============================================ */
        close(client_fd);
        printf("[CONNECTION] Connection closed\n\n");
    }
    
    close(server_fd);
    printf("[SERVER] Server stopped.\n");
    
    return 0;
}