# KeyStore

KeyStore is a lightweight, blazing-fast in-memory database designed for developers who want simple, reliable key-value storage without the overhead of traditional databases.

## Why KeyStore?

- **Zero hassle**: No setup, no config files, just run and store your data.
- **Flexible**: Use it from the command line or as a networked server.
- **Efficient**: Built with performance and minimalism in mind.
- **Self-cleaning**: Automatically evicts old or unused data to keep memory usage in check.

## How to Use

### 1. Command Line Mode (CLI)

Interact with your database directly from the terminal. Great for quick tests, scripting, or learning how it works.

```sh
./keystore cli
keystore> set mykey hello
keystore> get mykey
hello
keystore> delete mykey
```

### 2. HTTPS Server Mode

Run KeyStore as a server and connect from your own apps or tools. Perfect for integrating with other services or building distributed systems.

```sh
./keystore https
# Now connect from your client or use curl/postman to interact with the API
```

Optionally, Keystore comes bundled with Docker support. Start the Docker container using:

```sh
docker compose up --build -d
```

## Example: Storing and Retrieving Data

- **Set a value:**
  - CLI: `set username alice`
  - HTTPS: `POST /set` with `{ "key": "username", "value": "alice" }`
- **Get a value:**
  - CLI: `get username`
  - HTTPS: `GET /get?key=username`
- **Delete a value:**
  - CLI: `delete username`
  - HTTPS: `DELETE /delete?key=username`

## Upcoming Features

- **TCP mode** for secure remote access
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