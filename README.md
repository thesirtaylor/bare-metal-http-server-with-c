# HTTP Server in C - From Scratch

A low-level HTTP server implementation in C that demonstrates how web servers actually work under the hood. This project strips away all the frameworks and abstractions to show you the fundamental mechanics that power every web application.

## 🎯 Why This Project?

Ever wondered what's really happening when you use Express, Flask, or Django? This project answers that question by building a complete HTTP server from scratch using only C and POSIX sockets.

**What you'll understand:**
- How TCP/IP sockets actually work
- What HTTP requests and responses really are (just text!)
- How servers parse incoming requests
- How routing works at the fundamental level
- The request/response cycle that powers ALL web frameworks
<!-- - How to handle concurrent connections -->
- How to build and parse JSON without libraries
- How to call external APIs from your server

**This is the foundation that every web framework builds on top of.**

---

## ✨ Features

- ✅ **Pure C implementation** - No external dependencies except standard POSIX libraries
- ✅ **Complete HTTP/1.1** - Proper request parsing and response formatting
- ✅ **Multiple endpoints** - HTML pages, JSON APIs, static files
<!-- - ✅ **Multi-threading** - Handle multiple clients concurrently -->
- ✅ **JSON APIs** - RESTful endpoints with proper error handling
- ✅ **External API integration** - Call and parse responses from other APIs
- ✅ **Well-documented** - Extensive comments and documentation
- ✅ **Educational** - Interactive web pages that teach networking concepts

---

## 🚀 Quick Start

### Prerequisites

- GCC compiler
- Make
- Linux/Unix environment (or WSL on Windows)

### Build & Run

```bash
# Clone the repository
git clone <your-repo-url>
cd webserver

# Build the server
make

# Run the server
make run
```

The server will start on `http://localhost:8080`

---


---

## 🏗️ Project Structure

```
webserver/
├── src/
│   ├── main.c              # Entry point
│   ├── server.c            # Socket management (listen, accept)
│   ├── http_handler.c      # HTTP parsing and response building
│   ├── routes.c            # Request routing logic
│   ├── utils.c             # Utility functions
│   ├── glossary.c          # /glossary endpoint handler
│   ├── how_it_works.c      # /how-it-works endpoint handler
│   ├── api.c               # JSON API endpoints
│   └── api_client.c        # External API integration
├── include/
│   └── http_server.h       # Header with all declarations
├── build/                  # Compiled object files
├── bin/                    # Final executable
├── Makefile               # Build configuration
└── docs/                  # Extensive documentation
```

---

## 🧠 How It Works

### The 10-Step Request/Response Cycle

Every server (including Express, Flask, Django) follows this pattern:

```
1. socket()     - Create socket endpoint
2. bind()       - Attach to IP:Port (e.g., 0.0.0.0:8080)
3. listen()     - Mark socket as passive (ready to accept)
4. accept()     - Wait for client (BLOCKS here)
5. recv()       - Read HTTP request bytes
6. parse()      - Extract method, path, headers, body
7. route()      - Match path to handler function
8. execute()    - Run your business logic
9. send()       - Send HTTP response bytes
10. close()     - Disconnect client, loop to step 4
```

**This is what frameworks hide from you!**

### HTTP Protocol

The server implements core HTTP/1.1 features:

- Request parsing (method, path, headers, body)
- Response formatting (status line, headers, body)
- Content-Type handling (text/html, application/json, image/png)
- Status codes (200, 201, 400, 401, 404, 502)
- Proper header formatting (`\r\n` line endings)

---

## 📖 Documentation

The project includes extensive documentation:

- **[ARCHITECTURE.md](./src/readme/ARCHITECTURE.md)** - System Architecture
- **[CONTRIBUTING.md](./src/readme/CONTRIBUTING.md)** - Instruction for contrributors
- **[LEARNING.md](./src/readme/LEARNING.md)** -  Things you'll learn
- **[ENDPOINTS.md](./src/readme/ENDPOINTS.md)** - API documentation
- **[EXTERNALAPI.md](./src/readme/EXTERNALAPI.md)** - Calling external APIs


---

## 🧪 Testing

### Manual Testing

```bash
# Start the server
./bin/webserver

# In another terminal, test endpoints
curl http://localhost:8080/
curl http://localhost:8080/api/health
curl http://localhost:8080/api/users

# POST request
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Test User","email":"test@example.com"}'
```

### Load Testing

```bash
# Install Apache Bench
sudo apt-get install apache2-utils

# Test with 1000 requests, 10 concurrent
ab -n 1000 -c 10 http://localhost:8080/api/health
```

## 🚧 Limitations & Future Improvements

### Current Limitations
- Cannot handle multiple client connections
- No HTTPS/TLS support (plain HTTP only)
- Simple JSON parser (no nested arrays/objects parsing)
- No request body size limits
- No rate limiting
- No logging to files
- No configuration file support

### Potential Improvements

- [ ] Add HTTPS/SSL using OpenSSL
- [ ] Integrate proper JSON library (cJSON)
- [ ] Add configuration file support
- [ ] Implement connection pooling
- [ ] Add request/response logging
- [ ] Implement HTTP/2 support
- [ ] Add WebSocket support
- [ ] Add rate limiting
- [ ] Implement caching layer
- [ ] Add authentication middleware

## 📝 License

This project is open source and available under the MIT License.

---

## 🙏 Acknowledgments

This project was built to demystify web servers and show how they really work under the hood. It's inspired by the realization that understanding the fundamentals makes you a better developer, regardless of which framework you use.

**Key Insight:** Every web framework (Express, Flask, Django, Rails) does exactly what this server does. They just hide the complexity. Now you know what they're hiding.

---

## 📬 Questions?

If you have questions or want to discuss how servers work, feel free to open an issue!

---

## 🎯 The Bottom Line

**After working through this project, you'll understand:**

- What happens when you type a URL in your browser
- How frameworks like Express and Flask work under the hood
- Why servers sometimes hang or crash
- How to debug network issues
- How to build APIs from scratch
- The fundamentals that power the entire web

**You'll never look at `app.get()` the same way again.** 🚀

---

<p align="center">
  <strong>Built with ❤️ to teach the fundamentals of web servers</strong>
</p># bare-metal-http-server-with-c
