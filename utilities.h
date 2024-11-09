#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

#define INITIAL_BUFFER_SIZE 1024
#define CHUNK_SIZE 512
#define IM_INIT {NULL, 0}     // message initializer.

typedef struct {
  char *buffer;
  size_t len;
} message;

struct sockaddr_in create_ipv4_socket_addr(const char *ipAddress, uint16_t port);
int initial_client_socket_operation();
int initial_server_socket_operation(int *serverSocketFD);
char *recv_data(int sockFD);
message input_message();

#endif
