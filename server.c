#include "utilities.h"
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define NUM_OF_THREADS 100    // number of users that can connect to the server.
#define TIME_BUFFER_SIZE 26

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
  pthread_create(&accpetRunThreadId, NULL, &run_accept_thread, (void *)&server);

  while (!shutdownRequested) {
    if (newClientConnection) {
      // create a new thread to handle the new client.
      if (pthread_create(&threadId[threadIndex], NULL, &handleClientMessages, (void *)clientSocketFDArr[threadIndex])) {
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
  char *clientName;
  bool firstTimeRunning = true;
  time_t msgReceivedTime;
  struct tm *convertedTime;
  char timeBuffer[TIME_BUFFER_SIZE];

  while (!shutdownRequested) {
    recvBuf = recv_data(socketFD);
    if (recvBuf == NULL) {
      perror("recvBuffer");
      return NULL;
    } else if (!strncmp(recvBuf, "EOF", 3)) {
      break;
    }

    // name is sent in the first iteration.
    if (firstTimeRunning) {
      size_t nameLen = strlen(recvBuf);
      clientName = (char *)malloc(nameLen - 1);
      memcpy(clientName, recvBuf, nameLen - 2);       // -2 to trim off the \r\n in the end.
      clientName[nameLen - 1] = '\0';

      firstTimeRunning = false;
      continue;
    }

    // take the time when the message was sent.
    msgReceivedTime = time(NULL);
    if (msgReceivedTime == -1) {
      perror("time");
      return NULL;
    }
    convertedTime = localtime(&msgReceivedTime);
    if (convertedTime == NULL) {
      perror("localtime");
      return NULL;
    }
    if (strftime(timeBuffer, TIME_BUFFER_SIZE, "%c", convertedTime) == 0) {
      fprintf(stderr, "strftime: Length of string exceeds the buffer size.\n");
      return NULL;
    }

    FILE *fp = fopen("server-log.txt", "a");
    if (fp == NULL) {
      perror("fopen");
      exit(1);
    }
    fprintf(fp, "%s -> %s : %s", timeBuffer, clientName, recvBuf);

    printf("%s -> %s : %s", timeBuffer, clientName, recvBuf);
    
    if (fclose(fp)) {
      perror("fclose");
      exit(1);
    }
  }

  close(socketFD);
  free(recvBuf);
  free(clientName);
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
