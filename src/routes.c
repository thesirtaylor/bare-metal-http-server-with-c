#define _POSIX_C_SOURCE 200809L
#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void route_request(const HttpRequest *request, HttpResponse *response) {
    printf("[ROUTE] Routing %s %s\n",
           request->method == HTTP_GET ? "GET" : "POST",
           request->path);
    
    // GET routes
    if (request->method == HTTP_GET) {
        if (strcmp(request->path, "/") == 0) {
            handle_root(request, response);
        } else if (strcmp(request->path, "/info") == 0) {
            handle_info(request, response);
        } else if (strcmp(request->path, "/glossary") == 0) {
            handle_glossary(request, response);
        } else if (strcmp(request->path, "/how-it-works") == 0) {
            handle_how_it_works(request, response);
        } else if (strcmp(request->path, "/image") == 0) {
            handle_image(request, response);
        }
        // JSON API GET endpoints
        else if (strcmp(request->path, "/api/health") == 0) {
            handle_api_health(request, response);
        } else if (strcmp(request->path, "/api/users") == 0) {
            handle_api_users_get(request, response);
        } else if (strcmp(request->path, "/api/stats") == 0) {
            handle_api_stats(request, response);
        } else if (strcmp(request->path, "/api/time") == 0) {
            handle_api_time(request, response);
        }
        // External API calls
        else if (strcmp(request->path, "/api/weather") == 0) {
            handle_api_weather(request, response);
        } else if (strcmp(request->path, "/api/exchange") == 0) {
            handle_api_exchange_rates(request, response);
        } else if (strcmp(request->path, "/api/quote") == 0) {
            handle_api_quote(request, response);
        } else if (strcmp(request->path, "/api/proxy") == 0) {
            handle_api_proxy(request, response);
        } else {
            handle_not_found(request, response);
        }
    }
    // POST routes
    else if (request->method == HTTP_POST) {
        if (strcmp(request->path, "/echo") == 0) {
            handle_echo(request, response);
        } else if (strcmp(request->path, "/data") == 0) {
            handle_post_data(request, response);
        }
        // JSON API POST endpoints
        else if (strcmp(request->path, "/api/users") == 0) {
            handle_api_users_post(request, response);
        } else if (strcmp(request->path, "/api/login") == 0) {
            handle_api_login(request, response);
        } else if (strcmp(request->path, "/api/calculate") == 0) {
            handle_api_calculate(request, response);
        } else {
            handle_not_found(request, response);
        }
    }
    // Unknown methods
    else {
        handle_not_found(request, response);
    }
}

void handle_root(const HttpRequest *request, HttpResponse *response) {
    (void)request;  // Unused
    
    const char *html = 
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "    <title>C Web Server</title>\n"
        "    <style>\n"
        "        body {\n"
        "            font-family: 'Courier New', monospace;\n"
        "            max-width: 800px;\n"
        "            margin: 50px auto;\n"
        "            padding: 20px;\n"
        "            background: #0a0a0a;\n"
        "            color: #00ff00;\n"
        "        }\n"
        "        h1 { border-bottom: 2px solid #00ff00; padding-bottom: 10px; }\n"
        "        .endpoint {\n"
        "            background: #1a1a1a;\n"
        "            padding: 15px;\n"
        "            margin: 10px 0;\n"
        "            border-left: 3px solid #00ff00;\n"
        "        }\n"
        "        .method { color: #00aaff; font-weight: bold; }\n"
        "        a { color: #00ff00; }\n"
        "        code { background: #1a1a1a; padding: 2px 6px; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>🚀 Low-Level C Web Server</h1>\n"
        "    <p>A TCP/IP socket-based HTTP server written in C.</p>\n"
        "    \n"
        "    <h2>Available Endpoints:</h2>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method'>GET</span> <code>/</code><br>\n"
        "        This page - server home\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method'>GET</span> <code>/info</code><br>\n"
        "        Server information and networking details<br>\n"
        "        <a href='/info'>Visit /info</a>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method'>GET</span> <code>/glossary</code><br>\n"
        "        Complete C language reference with search<br>\n"
        "        <a href='/glossary'>Browse glossary</a>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method'>GET</span> <code>/how-it-works</code><br>\n"
        "        Deep dive into request/response cycle<br>\n"
        "        <a href='/how-it-works'>Learn how servers work</a>\n"
        "    </div>\n"
        "    \n"
        "    <h2 style='color:#00aaff;margin:30px 0 15px;'>🔌 JSON API Endpoints</h2>\n"
        "    <p style='margin-bottom:15px;'>RESTful API that accepts and returns JSON (like real backend servers!)</p>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/health</code><br>\n"
        "        Health check endpoint<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/health</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/users</code><br>\n"
        "        Get list of users<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/users</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method post'>POST</span> <code>/api/users</code><br>\n"
        "        Create new user (JSON body required)<br>\n"
        "        <code style='font-size:12px;'>curl -X POST http://localhost:8080/api/users -H \"Content-Type: application/json\" -d '{\"name\":\"John\",\"email\":\"john@test.com\"}'</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method post'>POST</span> <code>/api/login</code><br>\n"
        "        Login with username and password<br>\n"
        "        <code style='font-size:12px;'>curl -X POST http://localhost:8080/api/login -H \"Content-Type: application/json\" -d '{\"username\":\"admin\",\"password\":\"password\"}'</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method post'>POST</span> <code>/api/calculate</code><br>\n"
        "        Calculator API (add, subtract, multiply, divide)<br>\n"
        "        <code style='font-size:12px;'>curl -X POST http://localhost:8080/api/calculate -H \"Content-Type: application/json\" -d '{\"a\":10,\"b\":5,\"operation\":\"add\"}'</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/stats</code><br>\n"
        "        Server statistics<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/stats</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/time</code><br>\n"
        "        Current server time<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/time</code>\n"
        "    </div>\n"
        "    \n"
        "    <h2 style='color:#ffaa00;margin:30px 0 15px;'>🌍 External API Integration</h2>\n"
        "    <p style='margin-bottom:15px;'>Your server calls external APIs and returns their data!</p>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/weather</code><br>\n"
        "        Get London weather (calls wttr.in API)<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/weather</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/exchange</code><br>\n"
        "        Get USD exchange rates (calls exchangerate-api.com)<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/exchange</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/quote</code><br>\n"
        "        Get random quote (calls quotable.io API)<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/quote</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method get'>GET</span> <code>/api/proxy</code><br>\n"
        "        Proxy to GitHub API (pass-through example)<br>\n"
        "        <code style='font-size:12px;'>curl http://localhost:8080/api/proxy</code>\n"
        "    </div>\n"
        "    \n"
        "    <h2 style='color:#00aaff;margin:30px 0 15px;'>📝 Other Endpoints</h2>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method'>GET</span> <code>/image</code><br>\n"
        "        Serves a sample image (binary data transfer)<br>\n"
        "        <a href='/image'>View image</a>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method'>POST</span> <code>/echo</code><br>\n"
        "        Echoes back your request body<br>\n"
        "        Try: <code>curl -X POST -d 'Hello Server!' http://localhost:8080/echo</code>\n"
        "    </div>\n"
        "    \n"
        "    <div class='endpoint'>\n"
        "        <span class='method'>POST</span> <code>/data</code><br>\n"
        "        Process JSON or form data<br>\n"
        "        Try: <code>curl -X POST -H 'Content-Type: application/json' -d '{\"name\":\"test\"}' http://localhost:8080/data</code>\n"
        "    </div>\n"
        "    \n"
        "    <h2>Understanding the Network Stack:</h2>\n"
        "    <ul>\n"
        "        <li><strong>Application Layer:</strong> HTTP protocol (this server)</li>\n"
        "        <li><strong>Transport Layer:</strong> TCP sockets (reliable, ordered delivery)</li>\n"
        "        <li><strong>Network Layer:</strong> IP routing (how packets find the server)</li>\n"
        "        <li><strong>Link Layer:</strong> Ethernet/WiFi (actual 1s and 0s)</li>\n"
        "    </ul>\n"
        "</body>\n"
        "</html>";
    
    response->status_code = 200;
    strcpy(response->content_type, "text/html; charset=utf-8");
    response->body_length = strlen(html);
    response->body = strdup(html);
}

void handle_info(const HttpRequest *request, HttpResponse *response) {
    (void)request;  // Unused
    
    char html[4096];
    snprintf(html, sizeof(html),
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "    <title>Server Info</title>\n"
        "    <style>\n"
        "        body { font-family: monospace; max-width: 900px; margin: 50px auto; padding: 20px; background: #0a0a0a; color: #00ff00; }\n"
        "        h1 { border-bottom: 2px solid #00ff00; padding-bottom: 10px; }\n"
        "        .info-box { background: #1a1a1a; padding: 20px; margin: 15px 0; border-left: 4px solid #00ff00; }\n"
        "        .label { color: #00aaff; font-weight: bold; }\n"
        "        code { background: #2a2a2a; padding: 2px 6px; color: #ffaa00; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>🔍 Server Information</h1>\n"
        "    \n"
        "    <div class='info-box'>\n"
        "        <h2>How Your Request Got Here:</h2>\n"
        "        <p><span class='label'>1. DNS Resolution:</span><br>\n"
        "        Your browser asked a DNS server to convert the domain name to an IP address.</p>\n"
        "        \n"
        "        <p><span class='label'>2. TCP Connection:</span><br>\n"
        "        Your machine initiated a TCP 3-way handshake:<br>\n"
        "        - Your machine sent <code>SYN</code><br>\n"
        "        - Server replied <code>SYN-ACK</code><br>\n"
        "        - Your machine confirmed <code>ACK</code></p>\n"
        "        \n"
        "        <p><span class='label'>3. HTTP Request:</span><br>\n"
        "        Your browser sent an HTTP request as plain text over the TCP connection:<br>\n"
        "        <code>GET /info HTTP/1.1\\r\\nHost: ...\\r\\n\\r\\n</code></p>\n"
        "        \n"
        "        <p><span class='label'>4. Network Routing:</span><br>\n"
        "        Your packets traveled through multiple routers, each forwarding based on destination IP.<br>\n"
        "        Each router consulted its routing table to find the next hop.</p>\n"
        "        \n"
        "        <p><span class='label'>5. Socket Reception:</span><br>\n"
        "        This server's socket (bound to port 8080) received your packets.<br>\n"
        "        The OS reassembled TCP segments and delivered data to the server process.</p>\n"
        "    </div>\n"
        "    \n"
        "    <div class='info-box'>\n"
        "        <h2>Request Details:</h2>\n"
        "        <p><span class='label'>Method:</span> %s</p>\n"
        "        <p><span class='label'>Path:</span> %s</p>\n"
        "        <p><span class='label'>Your IP:</span> %s</p>\n"
        "    </div>\n"
        "    \n"
        "    <div class='info-box'>\n"
        "        <h2>How Response Gets Back:</h2>\n"
        "        <p>This HTML is converted to bytes, wrapped in HTTP headers, sent through the TCP socket,<br>\n"
        "        broken into IP packets, routed back to your machine, and rendered by your browser.</p>\n"
        "    </div>\n"
        "    \n"
        "    <p><a href='/'>← Back to home</a></p>\n"
        "</body>\n"
        "</html>",
        request->method == HTTP_GET ? "GET" : "POST",
        request->path,
        request->client_ip[0] ? request->client_ip : "unknown"
    );
    
    response->status_code = 200;
    strcpy(response->content_type, "text/html; charset=utf-8");
    response->body_length = strlen(html);
    response->body = strdup(html);
}

void handle_image(const HttpRequest *request, HttpResponse *response) {
    (void)request;  // Unused
    
    /* ============================================
       SERVING BINARY DATA (IMAGES)
       ============================================
       
       Images are binary data - sequences of bytes that represent pixels.
       
       For example, a PNG file starts with these bytes:
       0x89 0x50 0x4E 0x47 0x0D 0x0A 0x1A 0x0A
       (This is the PNG signature)
       
       When sending an image:
       1. Read the file as binary data (not text)
       2. Set Content-Type to image/png (or jpeg, gif, etc.)
       3. Set Content-Length to the file size in bytes
       4. Send the raw bytes through the socket
       
       The browser receives these bytes and knows (from Content-Type)
       to interpret them as an image, not text.
       
       At the network level, text and binary are the same - just bytes.
       The difference is how we interpret them!
       ============================================ */
    
    size_t image_size;
    unsigned char *image_data = read_image_file("sample.png", &image_size);
    
    if (image_data) {
        response->status_code = 200;
        strcpy(response->content_type, "image/png");
        response->body_length = image_size;
        response->body = (char *)image_data;
        
        printf("[IMAGE] Serving image: %zu bytes\n", image_size);
    } else {
        // If no image file, generate a simple SVG instead
        const char *svg = 
            "<svg width='400' height='300' xmlns='http://www.w3.org/2000/svg'>\n"
            "  <rect width='100%' height='100%' fill='#0a0a0a'/>\n"
            "  <text x='50%' y='50%' text-anchor='middle' fill='#00ff00' font-size='24' font-family='monospace'>\n"
            "    Sample Image from C Server\n"
            "  </text>\n"
            "  <text x='50%' y='60%' text-anchor='middle' fill='#00aaff' font-size='16' font-family='monospace'>\n"
            "    Binary data transmitted over TCP/IP\n"
            "  </text>\n"
            "</svg>";
        
        response->status_code = 200;
        strcpy(response->content_type, "image/svg+xml");
        response->body_length = strlen(svg);
        response->body = strdup(svg);
    }
}

void handle_echo(const HttpRequest *request, HttpResponse *response) {
    /* ============================================
       ECHO ENDPOINT - POST REQUEST
       ============================================
       
       This endpoint echoes back whatever data you send.
       
       When you POST data:
       1. Client sends: Headers + Body
       2. Body can be: JSON, form data, plain text, binary, etc.
       3. Content-Type header tells us what kind of data
       4. Content-Length tells us how many bytes
       
       The data arrives as a stream of bytes through the socket.
       ============================================ */
    
    if (request->body && request->body_length > 0) {
        char html[8192];
        snprintf(html, sizeof(html),
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>Echo Response</title>\n"
            "<style>body{font-family:monospace;max-width:800px;margin:50px auto;padding:20px;background:#0a0a0a;color:#00ff00;}\n"
            "h1{border-bottom:2px solid #00ff00;padding-bottom:10px;}.box{background:#1a1a1a;padding:20px;margin:15px 0;border-left:4px solid #00ff00;}</style>\n"
            "</head>\n"
            "<body>\n"
            "    <h1>📡 Echo Response</h1>\n"
            "    <div class='box'>\n"
            "        <h3>You sent:</h3>\n"
            "        <p><strong>Content-Type:</strong> %s</p>\n"
            "        <p><strong>Content-Length:</strong> %zu bytes</p>\n"
            "        <pre>%s</pre>\n"
            "    </div>\n"
            "    <p>Your data traveled through the network as TCP packets and arrived here!</p>\n"
            "    <p><a href='/'>← Back to home</a></p>\n"
            "</body>\n"
            "</html>",
            request->content_type[0] ? request->content_type : "not specified",
            request->body_length,
            request->body
        );
        
        response->status_code = 200;
        strcpy(response->content_type, "text/html; charset=utf-8");
        response->body_length = strlen(html);
        response->body = strdup(html);
    } else {
        const char *msg = "No body received in POST request";
        response->status_code = 200;
        strcpy(response->content_type, "text/plain");
        response->body_length = strlen(msg);
        response->body = strdup(msg);
    }
}

void handle_post_data(const HttpRequest *request, HttpResponse *response) {
    if (request->body && request->body_length > 0) {
        char html[8192];
        snprintf(html, sizeof(html),
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>Data Received</title>\n"
            "<style>body{font-family:monospace;max-width:800px;margin:50px auto;padding:20px;background:#0a0a0a;color:#00ff00;}\n"
            "h1{border-bottom:2px solid #00ff00;}.box{background:#1a1a1a;padding:20px;margin:15px 0;border-left:4px solid #00ff00;}</style>\n"
            "</head>\n"
            "<body>\n"
            "    <h1>✓ Data Processed</h1>\n"
            "    <div class='box'>\n"
            "        <h3>Received Data:</h3>\n"
            "        <pre>%s</pre>\n"
            "    </div>\n"
            "    <p><a href='/'>← Back to home</a></p>\n"
            "</body>\n"
            "</html>",
            request->body
        );
        
        response->status_code = 200;
        strcpy(response->content_type, "text/html; charset=utf-8");
        response->body_length = strlen(html);
        response->body = strdup(html);
    } else {
        handle_not_found(request, response);
    }
}

void handle_not_found(const HttpRequest *request, HttpResponse *response) {
    char html[2048];
    snprintf(html, sizeof(html),
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>404 Not Found</title>\n"
        "<style>body{font-family:monospace;max-width:800px;margin:50px auto;padding:20px;background:#0a0a0a;color:#ff0000;text-align:center;}\n"
        "h1{font-size:72px;margin:20px;}</style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>404</h1>\n"
        "    <h2>Not Found</h2>\n"
        "    <p>The path <code>%s</code> does not exist on this server.</p>\n"
        "    <p><a href='/' style='color:#00ff00;'>← Back to home</a></p>\n"
        "</body>\n"
        "</html>",
        request->path
    );
    
    response->status_code = 404;
    strcpy(response->content_type, "text/html; charset=utf-8");
    response->body_length = strlen(html);
    response->body = strdup(html);
}