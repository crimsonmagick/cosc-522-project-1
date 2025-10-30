#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messaging/pke_messaging.h"
#include "messaging/udp.h"
#include "key_repository.h"
#include "shared.h"
#include "util/server_configs.h"


int main() {
  const unsigned short serverPort = atoi(getServerConfig(PK).port);

  init();

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
    PKServerToPClientOrLodiServer responseMessage = {
      receivedMessage->userID,
      receivedMessage->publicKey
    };

    if (receivedMessage->messageType == registerKey) {
      addKey(receivedMessage->userID, receivedMessage->publicKey);
      responseMessage.messageType = ackRegisterKey;
    } else if (receivedMessage->messageType == requestKey) {
      unsigned int publicKey;
      if (getKey(receivedMessage->userID, &publicKey) == ERROR) {
        printf("publicKey=%u not found.\n", receivedMessage->publicKey);
      }
      responseMessage.messageType = responsePublicKey;
    } else {
      printf("Received message with unknown message type.\n");
    }

    char *sendBuffer = serializePKServerResponse(&responseMessage);

    const int sendSuccess = sendMessage(serverSocket, sendBuffer, PK_SERVER_RESPONSE_SIZE, &clientAddress);
    if (sendSuccess == ERROR) {
      printf("Error while sending message.\n");
    }

    free(sendBuffer);
    free(receivedMessage);
  }
}
