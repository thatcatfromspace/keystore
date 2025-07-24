# KeyStore

KeyStore is a lightweight, blazing-fast in-memory database designed for developers who want simple, reliable key-value storage without the overhead of traditional databases.

## Why KeyStore?

- **Zero hassle**: No setup, no config files, just run and store your data.
- **Flexible**: Use it from the command line, as a networked HTTP or TCP server, or even in Docker.
- **Efficient**: Built with performance and minimalism in mind.
- **Self-cleaning**: Automatically evicts old or unused data to keep memory usage in check.
- **Health checks**: Each mode can report its status for monitoring and system integration.

### Dependecies

- **C++17**: KeyStore is built with modern C++ features for performance and safety.
- **Crow**: A C++ micro web framework for the HTTP server mode.
- **spdlog**: For logging and debugging.
- **make**: For building the project.

If you do not want the hassle of building from source, you can download pre-built binaries from the [releases page](https://github.com/thatcatfromspace/keystore/releases) if you are on Linux, or use the Docker image for other platforms.

## How to Use

### 1. Command Line Mode (CLI)

Interact with your database directly from the terminal. Great for quick tests, scripting, or learning how it works.

```sh
./keystore cli
keystore> set mykey hello
keystore> get mykey
hello
keystore> del mykey
```

### 2. HTTP Server Mode

Run KeyStore as a server and connect from your own apps or tools. Perfect for integrating with other services or building distributed systems.

```sh
./keystore http
# Now connect from your client or use curl/postman to interact with the API
```

Optionally, Keystore comes bundled with Docker support. Start the Docker container using:

```sh
docker compose up --build -d
```

The HTTP server runs on port `3000`.

**NOTE**: For POST methods, the endpoint only accepts payload as `www-form-urlencoded`. For example, to set a value:

```sh
curl -X POST http://localhost:3000/set -d "key=username&value=alice"
```

This is due to an internal limitation with Crow.

### 3. TCP Server Mode

For lightweight, raw-socket access, run KeyStore in TCP mode. This is ideal for custom clients or embedded systems.

```sh
./keystore tcp
# Connect using your own TCP client or the provided example client
```

## Example: Storing and Retrieving Data

- **Set a value:**
  - CLI: `set username alice`
  - HTTP: `POST /set` with `{ "key": "username", "value": "alice" }`
  - TCP: Send a raw command like `set username alice\n`
- **Get a value:**
  - CLI: `get username`
  - HTTP: `GET /get?key=username`
  - TCP: Send `get username\n`
- **Delete a value:**
  - CLI: `del username`
  - HTTP: `DELETE /delete?key=username`
  - TCP: Send `delete username\n`

## What's New

- **TCP mode**: Now you can use KeyStore as a TCP server for even more flexibility.
- **Status reporting**: Each mode can report its current status, making it easy to integrate with health checks and monitoring tools.

## Upcoming Features

- **Swagger/OpenAPI documentation** for easy API exploration
- **Persistence**: Optionally save your data to disk
- **More advanced eviction policies**
- **Client libraries** for popular languages

## Getting Started

Clone the repository and build:

```sh
git clone https://github.com/thatcatfromspace/keystore.git
cd keystore
make
```

## License

MIT License
