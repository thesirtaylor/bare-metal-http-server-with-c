---

## 🎓 What You'll Learn

### Network Programming
- Socket creation and configuration
- TCP connection management
- DNS resolution
- Port binding and listening
- Accepting connections
- Reading and writing data over sockets

### HTTP Protocol
- Request format and parsing
- Response format and building
- Headers and content types
- Status codes
- HTTP methods (GET, POST)

### System Programming
- POSIX threads for concurrency
- Memory management (malloc, free)
- String manipulation in C
- Error handling
- File descriptors

### Web Development Fundamentals
- Request routing
- JSON parsing and generation
- API design patterns
- Error responses
- Status code semantics

---

## 🔍 Learning Path

### For Beginners

1. Start the server and visit `http://localhost:8080/`
2. Read `/info` to understand networking basics
3. Read `/how-it-works` to understand the request/response cycle
4. Look at `src/server.c` to see socket code
5. Look at `src/routes.c` to see routing

### For Intermediate

1. Study `src/http_handler.c` to understand HTTP parsing
2. Look at `src/api.c` to see JSON API implementation
3. Read about multi-threading in `MULTIPLE_CLIENTS.md`
4. Try modifying handlers to add new endpoints
5. Experiment with the JSON builder

### For Advanced

1. Study `src/api_client.c` to see HTTP client implementation
2. Implement your own JSON parser
3. Add support for HTTP/2
4. Implement WebSockets
5. Add TLS/SSL support
6. Implement connection pooling

---