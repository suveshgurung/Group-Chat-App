#include "utilities.h"
#include <signal.h>

int serverSocketFD;
int clientSocketFD;
char *recvBuf;

void signalHandler(int sig);

int main() {
  
  clientSocketFD = initial_server_socket_operation(&serverSocketFD);

  signal(SIGINT, signalHandler);

  while (1) {
    if ((recvBuf = recv_data(clientSocketFD)) == NULL) {
      perror("recvBuffer");
      return 5;
    }
    printf("%s", recvBuf);

    // char *sendBuf = "Hello from the server!";
    // if (send(clientSocketFD, sendBuf, strlen(sendBuf), 0) == -1) {
    //   perror("send");
    //   return 6;
    // }
  }

  close(clientSocketFD);
  close(serverSocketFD);
  free(recvBuf);

  return 0;
}

void signalHandler(int sig) {
  close(serverSocketFD);
  close(clientSocketFD);
  free(recvBuf);

  exit(0);
}
