#define _POSIX_C_SOURCE 200809L
#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 65536 // 64KB buffer for requests

/* ============================================
   HTTP PROTOCOL EXPLANATION
   ============================================

   HTTP is a TEXT-based protocol that runs over TCP.
   An HTTP request looks like this in raw bytes:

   GET /index.html HTTP/1.1\r\n
   Host: example.com\r\n
   User-Agent: Mozilla/5.0\r\n
   Accept: text/html\r\n
   \r\n

   Breaking it down:
   1. Request Line: METHOD PATH HTTP_VERSION
   2. Headers: Key: Value pairs separated by \r\n
   3. Empty line: \r\n\r\n marks end of headers
   4. Body: Optional data (for POST, PUT, etc.)

   The \r\n is CRLF (Carriage Return, Line Feed)
   - \r = 0x0D = 13 in decimal
   - \n = 0x0A = 10 in decimal

   When you send an image or JSON:
   - Text data is sent as-is (UTF-8 encoded bytes)
   - Binary data (images) is sent as raw bytes
   - Content-Length header tells how many bytes to expect
   - Content-Type header tells what kind of data it is

   At the network level, ALL of this is just bytes:
   - "GET" becomes: 0x47 0x45 0x54
   - An image is just bytes: 0xFF 0xD8 0xFF 0xE0... (JPEG header)

   The machine doesn't "understand" text or images - it just
   moves bytes. The HTTP protocol gives meaning to these bytes.

   ============================================ */

void send_http_response(int client_fd, const HttpResponse *response)
{
    /* ============================================
       BUILD HTTP RESPONSE
       ============================================
       Response format:
       HTTP/1.1 STATUS_CODE STATUS_MESSAGE\r\n
       Content-Type: type\r\n
       Content-Length: length\r\n
       \r\n
       BODY
       ============================================ */

    // Status line
    const char *status_message;
    switch (response->status_code)
    {
    case 200:
        status_message = "OK";
        break;
    case 404:
        status_message = "Not Found";
        break;
    case 500:
        status_message = "Internal Server Error";
        break;
    default:
        status_message = "Unknown";
        break;
    }

    // Build headers
    char headers[1024];
    int header_len = snprintf(headers, sizeof(headers),
                              "HTTP/1.1 %d %s\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %zu\r\n"
                              "Connection: close\r\n"
                              "\r\n",
                              response->status_code,
                              status_message,
                              response->content_type,
                              response->body_length);

    printf("[RESPONSE] Sending %d %s\n", response->status_code, status_message);
    printf("[RESPONSE] Content-Type: %s\n", response->content_type);
    printf("[RESPONSE] Content-Length: %zu bytes\n", response->body_length);

    /* ============================================
       SEND DATA THROUGH SOCKET
       ============================================
       send() writes data to the TCP socket

       What actually happens:
       1. Your data is copied to kernel buffer
       2. OS breaks it into TCP segments (MSS ~1460 bytes)
       3. Each segment gets TCP header (ports, sequence numbers)
       4. Segments wrapped in IP packets (source/dest IP)
       5. Packets sent over network as Ethernet frames (1s and 0s!)
       6. Client OS reassembles packets back into your data

       For an image:
       - Headers sent as text: "HTTP/1.1 200 OK\r\n..."
       - Then raw image bytes: 0xFF 0xD8 0xFF 0xE0...
       - Client browser interprets bytes based on Content-Type

       The network doesn't care if it's text, JSON, or an image.
       It just moves bytes from point A to point B.
       ============================================ */

    // Send headers
    ssize_t sent = send(client_fd, headers, header_len, 0);
    if (sent < 0)
    {
        perror("[ERROR] Failed to send headers");
        return;
    }

    // Send body (if any)
    if (response->body && response->body_length > 0)
    {
        sent = send(client_fd, response->body, response->body_length, 0);
        if (sent < 0)
        {
            perror("[ERROR] Failed to send body");
            return;
        }
        printf("[RESPONSE] Sent %zd bytes total\n", (ssize_t)header_len + sent);
    }
    else
    {
        printf("[RESPONSE] Sent %zd bytes (headers only)\n", sent);
    }
}

void handle_client_connection(int client_fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    /* ============================================
      READ DATA FROM SOCKET
      ============================================
      recv() reads data from the TCP socket

      What's actually happening:
      1. Client sends HTTP request as TCP packets
      2. OS receives packets, reassembles them in order
      3. Data is buffered in kernel space
      4. recv() copies data from kernel buffer to your buffer

      The data is just raw bytes. It could be:
      - ASCII text: "GET / HTTP/1.1"
      - UTF-8 text: "Hello 世界" (multibyte characters)
      - Binary: 0xFF 0xD8 0xFF 0xE0... (JPEG image)

      recv() doesn't care what the data means - it just copies bytes.
      ============================================ */

    printf("[REQUEST] Reading data from socket...\n");
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received < 0)
    {
        perror("[ERROR] Failed to receive data");
        return;
    }

    if (bytes_received == 0)
    {
        printf("[REQUEST] Client closed connection\n");
        return;
    }

    buffer[bytes_received] = '\0';

    printf("[REQUEST] Received %zd bytes\n", bytes_received);
    printf("[REQUEST] Raw request:\n%s\n", buffer);

    /* ============================================
       PARSE HTTP REQUEST
       ============================================
       Now we interpret the raw bytes as HTTP protocol
       ============================================ */
    HttpRequest request;
    memset(&request, 0, sizeof(request));
    parse_http_request(buffer, &request);

    /* ============================================
        ROUTE REQUEST TO HANDLER
        ============================================
        Based on the HTTP method and path, we call
        the appropriate handler function
        ============================================ */
    HttpResponse response;
    memset(&response, 0, sizeof(response));
    route_request(&request, &response);

    /* ============================================
    SEND HTTP RESPONSE
    ============================================
    Convert response structure to HTTP format
    and send bytes back through the socket
    ============================================ */
    send_http_response(client_fd, &response);

    // Clean up
    if (request.body)
    {
        free(request.body);
    }
    if (response.body)
    {
        free(response.body);
    }
}

void parse_http_request(const char *raw_request, HttpRequest *request)
{
    char *request_copy = strdup(raw_request);
    char *line = request_copy;
    char *next_line;

    /* ============================================
       PARSE REQUEST LINE
       ============================================
       First line format: METHOD PATH HTTP_VERSION
       Example: "GET /index.html HTTP/1.1"
       ============================================ */

    // Find end of first line
    next_line = strstr(line, "\r\n");
    if (next_line)
    {
        *next_line = '\0';
        next_line += 2; // Skip \r\n
    }

    // Parse method
    if (strncmp(line, "GET", 3) == 0)
    {
        request->method = HTTP_GET;
    }
    else if (strncmp(line, "POST", 4) == 0)
    {
        request->method = HTTP_POST;
    }
    else
    {
        request->method = HTTP_UNKNOWN;
    }

    // Parse path
    char *path_start = strchr(line, ' ');
    if (path_start)
    {
        path_start++; // Skip space
        char *path_end = strchr(path_start, ' ');
        if (path_end)
        {
            size_t path_len = path_end - path_start;
            if (path_len < sizeof(request->path))
            {
                strncpy(request->path, path_start, path_len);
                request->path[path_len] = '\0';
            }
        }
    }

    printf("[PARSE] Method: %s, Path: %s\n",
           request->method == HTTP_GET ? "GET" : request->method == HTTP_POST ? "POST"
                                                                              : "UNKNOWN",
           request->path);

    /* ============================================
       PARSE HEADERS
       ============================================
       Headers are key-value pairs: "Key: Value\r\n"
       ============================================ */

    line = next_line;
    size_t content_length = 0;

    while (line && *line != '\0')
    {
        next_line = strstr(line, "\r\n");
        if (next_line)
        {
            *next_line = '\0';
            next_line += 2;
        }

        // Empty line marks end of headers
        if (*line == '\0')
        {
            line = next_line;
            break;
        }

        // Parse Content-Type header
        if (strncasecmp(line, "Content-Type:", 13) == 0)
        {
            char *value = line + 13;
            while (*value == ' ')
                value++; // Skip spaces
            strncpy(request->content_type, value, sizeof(request->content_type) - 1);
        }

        // Parse Content-Length header
        if (strncasecmp(line, "Content-Length:", 15) == 0)
        {
            content_length = atoi(line + 15);
        }

        line = next_line;
    }

    /* ============================================
       PARSE BODY
       ============================================
       For POST requests, body comes after headers
       ============================================ */

    if (request->method == HTTP_POST && content_length > 0 && line)
    {
        request->body_length = content_length;
        request->body = malloc(content_length + 1);
        if (request->body)
        {
            // The body is already in our buffer after the headers
            size_t body_offset = line - request_copy;
            size_t available = strlen(raw_request) - body_offset;
            size_t to_copy = (available < content_length) ? available : content_length;

            memcpy(request->body, raw_request + body_offset, to_copy);
            request->body[to_copy] = '\0';

            printf("[PARSE] Body length: %zu bytes\n", request->body_length);
            printf("[PARSE] Body preview: %.100s%s\n",
                   request->body,
                   request->body_length > 100 ? "..." : "");
        }
    }

    free(request_copy);
}
