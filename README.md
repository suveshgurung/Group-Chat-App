# Group Chat Application - Using Socket Programming

This project features a basic group chat application made using socket programming. The features of this application are:\
1. A server which handles all the requests from the client.
2. Upto **100** clients at a time that can connect to the server.
3. A **server-log.txt** file which contains each message sent by each client.

## Build

To build the project, run the command:
```sh
make
```
This should generate two executables namely **server** and **client**.

## Usage

First of all start the server using the command:
```sh
./server
```
This should start the server. Now the server starts listening for incoming clients.\
\
To connect to the server as a client, run the command:
```sh
./client
```
This should connect you to the server. Now you can start sending messages as a client (person in a group chat).
