#include "utilities.h"

int main() {

  int clientSocketFD = initial_client_socket_operation();

  message input = input_message();
  if (input.buffer == NULL) {
    perror("input_message");
    return 7;
  }

  ssize_t bytesSent;
  if ((bytesSent = send(clientSocketFD, input.buffer, input.len, 0)) == -1) {
    perror("send");
    return 5;
  }

  char buf[1024];
  if (recv(clientSocketFD, buf, 1024, 0) == -1) {
    perror("recv");
    return 6;
  }

  printf("%s\n", buf);

  close(clientSocketFD);
  free(input.buffer);

  return 0;
}
