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

  close(serverSocketFD);
  close(clientSocketFD);

  int newServerSocketFD;
  int newClientSocketFD = initial_server_socket_operation(&newServerSocketFD);

  char *sendBuf = "Hello from the server!";
  if (send(newClientSocketFD, sendBuf, strlen(sendBuf), 0) == -1) {
    perror("send");
    return 6;
  }

  close(newServerSocketFD);
  close(clientSocketFD);
  free(recvBuf);

  return 0;
}
