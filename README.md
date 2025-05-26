# Console Chat (Client-Server) for Linux

*General application workflow diagram*

## Project Description

A simple console chat application where clients connect to the server and communicate directly with it (not with each other). 
The server uses `fork()` to handle multiple clients simultaneously.

## Features

### Server Side
- Multi-client architecture using `fork()`
- Logging of all events
- Client exit handling
- Zombie process prevention
- Buffered input/output

### Client Side
- Interactive console interface
- `exit` command handling
- Sending identical messages