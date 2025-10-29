# Architecture Overview

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                         CLIENTS                              │
│  (Browser, curl, Postman, Mobile Apps, Other Servers)       │
└────────────────────────┬────────────────────────────────────┘
                         │ HTTP Requests
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    TCP/IP Stack (OS)                         │
│              (Socket Layer - Port 8080)                      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   HTTP SERVER (main.c)                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  SERVER SOCKET (server.c)                             │  │
│  │  • socket()  - Create socket                          │  │
│  │  • bind()    - Bind to 0.0.0.0:8080                   │  │
│  │  • listen()  - Start listening                        │  │
│  │  • accept()  - Accept connections (blocks)            │  │
│  └───────────────────┬───────────────────────────────────┘  │
│                      │                                       │
│                      ▼                                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  HTTP PARSER (http_handler.c)                         │  │
│  │  • recv() - Read request bytes                        │  │
│  │  • Parse HTTP method (GET/POST)                       │  │
│  │  • Parse path (/api/users)                            │  │
│  │  • Parse headers (Content-Type, etc.)                 │  │
│  │  • Parse body (JSON data)                             │  │
│  └───────────────────┬───────────────────────────────────┘  │
│                      │                                       │
│                      ▼                                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  ROUTER (routes.c)                                    │  │
│  │  • Match path to handler function                     │  │
│  │  • Route: / → handle_root()                           │  │
│  │  • Route: /api/users → handle_api_users()            │  │
│  │  • Route: /api/weather → handle_api_weather()        │  │
│  └───────────────────┬───────────────────────────────────┘  │
│                      │                                       │
│                      ▼                                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  HANDLERS (routes.c, api.c, api_client.c)            │  │
│  │  • Execute business logic                             │  │
│  │  • Build JSON response                                │  │
│  │  • Call external APIs if needed                       │  │
│  └───────────────────┬───────────────────────────────────┘  │
│                      │                                       │
│                      ▼                                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  RESPONSE BUILDER (http_handler.c)                    │  │
│  │  • Format HTTP response                               │  │
│  │  • Add status line (HTTP/1.1 200 OK)                  │  │
│  │  • Add headers (Content-Type, etc.)                   │  │
│  │  • Add body (HTML/JSON)                               │  │
│  │  • send() - Send response bytes                       │  │
│  └───────────────────┬───────────────────────────────────┘  │
│                      │                                       │
│                      ▼                                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  CLEANUP                                              │  │
│  │  • close() - Close client socket                      │  │
│  │  • free() - Free allocated memory                     │  │
│  │  • Thread exits                                       │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## 📂 File Organization

```
http-server/
│
├── src/
│   ├── main.c              ← Entry point (starts server)
│   │                          • Parse command line args
│   │                          • Initialize server
│   │                          • Main loop
│   │
│   ├── server.c            ← Socket management
│   │                          • create_server_socket()
│   │                          • bind_server_socket()
│   │                          • listen_for_connections()
│   │                          • accept_connection()
│   │                          • handle_client() [threaded]
│   │
│   ├── http_handler.c      ← HTTP protocol
│   │                          • parse_http_request()
│   │                          • build_http_response()
│   │                          • send_response()
│   │
│   ├── routes.c            ← Routing logic
│   │                          • route_request()
│   │                          • handle_root()
│   │                          • handle_info()
│   │                          • handle_glossary()
│   │                          • handle_post_data()
│   │
│   ├── api.c               ← JSON API endpoints
│   │                          • handle_api_health()
│   │                          • handle_api_users_get()
│   │                          • handle_api_users_post()
│   │                          • handle_api_login()
│   │                          • handle_api_calculate()
│   │                          • JSONBuilder (dynamic JSON)
│   │                          • JSONParser (parse JSON)
│   │
│   ├── api_client.c        ← External API integration
│   │                          • http_get() [HTTP client]
│   │                          • handle_api_weather()
│   │                          • handle_api_exchange()
│   │                          • handle_api_quote()
│   │                          • json_extract_string()
│   │
│   ├── glossary.c          ← /glossary endpoint
│   │                          • C function reference
│   │
│   ├── how_it_works.c      ← /how-it-works endpoint
│   │                          • Request/response explanation
│   │
│   └── utils.c             ← Helper functions
│                              • String utilities
│                              • Error handling
│
├── include/
│   └── http_server.h       ← All function declarations
│                              • Structs (HttpRequest, HttpResponse)
│                              • Function prototypes
│                              • Constants
│
├── build/                  ← Compiled object files (.o)
├── bin/                    ← Final executable
└── Makefile               ← Build configuration
```

## 🔄 Request Flow (Detailed)

### Step-by-Step Process

```
1. SERVER STARTUP
   main.c:main()
   └─> server.c:create_server_socket()
       └─> socket(AF_INET, SOCK_STREAM, 0)
       └─> setsockopt() [SO_REUSEADDR]
   └─> server.c:bind_server_socket()
       └─> bind(socket, 0.0.0.0:8080)
   └─> server.c:listen_for_connections()
       └─> listen(socket, BACKLOG)
   └─> Print "Server running on http://localhost:8080"

2. WAITING FOR CLIENTS
   main.c:main() [infinite loop]
   └─> server.c:accept_connection()
       └─> accept() ← BLOCKS HERE until client connects

3. CLIENT CONNECTS
   Browser sends: GET /api/users HTTP/1.1
   
   accept() returns → client_fd
   
   IF THREADED MODE:
   └─> pthread_create()
       └─> server.c:handle_client(client_fd) [runs in new thread]
   
   IF SEQUENTIAL MODE:
   └─> server.c:handle_client(client_fd) [blocks main thread]

4. READ REQUEST
   server.c:handle_client()
   └─> http_handler.c:parse_http_request()
       └─> recv(client_fd, buffer, size) [read bytes from socket]
       └─> Parse: "GET /api/users HTTP/1.1\r\n..."
       └─> Extract:
           • method = "GET"
           • path = "/api/users"
           • headers = [Host, User-Agent, ...]
           • body = "" (empty for GET)
       └─> Store in HttpRequest struct

5. ROUTE REQUEST
   server.c:handle_client()
   └─> routes.c:route_request(request, response)
       └─> Check request.method
           └─> IF GET:
               └─> Check request.path
                   └─> IF "/api/users":
                       └─> api.c:handle_api_users_get()
                   └─> IF "/api/weather":
                       └─> api_client.c:handle_api_weather()
                   └─> ELSE:
                       └─> handle_not_found()

6. EXECUTE HANDLER
   Example: api.c:handle_api_users_get()
   └─> Create JSONBuilder
   └─> Build JSON response:
       {
         "success": true,
         "data": [...users...],
         "count": 3
       }
   └─> Set response:
       • status_code = 200
       • content_type = "application/json"
       • body = JSON string
       • body_length = strlen(body)

7. BUILD HTTP RESPONSE
   server.c:handle_client()
   └─> http_handler.c:build_http_response()
       └─> Format:
           HTTP/1.1 200 OK\r\n
           Content-Type: application/json\r\n
           Content-Length: 234\r\n
           Connection: close\r\n
           \r\n
           {"success":true,...}
       └─> Store in response_buffer

8. SEND RESPONSE
   server.c:handle_client()
   └─> http_handler.c:send_response()
       └─> send(client_fd, response_buffer, length)
       └─> Bytes travel over TCP to client

9. CLEANUP
   server.c:handle_client()
   └─> close(client_fd)
   └─> free(request.path)
   └─> free(response.body)
   └─> IF THREADED: thread exits
   └─> IF SEQUENTIAL: loop back to accept()

10. SERVER CONTINUES
    main.c:main() [infinite loop continues]
    └─> Back to accept() waiting for next client
```

## 🧵 Threading Model

### Multi-threaded Mode (Default)

```
Main Thread:
┌──────────────────┐
│  while(1) {      │
│    client_fd =   │
│      accept()    │  ← Waits for client
│                  │
│    spawn_thread()│  ← Create new thread
│  }               │
└──────────────────┘
         │
         │ For each client:
         ▼
┌──────────────────┐
│  Worker Thread 1 │  ← Client 1
│  • Parse request │
│  • Route         │
│  • Execute       │
│  • Send response │
│  • Cleanup       │
│  • Exit          │
└──────────────────┘

┌──────────────────┐
│  Worker Thread 2 │  ← Client 2 (concurrent!)
│  • Parse request │
│  • Route         │
│  • Execute       │
│  • Send response │
│  • Cleanup       │
│  • Exit          │
└──────────────────┘
```

**Advantages:**
- Multiple clients handled simultaneously
- Non-blocking (main thread always ready for new clients)
- Better performance under load

**Thread lifecycle:**
```c
pthread_t thread;
pthread_create(&thread, NULL, handle_client, &client_fd);
pthread_detach(thread);  // Auto-cleanup when thread exits
```

### Sequential Mode

```
Main Thread:
┌──────────────────┐
│  while(1) {      │
│    client_fd =   │
│      accept()    │  ← Waits for client
│                  │
│    handle_client()│  ← Blocks until done
│  }               │
└──────────────────┘

Client 1 → Process → Send → Done
                             ↓
              Client 2 → Process → Send → Done
                                          ↓
                       Client 3 → Process → ...
```

**Disadvantages:**
- Only one client at a time
- Other clients must wait
- Poor performance under load

**Use case:** Debugging, education, simple scenarios

## 🔌 Socket States

```
Server Socket Lifecycle:
──────────────────────────

1. CREATED
   socket() returns fd
   State: UNBOUND
   
2. BOUND
   bind(fd, 0.0.0.0:8080)
   State: BOUND, not listening
   
3. LISTENING
   listen(fd, backlog)
   State: PASSIVE (ready to accept)
   
4. ACCEPTING
   accept() → blocks
   State: Waiting for client
   
5. CONNECTED
   accept() returns client_fd
   State: New socket for client
   
6. COMMUNICATING
   recv() / send()
   State: Data exchange
   
7. CLOSED
   close(client_fd)
   State: Connection terminated
   
8. LOOP
   Back to step 4 (accept)
```

## 📊 Data Structures

### HttpRequest
```c
typedef struct {
    HttpMethod method;      // GET, POST, PUT, DELETE
    char path[256];         // "/api/users"
    char headers[2048];     // "Host: localhost\r\n..."
    char body[4096];        // Request body (POST data)
    size_t body_length;     // Length of body
} HttpRequest;
```

### HttpResponse
```c
typedef struct {
    int status_code;           // 200, 404, 500, etc.
    char content_type[64];     // "application/json"
    char *body;                // Response body (dynamically allocated)
    size_t body_length;        // Length of body
} HttpResponse;
```

### JSONBuilder
```c
typedef struct {
    char *buffer;    // Dynamic buffer
    size_t size;     // Current size
    size_t capacity; // Total capacity (grows as needed)
} JSONBuilder;
```

## 🔐 Memory Management

```
Heap Allocations (must be freed):
─────────────────────────────────

1. HttpResponse.body
   • Allocated by handlers
   • Freed after send()
   
2. JSONBuilder.buffer
   • Allocated on create
   • Auto-freed on finalize
   
3. External API responses
   • Allocated by http_get()
   • Freed by http_response_free()
   
4. Parsed JSON strings
   • Returned by json_extract_string()
   • Must be freed by caller

Stack Allocations (auto-freed):
────────────────────────────────

1. HttpRequest struct
   • Created on stack
   • Auto-freed when function returns
   
2. Temporary buffers
   • char buffer[1024]
   • Auto-freed on scope exit
```

## 🎯 Design Decisions

### Why C?
- Shows raw networking without abstractions
- Teaches memory management
- Demonstrates low-level HTTP handling
- Foundation for understanding frameworks

### Why Threads?
- Handles multiple clients concurrently
- Simpler than async I/O for learning
- More intuitive than event loops
- Shows OS-level concurrency

### Why Custom JSON?
- Shows how parsing works
- No external dependencies
- Educational value
- Production apps should use libraries

### Why Port 8080?
- Doesn't require root (ports <1024 do)
- Common for HTTP servers
- Easy to remember
- Doesn't conflict with system services

## 🚀 Performance Characteristics

### Throughput
- Single-threaded: ~1000 req/s
- Multi-threaded: ~5000 req/s (depends on cores)

### Latency
- Local: <1ms
- With external API: 50-500ms (network dependent)

### Memory
- Base: ~1MB
- Per connection: ~10KB
- Per thread: ~8MB (stack size)

### Scalability
- Limited by: Thread overhead
- Max threads: ~1000-10000 (OS dependent)
- Better scaling: Use epoll/kqueue (future improvement)

## 📈 Future Architecture

### Potential Improvements

1. **Event-driven** (epoll/kqueue)
   - Single thread, multiple clients
   - Better scalability
   - More complex code

2. **Thread pool**
   - Pre-spawn worker threads
   - Reuse threads
   - Avoid thread creation overhead

3. **Connection pooling**
   - Keep-alive connections
   - Reuse TCP connections
   - Reduce handshake overhead

4. **Async I/O**
   - Non-blocking sockets
   - Event loop
   - Higher throughput

---

**This architecture is the foundation of every web server!**

<!-- 

between SERVER SOCKET & HTTP PARSER
│  ┌───────────────────────────────────────────────────────┐  │
│  │  CONNECTION HANDLER (Multi-threaded)                  │  │
│  │  • Spawn thread for each client                       │  │
│  │  • Each client gets independent thread                │  │
│  └───────────────────┬───────────────────────────────────┘  │
│                      │                                       │
│                      ▼                                       │ -->