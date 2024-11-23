#include "utilities.h"
#include <signal.h>
#include <pthread.h>

#define NUM_OF_THREADS 100    // number of users that can connect to the server.

volatile int shutdownRequested = 0;

void signalHandler(int sig);
void *handleClientMessages(void *clientSocketFD);

int main() {

  int serverSocketFD;
  struct sockaddr_in address = initial_server_socket_operation(&serverSocketFD);

  signal(SIGINT, signalHandler);

  struct sockaddr_in clientSocketAddr;
  socklen_t clientAddrLen = sizeof(clientSocketAddr);

  int threadIndex = 0;
  pthread_t threadId[NUM_OF_THREADS];
  long clientSocketFDArr[NUM_OF_THREADS];

  while (!shutdownRequested) {
    // TODO: SIGINT pathauda accept is blocking. need to see this.
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&address, &clientAddrLen);
    if (clientSocketFD == -1) {
      perror("accept");
      exit(4);
    }

    // create a new thread to handle the new client.
    clientSocketFDArr[threadIndex] = clientSocketFD;

    if (pthread_create(&threadId[threadIndex], NULL, handleClientMessages, (void *)clientSocketFDArr[threadIndex])) {
      fprintf(stderr, "error in pthread_create\n");
      return 6;
    }

    threadIndex++;
  }

  // wait for all threads to terminate.
  for (int i = 0; i < threadIndex; i++) {
    pthread_join(threadId[i], NULL);
  }

  shutdown(serverSocketFD, SHUT_RDWR);

  return 0;
}

void signalHandler(int sig) {
  shutdownRequested = 1;
}

void *handleClientMessages(void *clientSocketFD) {
  long socketFD = (long) clientSocketFD;
  char *recvBuf;

  while (!shutdownRequested) {
    recvBuf = recv_data(socketFD);
    if (recvBuf == NULL) {
      perror("recvBuffer");
      return NULL;
    } else if (!strncmp(recvBuf, "EOF", 3)) {
      break;
    }

    printf("%s", recvBuf);
  }

  close(socketFD);
  free(recvBuf);
  pthread_exit(NULL);
}
