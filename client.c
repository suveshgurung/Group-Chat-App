#include "utilities.h"

int main() {

  int clientSocketFD = initial_client_socket_operation();

  printf("Enter text to send to the server, type 'exit' to exit\n\n");
  message input;
  while (1) {
    input = input_message();
    if (input.buffer == NULL) {
      perror("input_message");
      return 7;
    }

    if (!strcmp(input.buffer, "exit\r\n")) {
      break;
    }

    ssize_t bytesSent;
    if ((bytesSent = send(clientSocketFD, input.buffer, input.len, 0)) == -1) {
      perror("send");
      return 5;
    }
  }

  close(clientSocketFD);
  free(input.buffer);

  return 0;
}
