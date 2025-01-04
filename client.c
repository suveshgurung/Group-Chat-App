#include "utilities.h"

#define CN_INIT {NULL, 0}

typedef struct {
  char *name;
  int nameLength;
} clientName;

#define NAME_BUFFER_SIZE 100

int main() {

  int clientSocketFD = initial_client_socket_operation();
  clientName client = CN_INIT;
  client.name = (char *)malloc(NAME_BUFFER_SIZE);

  if (client.name == NULL) {
    perror("malloc");
    return 1;
  }

  int ch;
  ssize_t nameSize = NAME_BUFFER_SIZE;

  printf("Enter your name : ");
  while ((ch = getchar()) != '\n' && (ch != EOF)) {
    if (client.nameLength + NAME_BUFFER_SIZE >= nameSize) {
      nameSize += NAME_BUFFER_SIZE;
      client.name = (char *)realloc(client.name, nameSize);
      if (client.name == NULL) {
        perror("realloc");
        free(client.name);
        return 1;
      }
    }
    client.name[client.nameLength++] = ch;
  }
  // deliiter characters at the end to represent a name is being sent to the socket.
  client.name[client.nameLength++] = '\r';
  client.name[client.nameLength++] = '\n';
  client.name[client.nameLength] = '\0';

  if (send(clientSocketFD, client.name, client.nameLength, 0) == -1) {
    perror("send");
    return 5;
  }

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
  free(client.name);

  return 0;
}
