#include "utilities.h"
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_OF_THREADS 100    // number of users that can connect to the server.

typedef struct {
  struct sockaddr_in address;
  int serverSocketFD;
} server_details;

volatile bool shutdownRequested = false;
volatile bool newClientConnection = false;

int threadIndex = 0;
pthread_t threadId[NUM_OF_THREADS];
long clientSocketFDArr[NUM_OF_THREADS];

void signalHandler(int sig);
void *handleClientMessages(void *clientSocketFD);
void *run_accept_thread(void *arg);

int main() {

  server_details server;
  server.address = initial_server_socket_operation(&server.serverSocketFD);

  signal(SIGINT, signalHandler);

  pthread_t accpetRunThreadId;
  pthread_create(&accpetRunThreadId, NULL, run_accept_thread, (void *)&server);

  while (!shutdownRequested) {
    if (newClientConnection) {
      // create a new thread to handle the new client.
      if (pthread_create(&threadId[threadIndex], NULL, handleClientMessages, (void *)clientSocketFDArr[threadIndex])) {
        fprintf(stderr, "error in pthread_create\n");
        return 6;
      }

      threadIndex++;
      newClientConnection = false;
    }
  }

  // wait for all threads to terminate.
  for (int i = 0; i < threadIndex; i++) {
    pthread_join(threadId[i], NULL);
  }

  shutdown(server.serverSocketFD, SHUT_RDWR);

  return 0;
}

void signalHandler(int sig) {
  shutdownRequested = true;
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

void *run_accept_thread(void *arg) {
  server_details *serverDetails = (server_details *)(arg);

  struct sockaddr_in clientSocketAddr;
  socklen_t clientAddrLen = sizeof(clientSocketAddr);

  while (true) {
    int clientSocketFD = accept(serverDetails->serverSocketFD, (struct sockaddr *)&serverDetails->address, &clientAddrLen);
    if (clientSocketFD == -1) {
      perror("accept");
      exit(4);
    }

    newClientConnection = true;

    clientSocketFDArr[threadIndex] = clientSocketFD;
  }

  return NULL;
}
