#define _POSIX_C_SOURCE 200809L
#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

/*
 * EXTERNAL API CLIENT
 * 
 * Shows how to call external APIs and parse their JSON responses
 * Examples:
 * 1. Weather API
 * 2. Exchange Rates API
 * 3. Random User API
 * 4. Cat Facts API
 */

// ========================================
// HTTP Client - Make HTTP Requests
// ========================================

typedef struct {
    int status_code;
    char *body;
    size_t body_length;
    char *error;
} HTTPResponse;

static void http_response_free(HTTPResponse *resp) {
    if (resp->body) free(resp->body);
    if (resp->error) free(resp->error);
}

// Make HTTP GET request to external API
static HTTPResponse http_get(const char *host, const char *path, int port) {
    HTTPResponse response = {0};
    int sock = -1;
    struct addrinfo hints, *result, *rp;
    
    // Validate input
    if (!host || !path) {
        response.error = strdup("Invalid parameters: host or path is NULL");
        return response;
    }
    
    // Resolve hostname
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    int getaddr_result = getaddrinfo(host, port_str, &hints, &result);
    if (getaddr_result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "DNS lookup failed for %s: %s", 
                 host, gai_strerror(getaddr_result));
        response.error = strdup(error_msg);
        return response;
    }
    
    // Try to connect
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1) continue;
        
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
            break; // Success!
        }
        
        close(sock);
        sock = -1;
    }
    
    freeaddrinfo(result);
    
    if (sock == -1) {
        response.error = strdup("Failed to connect to server");
        return response;
    }
    
    // Build HTTP request
    char request[2048];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: C-HTTP-Server/1.0\r\n"
        "Accept: application/json\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host);
    
    // Send request
    send(sock, request, strlen(request), 0);
    
    // Read response
    char buffer[4096];
    char *full_response = malloc(4096);
    size_t total_received = 0;
    size_t capacity = 4096;
    
    if (!full_response) {
        close(sock);
        response.error = strdup("Memory allocation failed");
        return response;
    }
    
    ssize_t n;
    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        // Resize if needed
        if (total_received + n + 1 > capacity) {
            capacity *= 2;
            char *new_buf = realloc(full_response, capacity);
            if (!new_buf) {
                free(full_response);
                close(sock);
                response.error = strdup("Memory allocation failed");
                return response;
            }
            full_response = new_buf;
        }
        
        memcpy(full_response + total_received, buffer, n);
        total_received += n;
    }
    
    full_response[total_received] = '\0';
    close(sock);
    
    // Parse HTTP response
    // Find status code
    char *status_line = strstr(full_response, "HTTP/1.");
    if (status_line) {
        char *space = strchr(status_line, ' ');
        if (space) {
            response.status_code = atoi(space + 1);
        }
    }
    
    // Find body (after \r\n\r\n)
    char *body_start = strstr(full_response, "\r\n\r\n");
    if (body_start) {
        body_start += 4; // Skip the \r\n\r\n
        size_t body_len = strlen(body_start);
        if (body_len > 0) {
            response.body_length = body_len;
            response.body = strdup(body_start);
        }
    }
    
    // Always set body even if empty to avoid NULL dereference
    if (!response.body) {
        response.body = strdup("");
        response.body_length = 0;
    }
    
    free(full_response);
    return response;
}

// ========================================
// JSON Parser (for external API responses)
// ========================================

// Extract string value from JSON (handles nested paths like "main.temp")
static char* json_extract_string(const char *json, const char *path) {
    char *current_json = (char*)json;
    char path_copy[256];
    strncpy(path_copy, path, sizeof(path_copy) - 1);
    
    char *token = strtok(path_copy, ".");
    
    while (token) {
        // Find "token":
        char search[128];
        snprintf(search, sizeof(search), "\"%s\"", token);
        
        char *key_pos = strstr(current_json, search);
        if (!key_pos) return NULL;
        
        // Find the value after the colon
        char *colon = strchr(key_pos, ':');
        if (!colon) return NULL;
        
        colon++; // Skip colon
        
        // Skip whitespace
        while (*colon == ' ' || *colon == '\t' || *colon == '\n' || *colon == '\r') {
            colon++;
        }
        
        // Check if next token exists (nested object)
        token = strtok(NULL, ".");
        
        if (!token) {
            // This is the final value
            if (*colon == '"') {
                // String value
                char *start = colon + 1;
                char *end = strchr(start, '"');
                if (!end) return NULL;
                
                size_t len = end - start;
                char *result = malloc(len + 1);
                if (result) {
                    strncpy(result, start, len);
                    result[len] = '\0';
                }
                return result;
            } else {
                // Number or other value as string
                char *start = colon;
                char *end = start;
                while (*end && *end != ',' && *end != '}' && *end != ']' && 
                       *end != '\n' && *end != '\r') {
                    end++;
                }
                
                // Trim trailing whitespace
                while (end > start && (*(end-1) == ' ' || *(end-1) == '\t')) {
                    end--;
                }
                
                size_t len = end - start;
                char *result = malloc(len + 1);
                if (result) {
                    strncpy(result, start, len);
                    result[len] = '\0';
                }
                return result;
            }
        } else {
            // Move to nested object
            current_json = colon;
        }
    }
    
    return NULL;
}

static double json_extract_number(const char *json, const char *path) __attribute__((unused));

static double json_extract_number(const char *json, const char *path) {
    char *str_value = json_extract_string(json, path);
    if (!str_value) return 0.0;
    
    double value = atof(str_value);
    free(str_value);
    return value;
}

// ========================================
// API Endpoint: Fetch Weather
// ========================================

void handle_api_weather(const HttpRequest *request, HttpResponse *response) {
    (void)request;
    
    // Use a simpler weather API that works with HTTP
    // wttr.in supports both HTTP and returns plain text formats
    printf("[API] Calling weather API (HTTP)...\n");
    
    // Use simple text format instead of JSON for HTTP
    HTTPResponse api_response = http_get("wttr.in", "/London?format=3", 80);
    
    if (api_response.error) {
        printf("[API] Error: %s\n", api_response.error);
        
        JSONBuilder *jb = json_builder_create();
        json_builder_append(jb, "{\n");
        json_builder_append(jb, "  \"success\": false,\n");
        json_builder_append(jb, "  \"error\": \"Failed to fetch weather data\",\n");
        
        char temp[256];
        snprintf(temp, sizeof(temp), "  \"details\": \"");
        json_builder_append(jb, temp);
        json_builder_append_escaped(jb, api_response.error);
        json_builder_append(jb, "\"\n");
        
        json_builder_append(jb, "}");
        
        response->status_code = 502;
        strcpy(response->content_type, "application/json");
        response->body = json_builder_finalize(jb);
        response->body_length = strlen(response->body);
        
        http_response_free(&api_response);
        return;
    }
    
    printf("[API] Received %zu bytes, status: %d\n", 
           api_response.body_length, api_response.status_code);
    
    // Check for redirects or errors
    if (api_response.status_code != 200) {
        JSONBuilder *jb = json_builder_create();
        json_builder_append(jb, "{\n");
        json_builder_append(jb, "  \"success\": false,\n");
        
        char temp[256];
        if (api_response.status_code == 301 || api_response.status_code == 302) {
            snprintf(temp, sizeof(temp), 
                "  \"error\": \"Weather API redirected (try HTTPS)\",\n");
        } else {
            snprintf(temp, sizeof(temp), 
                "  \"error\": \"Weather API returned status %d\",\n",
                api_response.status_code);
        }
        json_builder_append(jb, temp);
        
        snprintf(temp, sizeof(temp), "  \"status_code\": %d\n", 
                api_response.status_code);
        json_builder_append(jb, temp);
        json_builder_append(jb, "}");
        
        response->status_code = 502;
        strcpy(response->content_type, "application/json");
        response->body = json_builder_finalize(jb);
        response->body_length = strlen(response->body);
        
        http_response_free(&api_response);
        return;
    }
    
    // Parse simple format: "London: ☀️ +15°C"
    JSONBuilder *jb = json_builder_create();
    json_builder_append(jb, "{\n");
    json_builder_append(jb, "  \"success\": true,\n");
    json_builder_append(jb, "  \"location\": \"London\",\n");
    json_builder_append(jb, "  \"data\": {\n");
    json_builder_append(jb, "    \"weather\": \"");
    
    // Clean and escape the response
    if (api_response.body && api_response.body_length > 0) {
        // Remove newlines
        char *cleaned = strdup(api_response.body);
        for (size_t i = 0; cleaned[i]; i++) {
            if (cleaned[i] == '\n' || cleaned[i] == '\r') {
                cleaned[i] = ' ';
            }
        }
        json_builder_append_escaped(jb, cleaned);
        free(cleaned);
    } else {
        json_builder_append(jb, "Unknown");
    }
    
    json_builder_append(jb, "\"\n");
    json_builder_append(jb, "  },\n");
    json_builder_append(jb, "  \"source\": \"wttr.in\",\n");
    json_builder_append(jb, "  \"note\": \"Using HTTP endpoint (limited data)\"\n");
    json_builder_append(jb, "}");
    
    response->status_code = 200;
    strcpy(response->content_type, "application/json");
    response->body = json_builder_finalize(jb);
    response->body_length = strlen(response->body);
    
    http_response_free(&api_response);
}

// ========================================
// API Endpoint: Fetch Exchange Rates
// ========================================

void handle_api_exchange_rates(const HttpRequest *request, HttpResponse *response) {
    (void)request;
    
    printf("[API] Exchange rates endpoint called\n");
    
    // Most currency APIs require HTTPS (port 443) which we don't support
    // Provide helpful error message
    JSONBuilder *jb = json_builder_create();
    json_builder_append(jb, "{\n");
    json_builder_append(jb, "  \"success\": false,\n");
    json_builder_append(jb, "  \"error\": \"Exchange rate APIs require HTTPS\",\n");
    json_builder_append(jb, "  \"info\": \"This server doesn't support SSL/TLS connections\",\n");
    json_builder_append(jb, "  \"suggestion\": \"To enable this, add OpenSSL library support\",\n");
    json_builder_append(jb, "  \"sample_data\": {\n");
    json_builder_append(jb, "    \"base\": \"USD\",\n");
    json_builder_append(jb, "    \"rates\": {\n");
    json_builder_append(jb, "      \"EUR\": 0.85,\n");
    json_builder_append(jb, "      \"GBP\": 0.73,\n");
    json_builder_append(jb, "      \"JPY\": 110.25,\n");
    json_builder_append(jb, "      \"CAD\": 1.25\n");
    json_builder_append(jb, "    },\n");
    json_builder_append(jb, "    \"note\": \"This is sample data, not live rates\"\n");
    json_builder_append(jb, "  }\n");
    json_builder_append(jb, "}");
    
    response->status_code = 501; // Not Implemented
    strcpy(response->content_type, "application/json");
    response->body = json_builder_finalize(jb);
    response->body_length = strlen(response->body);
}

// ========================================
// API Endpoint: Random Quote
// ========================================

void handle_api_quote(const HttpRequest *request, HttpResponse *response) {
    (void)request;
    
    printf("[API] Quote endpoint called\n");
    
    // Most quote APIs require HTTPS
    JSONBuilder *jb = json_builder_create();
    json_builder_append(jb, "{\n");
    json_builder_append(jb, "  \"success\": false,\n");
    json_builder_append(jb, "  \"error\": \"Quote APIs require HTTPS\",\n");
    json_builder_append(jb, "  \"info\": \"This server uses plain HTTP (no SSL/TLS)\",\n");
    json_builder_append(jb, "  \"sample_quote\": {\n");
    json_builder_append(jb, "    \"quote\": \"The only way to do great work is to love what you do.\",\n");
    json_builder_append(jb, "    \"author\": \"Steve Jobs\",\n");
    json_builder_append(jb, "    \"note\": \"This is a sample quote, not from API\"\n");
    json_builder_append(jb, "  },\n");
    json_builder_append(jb, "  \"how_to_fix\": \"Add OpenSSL library for HTTPS support\"\n");
    json_builder_append(jb, "}");
    
    response->status_code = 501; // Not Implemented
    strcpy(response->content_type, "application/json");
    response->body = json_builder_finalize(jb);
    response->body_length = strlen(response->body);
}

// ========================================
// API Endpoint: Proxy (Pass-through)
// ========================================

void handle_api_proxy(const HttpRequest *request, HttpResponse *response) {
    (void)request;
    
    printf("[API] Proxy endpoint called\n");
    
    // GitHub API requires HTTPS
    JSONBuilder *jb = json_builder_create();
    json_builder_append(jb, "{\n");
    json_builder_append(jb, "  \"success\": false,\n");
    json_builder_append(jb, "  \"error\": \"Proxy target requires HTTPS\",\n");
    json_builder_append(jb, "  \"info\": \"Most modern APIs use HTTPS (SSL/TLS)\",\n");
    json_builder_append(jb, "  \"limitation\": \"This server only supports plain HTTP\",\n");
    json_builder_append(jb, "  \"how_it_would_work\": {\n");
    json_builder_append(jb, "    \"step1\": \"Your server receives request\",\n");
    json_builder_append(jb, "    \"step2\": \"Your server calls external API\",\n");
    json_builder_append(jb, "    \"step3\": \"External API returns data\",\n");
    json_builder_append(jb, "    \"step4\": \"Your server forwards response to client\"\n");
    json_builder_append(jb, "  },\n");
    json_builder_append(jb, "  \"note\": \"This is how API proxies/gateways work!\"\n");
    json_builder_append(jb, "}");
    
    response->status_code = 501; // Not Implemented  
    strcpy(response->content_type, "application/json");
    response->body = json_builder_finalize(jb);
    response->body_length = strlen(response->body);
}