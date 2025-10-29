# 🌍 Calling External APIs from Your C Server

## Overview

Your server can now **call external APIs** and parse their JSON responses! This shows how backend servers integrate with third-party services.

### Real-World Use Cases
- 🌦️ **Weather data** - Get weather from APIs
- 💱 **Exchange rates** - Get current currency rates  
- 📰 **News feeds** - Fetch news from APIs
- 💳 **Payment processing** - Call Stripe/PayPal APIs
- 📧 **Email sending** - Call SendGrid/Mailgun APIs
- 🗺️ **Maps/Geocoding** - Call Google Maps API
- 📊 **Analytics** - Send data to analytics APIs

---

## How It Works

### The Complete Flow

```
1. Client → Your Server
   User: curl http://localhost:8080/api/weather

2. Your Server → External API
   Your server: Connect to wttr.in
   Your server: Send HTTP GET request
   
3. External API → Your Server
   wttr.in: Returns JSON weather data
   
4. Your Server Parses Response
   Extract: temperature, conditions, etc.
   
5. Your Server → Client
   Return: Formatted JSON to user
```

**Your server acts as a middleman/proxy!**

---

## The HTTP Client

### Making HTTP Requests

```c
// Connect to external server
HTTPResponse http_get(const char *host, const char *path, int port) {
    // 1. Resolve hostname (DNS lookup)
    getaddrinfo(host, port_str, &hints, &result);
    
    // 2. Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // 3. Connect to server
    connect(sock, server_addr, addr_len);
    
    // 4. Send HTTP request
    send(sock, "GET /path HTTP/1.1\r\n...", ...);
    
    // 5. Receive response
    char buffer[4096];
    while (recv(sock, buffer, sizeof(buffer), 0) > 0) {
        // Accumulate response...
    }
    
    // 6. Parse HTTP response
    // Extract status code, headers, body
    
    // 7. Return parsed response
    return response;
}
```

### What Happens Behind the Scenes

```
1. DNS Lookup
   wttr.in → 5.9.243.187

2. TCP Connection
   socket() → connect() → TCP handshake

3. HTTP Request Sent
   GET /London?format=j1 HTTP/1.1
   Host: wttr.in
   User-Agent: C-HTTP-Server/1.0
   
4. HTTP Response Received
   HTTP/1.1 200 OK
   Content-Type: application/json
   
   {"current_condition":[{"temp_C":"15"...}]}

5. Parse Response
   Extract body from headers
   Parse JSON fields
   
6. Return to Handler
   Your endpoint uses the data
```

---

## Endpoint Examples

### 1. Weather API

**Endpoint:** `GET /api/weather`

**What it does:**
1. Calls wttr.in API for London weather
2. Parses JSON response
3. Extracts temperature, conditions, humidity
4. Returns formatted JSON

**Code Flow:**
```c
void handle_api_weather(HttpRequest *req, HttpResponse *res) {
    // Call external API
    HTTPResponse api_resp = http_get("wttr.in", "/London?format=j1", 80);
    
    if (api_resp.error) {
        // Handle connection error
        res->status_code = 502; // Bad Gateway
        res->body = "{\"error\":\"Failed to connect\"}";
        return;
    }
    
    // Parse JSON response
    char *temp = json_extract_string(api_resp.body, "current_condition.0.temp_C");
    char *weather = json_extract_string(api_resp.body, "current_condition.0.weatherDesc.0.value");
    
    // Build our response
    snprintf(json, size,
        "{\n"
        "  \"temperature\": %s,\n"
        "  \"weather\": \"%s\"\n"
        "}", temp, weather);
    
    res->body = json;
    
    // Clean up
    free(temp);
    free(weather);
    http_response_free(&api_resp);
}
```

**Test:**
```bash
curl http://localhost:8080/api/weather
```

**Response:**
```json
{
  "success": true,
  "location": "London",
  "data": {
    "temperature_celsius": 15,
    "weather": "Partly cloudy",
    "feels_like_celsius": 13,
    "humidity_percent": 72
  },
  "source": "wttr.in API"
}
```

---

### 2. Exchange Rates API

**Endpoint:** `GET /api/exchange`

**What it does:**
1. Calls exchangerate-api.com for USD rates
2. Parses rates for EUR, GBP, JPY, CAD
3. Returns formatted rates

**External API Response:**
```json
{
  "base": "USD",
  "rates": {
    "EUR": 0.85,
    "GBP": 0.73,
    "JPY": 110.25,
    "CAD": 1.25
  }
}
```

**Your Response:**
```json
{
  "success": true,
  "base": "USD",
  "rates": {
    "EUR": 0.85,
    "GBP": 0.73,
    "JPY": 110.25,
    "CAD": 1.25
  },
  "source": "exchangerate-api.com"
}
```

**Test:**
```bash
curl http://localhost:8080/api/exchange
```

---

### 3. Random Quote API

**Endpoint:** `GET /api/quote`

**What it does:**
1. Calls quotable.io for random quote
2. Extracts quote and author
3. Returns formatted response

**Test:**
```bash
curl http://localhost:8080/api/quote
```

**Response:**
```json
{
  "success": true,
  "data": {
    "quote": "The only way to do great work is to love what you do.",
    "author": "Steve Jobs"
  },
  "source": "quotable.io"
}
```

---

### 4. Proxy Endpoint

**Endpoint:** `GET /api/proxy`

**What it does:**
Pass-through mode - returns external API response directly without modification

**Code:**
```c
void handle_api_proxy(HttpRequest *req, HttpResponse *res) {
    HTTPResponse api_resp = http_get("api.github.com", "/zen", 443);
    
    // Return external response directly (proxy)
    res->status_code = api_resp.status_code;
    res->body = api_resp.body;  // No modification!
}
```

**Test:**
```bash
curl http://localhost:8080/api/proxy
```

---

## JSON Parsing from External APIs

### The Parser

Handles nested JSON paths:

```c
// Extract value from: {"current_condition":[{"temp_C":"15"}]}
char *temp = json_extract_string(json, "current_condition.0.temp_C");
// Returns: "15"

// Extract from: {"user":{"name":"John"}}
char *name = json_extract_string(json, "user.name");
// Returns: "John"

// Extract from: {"data":[{"id":1},{"id":2}]}
char *id = json_extract_string(json, "data.1.id");
// Returns: "2"
```

### Features

✅ **Nested objects** - `"user.address.city"`  
✅ **Array indexing** - `"items.0.name"`  
✅ **String values** - Handles quotes  
✅ **Number values** - Returns as string  
✅ **Error handling** - Returns NULL if not found

### Example Parsing

**External API Returns:**
```json
{
  "weather": {
    "current": {
      "temperature": 20,
      "condition": "Sunny",
      "wind": {
        "speed": 15,
        "direction": "NW"
      }
    }
  }
}
```

**Your Code:**
```c
char *temp = json_extract_string(json, "weather.current.temperature");
char *condition = json_extract_string(json, "weather.current.condition");
char *wind_speed = json_extract_string(json, "weather.current.wind.speed");

printf("Temp: %s°C, Condition: %s, Wind: %s km/h\n",
       temp, condition, wind_speed);
// Output: Temp: 20°C, Condition: Sunny, Wind: 15 km/h

free(temp);
free(condition);
free(wind_speed);
```

---

## Error Handling

### Network Errors

```c
HTTPResponse api_resp = http_get(host, path, port);

if (api_resp.error) {
    // Connection failed, timeout, DNS error, etc.
    printf("Error: %s\n", api_resp.error);
    
    // Return 502 Bad Gateway to client
    res->status_code = 502;
    res->body = "{\"error\":\"External API unavailable\"}";
    
    http_response_free(&api_resp);
    return;
}
```

### HTTP Errors

```c
if (api_resp.status_code != 200) {
    // External API returned error (404, 500, etc.)
    printf("API returned status: %d\n", api_resp.status_code);
    
    res->status_code = 502;
    res->body = "{\"error\":\"External API error\"}";
    return;
}
```

### Parsing Errors

```c
char *data = json_extract_string(api_resp.body, "field");

if (!data) {
    // Field not found in JSON
    res->status_code = 502;
    res->body = "{\"error\":\"Unexpected API response format\"}";
    return;
}

// Use data...
free(data);
```

---

## Real-World Integration Examples

### 1. Payment Processing (Stripe-like)

```c
void handle_api_create_payment(HttpRequest *req, HttpResponse *res) {
    // Get payment details from client
    char *amount = json_get_string_value(req->body, "amount");
    char *currency = json_get_string_value(req->body, "currency");
    
    // Call Stripe API
    char post_data[512];
    snprintf(post_data, sizeof(post_data),
        "{\"amount\":%s,\"currency\":\"%s\"}", amount, currency);
    
    HTTPResponse stripe_resp = http_post(
        "api.stripe.com",
        "/v1/charges",
        443,
        post_data
    );
    
    // Parse Stripe response
    char *charge_id = json_extract_string(stripe_resp.body, "id");
    char *status = json_extract_string(stripe_resp.body, "status");
    
    // Return to client
    // ... build response ...
}
```

### 2. Email Sending (SendGrid-like)

```c
void handle_api_send_email(HttpRequest *req, HttpResponse *res) {
    char *to = json_get_string_value(req->body, "to");
    char *subject = json_get_string_value(req->body, "subject");
    char *body = json_get_string_value(req->body, "body");
    
    // Call SendGrid API
    char email_json[2048];
    snprintf(email_json, sizeof(email_json),
        "{\n"
        "  \"personalizations\":[{\"to\":[{\"email\":\"%s\"}]}],\n"
        "  \"subject\":\"%s\",\n"
        "  \"content\":[{\"type\":\"text/plain\",\"value\":\"%s\"}]\n"
        "}", to, subject, body);
    
    HTTPResponse sendgrid_resp = http_post(
        "api.sendgrid.com",
        "/v3/mail/send",
        443,
        email_json
    );
    
    // Check if sent successfully
    if (sendgrid_resp.status_code == 202) {
        res->body = "{\"success\":true,\"message\":\"Email sent\"}";
    }
}
```

### 3. Geocoding (Google Maps-like)

```c
void handle_api_geocode(HttpRequest *req, HttpResponse *res) {
    char *address = json_get_string_value(req->body, "address");
    
    // Build API URL
    char path[512];
    snprintf(path, sizeof(path),
        "/maps/api/geocode/json?address=%s&key=YOUR_KEY", address);
    
    // Call Google Maps API
    HTTPResponse maps_resp = http_get("maps.googleapis.com", path, 443);
    
    // Parse coordinates
    char *lat = json_extract_string(maps_resp.body, "results.0.geometry.location.lat");
    char *lng = json_extract_string(maps_resp.body, "results.0.geometry.location.lng");
    
    // Return coordinates
    // ... build response ...
}
```

---

## Comparison with Other Languages

### Our C Server
```c
HTTPResponse resp = http_get("api.example.com", "/data", 80);
char *value = json_extract_string(resp.body, "field");
// Use value...
free(value);
http_response_free(&resp);
```

### Node.js (fetch)
```javascript
const response = await fetch('http://api.example.com/data');
const json = await response.json();
const value = json.field;
```

### Python (requests)
```python
response = requests.get('http://api.example.com/data')
json_data = response.json()
value = json_data['field']
```

### Go
```go
resp, _ := http.Get("http://api.example.com/data")
body, _ := ioutil.ReadAll(resp.Body)
var data map[string]interface{}
json.Unmarshal(body, &data)
value := data["field"]
```

**All doing the same thing: HTTP request → Parse JSON → Use data!**

---

## Testing

### Test All External APIs

```bash
# Start server
./bin/http-server

# Weather
curl http://localhost:8080/api/weather

# Exchange rates
curl http://localhost:8080/api/exchange

# Random quote
curl http://localhost:8080/api/quote

# Proxy
curl http://localhost:8080/api/proxy
```

### Watch Server Logs

The server logs show what's happening:

```
[API] Calling external weather API...
[API] Received 1234 bytes from weather API
[API] Status code: 200
[API] Parsed - Temp: 15°C, Weather: Partly cloudy
```

---

## Performance Considerations

### Timeouts

External APIs can be slow or unresponsive. Add timeouts:

```c
// Set socket timeout (5 seconds)
struct timeval timeout;
timeout.tv_sec = 5;
timeout.tv_usec = 0;
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
```

### Caching

Don't call external APIs on every request:

```c
// Cache weather data for 10 minutes
static char *cached_weather = NULL;
static time_t cache_time = 0;

if (time(NULL) - cache_time < 600) {  // 10 minutes
    // Use cached data
    res->body = strdup(cached_weather);
} else {
    // Fetch fresh data
    HTTPResponse api_resp = http_get(...);
    cached_weather = strdup(api_resp.body);
    cache_time = time(NULL);
}
```

### Async Processing

For slow APIs, use threads:

```c
// Spawn thread to call API in background
pthread_t thread;
pthread_create(&thread, NULL, fetch_weather_async, NULL);
pthread_detach(thread);

// Return immediately
res->body = "{\"status\":\"Processing...\"}";
```

---

## Summary

### What You Learned

✅ **HTTP client** - Connect to external servers  
✅ **DNS resolution** - Look up hostnames  
✅ **Request building** - Format HTTP requests  
✅ **Response parsing** - Extract status and body  
✅ **JSON parsing** - Extract nested fields  
✅ **Error handling** - Handle failures gracefully  
✅ **Real-world patterns** - Payments, emails, maps

### How It Works

```
Your Server = Middleman/Proxy

Client → Your Server → External API → Your Server → Client
        (request)    (parse JSON)    (format)     (response)
```

### Files

- [src/api_client.c](computer:///mnt/user-data/outputs/http-server/src/api_client.c) - HTTP client + external API handlers
- 4 endpoints: weather, exchange, quote, proxy

### Next Steps

1. Add more external APIs (GitHub, Twitter, etc.)
2. Implement caching
3. Add authentication (API keys)
4. Handle rate limiting
5. Use real JSON library (cJSON)

**Now you understand how backend servers integrate with external services!** 🌍🔌