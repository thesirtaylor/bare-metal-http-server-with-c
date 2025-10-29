#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stddef.h>

/* ============================================
   HTTP Server - Core Declarations
   ============================================ */

// HTTP Method Types
typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_UNKNOWN
} HttpMethod;

// HTTP Request Structure
typedef struct {
    HttpMethod method;
    char path[256];
    char content_type[128];
    char *body;
    size_t body_length;
    char client_ip[46];  // IPv6 max length
} HttpRequest;

// HTTP Response Structure
typedef struct {
    int status_code;
    char content_type[128];
    char *body;
    size_t body_length;
} HttpResponse;

/* ============================================
   Server Functions
   ============================================ */

/**
 * Initialize and start the HTTP server
 * @param port The port number to listen on
 * @return 0 on success, -1 on failure
 */
int start_http_server(int port);

/**
 * Parse raw HTTP request into HttpRequest structure
 * @param raw_request Raw HTTP request string
 * @param request Pointer to HttpRequest structure to fill
 */
void parse_http_request(const char *raw_request, HttpRequest *request);

/**
 * Handle incoming client connection
 * @param client_fd Client socket file descriptor
 */
void handle_client_connection(int client_fd);

/**
 * Route request to appropriate handler
 * @param request The parsed HTTP request
 * @param response The HTTP response to fill
 */
void route_request(const HttpRequest *request, HttpResponse *response);

/**
 * Send HTTP response back to client
 * @param client_fd Client socket file descriptor
 * @param response The HTTP response to send
 */
void send_http_response(int client_fd, const HttpResponse *response);

/* ============================================
   Route Handlers
   ============================================ */

/**
 * GET / - Root endpoint
 */
void handle_root(const HttpRequest *request, HttpResponse *response);

/**
 * GET /info - Server information
 */
void handle_info(const HttpRequest *request, HttpResponse *response);

/**
 * GET /image - Serve an image file
 */
void handle_image(const HttpRequest *request, HttpResponse *response);

/**
 * GET /glossary - C language reference
 */
void handle_glossary(const HttpRequest *request, HttpResponse *response);

/**
 * GET /how-it-works - Request/response cycle explanation
 */
void handle_how_it_works(const HttpRequest *request, HttpResponse *response);

/**
 * JSON Builder for safe JSON construction
 */
typedef struct {
    char *buffer;
    size_t size;
    size_t capacity;
} JSONBuilder;

JSONBuilder* json_builder_create(void);
void json_builder_append(JSONBuilder *jb, const char *str);
void json_builder_append_escaped(JSONBuilder *jb, const char *str);
char* json_builder_finalize(JSONBuilder *jb);

/**
 * JSON API Endpoints
 */
void handle_api_health(const HttpRequest *request, HttpResponse *response);
void handle_api_users_get(const HttpRequest *request, HttpResponse *response);
void handle_api_users_post(const HttpRequest *request, HttpResponse *response);
void handle_api_stats(const HttpRequest *request, HttpResponse *response);
void handle_api_login(const HttpRequest *request, HttpResponse *response);
void handle_api_calculate(const HttpRequest *request, HttpResponse *response);
void handle_api_time(const HttpRequest *request, HttpResponse *response);

/**
 * External API Client Endpoints (calls other APIs)
 */
void handle_api_weather(const HttpRequest *request, HttpResponse *response);
void handle_api_exchange_rates(const HttpRequest *request, HttpResponse *response);
void handle_api_quote(const HttpRequest *request, HttpResponse *response);
void handle_api_proxy(const HttpRequest *request, HttpResponse *response);

/**
 * POST /echo - Echo back the request body
 */
void handle_echo(const HttpRequest *request, HttpResponse *response);

/**
 * POST /data - Process JSON data
 */
void handle_post_data(const HttpRequest *request, HttpResponse *response);

/**
 * 404 Not Found handler
 */
void handle_not_found(const HttpRequest *request, HttpResponse *response);

/* ============================================
   Utility Functions
   ============================================ */

/**
 * Read an image file into memory
 * @param filename Path to image file
 * @param size Pointer to store file size
 * @return Pointer to file data (caller must free)
 */
unsigned char *read_image_file(const char *filename, size_t *size);

/**
 * URL decode a string
 * @param str String to decode
 */
void url_decode(char *str);

/**
 * Get MIME type from file extension
 * @param filename File name or path
 * @return MIME type string
 */
const char *get_mime_type(const char *filename);

#endif /* HTTP_SERVER_H */