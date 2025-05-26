# Console Chat (Client-Server) for Linux


https://github.com/user-attachments/assets/8d64331a-09b6-4368-8e28-1cde57d076c2


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
