#include "utilities.h"
#include <time.h>

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
  // close the socket to say that sending is done.
  close(clientSocketFD);

  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 1000000;
  nanosleep(&ts, NULL);

  int newClientSocketFD = initial_client_socket_operation();

  char buf[1024];
  if (recv(newClientSocketFD, buf, 1024, 0) == -1) {
    perror("recv");
    return 6;
  }

  printf("%s\n", buf);

  close(newClientSocketFD);
  free(input.buffer);

  return 0;
}
