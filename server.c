#include "utilities.h"

int main() {
  
  int serverSocketFD;
  int clientSocketFD = initial_server_socket_operation(&serverSocketFD);

  char *recvBuf;
  if ((recvBuf = recv_data(clientSocketFD)) == NULL) {
    perror("recvBuffer");
    return 5;
  }
  printf("%s\n", recvBuf);

  char *sendBuf = "Hello from the server!";
  if (send(clientSocketFD, sendBuf, strlen(sendBuf), 0) == -1) {
    perror("send");
    return 6;
  }

  close(serverSocketFD);
  close(clientSocketFD);
  free(recvBuf);

  return 0;
}
