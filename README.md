# Chat App

This is a C-based group chat application that makes use of sockets and multithreading. To handle multiple clients and broadcast messages from one client to the others, the server is multithreaded. The client also has two threads: one for receiving broadcast messages (messages from other clients), and the other for receiving user input and sending it to the server.

Currently, the server IP address is set to localhost, hence the ChatApp would run only on the terminals of same machine.

# To-do
- Implement graceful shutdown for server
- Implement Keyboard interrupt shutdown in both server and client , currently closing program using keyboard interrupt in client causes infinite loop in server (and vice versa). 
- use ifreq to serve clients in LAN and not just localhost

# Requirements

- Unix OS and gcc to compile the code.

# Compile and Running

- use the make file to compile
- This generates 2 executables - s (for server) and c (for client)
- Run server file before client and provide necessary arguements

# Screenshot

![Working Example](https://github.com/Vidit-Patel17/ChatApp/blob/main/screenshot.png?raw=true)
