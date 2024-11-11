#include "utilities.h"

struct sockaddr_in create_ipv4_socket_addr(const char *ipAddress, uint16_t port) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  int returnVal;

  // error handling.
  if ((returnVal = inet_pton(AF_INET, ipAddress, &address.sin_addr.s_addr)) != 1) {
    if (returnVal == 0) {
      fprintf(stderr, "IP address does not represent a valid network\n");
      exit(2);
    }
    if (returnVal == -1) {
      perror("inet_pton");
      exit(3);
    }
  }

  return address;
}

int initial_client_socket_operation() {
  int socketFD = socket(AF_INET, SOCK_STREAM, 0);

  if (socketFD == -1) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in address = create_ipv4_socket_addr("127.0.0.1", 6000);

  if (connect(socketFD, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("connect");
    return 4;
  }

  return socketFD;
}

int initial_server_socket_operation(int *serverSocketFD) {
  *serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

  if (*serverSocketFD == -1) {
    perror("socket");
    exit(1);
  }

  struct sockaddr_in address = create_ipv4_socket_addr("127.0.0.1", 6000);

  if (bind(*serverSocketFD, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("bind");
    exit(2);
  }

  if (listen(*serverSocketFD, 0) == -1) {
    perror("listen");
    exit(3);
  }

  struct sockaddr_in clientSocketAddr;
  socklen_t clientAddrLen = sizeof(clientSocketAddr);
  int clientSocketFD = accept(*serverSocketFD, (struct sockaddr *)&address, &clientAddrLen);
  if (clientSocketFD == -1) {
    perror("accept");
    exit(4);
  }

  return clientSocketFD;
}

char *recv_data(int sockFD) {
  size_t bufferSize = INITIAL_BUFFER_SIZE;
  ssize_t dataLen = 0 ;
  ssize_t bytesReceived;
  char *recvBuf = (char *)malloc(bufferSize);

  if (recvBuf == NULL) {
    perror("malloc");
    return NULL;
  }

  while (1) {
    if (dataLen + CHUNK_SIZE >= bufferSize) {
      bufferSize += INITIAL_BUFFER_SIZE;
      recvBuf = (char *)realloc(recvBuf, bufferSize + 1);     // +1 just incase for the null character.
      if (recvBuf == NULL) {
        perror("malloc");
        return NULL;
      }
    }

    bytesReceived = recv(sockFD, recvBuf + dataLen, bufferSize, 0);
    if (bytesReceived == -1) {
      perror("recv");
      free(recvBuf);
      return NULL;
    } 
    dataLen += bytesReceived;

    // check for the delimiter character.
    if (!strncmp(&recvBuf[dataLen - 2], "\r\n", 2)) {
      break;
    }
  }
  recvBuf[dataLen] = '\0';

  return recvBuf;
}

message input_message() {
  message input = IM_INIT;
  size_t size = 1;
  int ch;

  input.buffer = (char *)malloc(size);
  if (input.buffer == NULL) {
    perror("malloc");
    free(input.buffer);

    message returnVal = IM_INIT;
    return returnVal;
  }

  while ((ch = getchar()) != '\n' && (ch != EOF)) {
    if (input.len + 1 >= size) {
      size += 1;
      input.buffer = (char *)realloc(input.buffer, size);
      if (input.buffer == NULL) {
        perror("realloc");
        free(input.buffer);
        message returnVal = IM_INIT;
        return returnVal;
      }
    }
    input.buffer[input.len++] = ch;
  }
  input.buffer[input.len++] = '\r';
  input.buffer[input.len++] = '\n';
  // need to end with a null character. Navaye send() garda last byte lai null le replace gardine raixa. syet.
  input.buffer[input.len] = '\0';

  return input;
}
