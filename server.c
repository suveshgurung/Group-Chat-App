#include "utilities.h"
#include <signal.h>
#include <pthread.h>

typedef struct {
  pthread_t threadId;
  int clientSocketFD;
} thread_args;

int serverSocketFD;
int clientSocketFD;
char *recvBuf;
thread_args *threadArgs = NULL;

void signalHandler(int sig);
void *handleClientMessages(void *clientSocketFD);

int main() {

  struct sockaddr_in address = initial_server_socket_operation(&serverSocketFD);

  signal(SIGINT, signalHandler);

  struct sockaddr_in clientSocketAddr;
  socklen_t clientAddrLen = sizeof(clientSocketAddr);

  int numOfThreads = 1;

  while (1) {
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&address, &clientAddrLen);
    if (clientSocketFD == -1) {
      perror("accept");
      exit(4);
    } else {
      // create a new thread to handle the new client.
      threadArgs = (thread_args *)realloc(threadArgs, numOfThreads * sizeof(thread_args));
      if (threadArgs == NULL) {
        perror("malloc");
        return 7;
      }

      threadArgs[numOfThreads - 1].clientSocketFD = clientSocketFD;
      printf("clientSocketFD: %d\n", threadArgs[numOfThreads - 1].clientSocketFD);
      printf("numOfThreads: %d\n", numOfThreads);

      if (pthread_create(&threadArgs[numOfThreads - 1].threadId, NULL, handleClientMessages, &threadArgs[numOfThreads - 1].clientSocketFD)) {
        fprintf(stderr, "error in pthread_create\n");
        return 6;
      }

      numOfThreads++;
    }
  }

  close(serverSocketFD);
  close(clientSocketFD);
  free(recvBuf);
  free(threadArgs);

  return 0;
}

void signalHandler(int sig) {
  close(serverSocketFD);
  close(clientSocketFD);
  free(recvBuf);
  free(threadArgs);

  exit(0);
}

void *handleClientMessages(void *clientSocketFD) {
  while (1) {
    if ((recvBuf = recv_data(*(int*)clientSocketFD)) == NULL) {
      perror("recvBuffer");
      return NULL;
    }

    printf("%s", recvBuf);
  }
}
