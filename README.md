# UDPEcho
Simple UDP Client- Server

This project contains a makefile, a server, and a client.

The project is built with make.

The client consists of a main routine and a signal routine to process Control-C from the user.
The client will run forever until it receives Control-C. 
Once terminated, the client will output the Total Bytes Sent and Average Throughput.

The server consists of a main routine and signal routine to process Control-C from the user.  
The server will run forever until it receives Control-C.  
Once terminated, the server will output the total bytes received and the number of connections received. 
This is immediately followed by detail output about each connection.  
The server also specifies a C-struct to store each incoming connection.  
The program can currently handle up to 100 connections.  
To accept more connections, change the size of the 'conns' array.
