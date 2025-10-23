#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messaging/pke_messaging.h"
#include "messaging/udp.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <UDP SERVER PORT>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const unsigned short serverPort = atoi(argv[1]);
  const int serverSocket = getServerSocket(serverPort, NULL);
  if (serverSocket < 0) {
    printf("Unable to create socket\n");
    exit(EXIT_FAILURE);
  }

  while (true) {
    struct sockaddr_in clientAddress;

    char receivedBuffer[PK_CLIENT_REQUEST_SIZE];
    const int receivedSuccess = receiveMessage(serverSocket, receivedBuffer, PK_CLIENT_REQUEST_SIZE, &clientAddress);

    if (receivedSuccess == ERROR) {
      printf("Failed to handle incoming PClientToPKServer message.\n");
      continue;
    }

    PClientToPKServer *receivedMessage = deserializePKClientRequest(receivedBuffer, PK_CLIENT_REQUEST_SIZE);

    PKServerToPClientOrLodiServer toSendMessage = {
      ackRegisterKey,
      receivedMessage->userID,
      receivedMessage->publicKey
    };

    char* sendBuffer = serializePKServerResponse(&toSendMessage);

    const int sendSuccess = sendMessage(serverSocket, sendBuffer, PK_SERVER_RESPONSE_SIZE, &clientAddress);
    if (sendSuccess == ERROR) {
      printf("Error while sending message.\n");
    }

    free(sendBuffer);
    free(receivedMessage);
  }
}
