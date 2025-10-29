## 📚 Endpoints

### 🌐 Web Pages

#### `GET /`
**Home page** - Overview of all available endpoints with direct links and curl examples.

```bash
curl http://localhost:8080/
```

#### `GET /info`
**Network concepts** - Interactive page explaining TCP/IP, DNS, ports, sockets, and the OSI model. Perfect for understanding how networking works.

```bash
curl http://localhost:8080/info
```

#### `GET /glossary`
**C functions reference** - Beginner-friendly guide to every C function used in this server. Includes memory management, string operations, and network functions with detailed explanations.

```bash
curl http://localhost:8080/glossary
```

#### `GET /how-it-works`
**Request/response cycle** - Deep dive into the universal 10-step pattern that ALL servers follow (Express, Flask, Django, Rails). Shows what frameworks hide from you.

```bash
curl http://localhost:8080/how-it-works
```

---

### 🔌 JSON API Endpoints

#### `GET /api/health`
Health check endpoint - Returns server status and uptime.

```bash
curl http://localhost:8080/api/health
```

**Response:**
```json
{
  "status": "healthy",
  "service": "webserver",
  "version": "1.0.0",
  "timestamp": "2025-10-28 16:30:45",
  "uptime_seconds": 12345
}
```

#### `GET /api/users`
Get list of users (simulated database).

```bash
curl http://localhost:8080/api/users
```

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "name": "Alice Johnson",
      "email": "alice@example.com",
      "role": "admin"
    }
  ],
  "count": 3
}
```

#### `POST /api/users`
Create a new user.

```bash
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"John Doe","email":"john@example.com","role":"user"}'
```

**Response:**
```json
{
  "success": true,
  "message": "User created successfully",
  "data": {
    "id": 456,
    "name": "John Doe",
    "email": "john@example.com",
    "role": "user"
  }
}
```

#### `POST /api/login`
User authentication.

```bash
curl -X POST http://localhost:8080/api/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"password"}'
```

**Response:**
```json
{
  "success": true,
  "message": "Login successful",
  "data": {
    "user": "admin",
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9",
    "expires_in": 3600
  }
}
```

#### `POST /api/calculate`
Mathematical operations API.

```bash
curl -X POST http://localhost:8080/api/calculate \
  -H "Content-Type: application/json" \
  -d '{"a":10,"b":5,"operation":"add"}'
```

**Operations:** `add`, `subtract`, `multiply`, `divide`

**Response:**
```json
{
  "success": true,
  "data": {
    "a": 10,
    "b": 5,
    "operation": "add",
    "result": 15.00
  }
}
```

#### `GET /api/stats`
Server statistics and metrics.

```bash
curl http://localhost:8080/api/stats
```

**Response:**
```json
{
  "success": true,
  "data": {
    "requests_total": 1523,
    "requests_per_second": 12.5,
    "active_connections": 3,
    "total_bytes_sent": 15728640,
    "uptime_hours": 48.5
  }
}
```

#### `GET /api/time`
Current server time in multiple formats.

```bash
curl http://localhost:8080/api/time
```

---

### 🌍 External API Integration

#### `GET /api/weather`
Get London weather by calling wttr.in API.

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
    "humidity_percent": 72
  }
}
```

#### `GET /api/exchange`
Get USD exchange rates from external API.

```bash
curl http://localhost:8080/api/exchange
```

#### `GET /api/quote`
Get random inspirational quote.

```bash
curl http://localhost:8080/api/quote
```

### JSON Handling

Custom JSON builder and parser for production-ready API responses:

- **JSONBuilder** - Dynamic string building with character escaping
- **JSONParser** - Safe extraction of values from JSON strings
- Handles special characters (quotes, newlines, etc.)
- Memory-safe with automatic buffer resizing
