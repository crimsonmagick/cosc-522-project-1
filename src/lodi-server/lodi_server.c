#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messaging/lodi_messaging.h"
#include "messaging/udp.h"
#include "shared.h"
#include "messaging/pke_messaging.h"
#include "messaging/tfa_messaging.h"
#include "util/rsa.h"
#include "util/server_configs.h"

int getPublicKey(const unsigned int userID, unsigned int *publicKey) {
  const ServerConfig config = getServerConfig(PK);
  const PClientToPKServer requestMessage = {
    .messageType = requestKey,
    .userID = userID
  };

  char *requestBuffer = serializePKClientRequest(&requestMessage);
  char responseBuffer[PK_SERVER_RESPONSE_SIZE];
  const int sendStatus = synchronousSend(requestBuffer, PK_CLIENT_REQUEST_SIZE, responseBuffer,
                                         PK_SERVER_RESPONSE_SIZE, config.address, atoi(config.port));
  free(requestBuffer);

  if (sendStatus == ERROR) {
    printf("Aborting registration...\n");
  } else {
    PKServerToLodiClient *responseDeserialized = deserializePKServerResponse(
      responseBuffer, PK_SERVER_RESPONSE_SIZE);
    *publicKey = responseDeserialized->publicKey;
    printf("Key retrieval successful! Received: messageType=%u, userID=%u, publicKey=%u\n",
           responseDeserialized->messageType, responseDeserialized->userID, *publicKey);
    free(responseDeserialized);
  }

  return sendStatus;
}

int sendPushRequest(const unsigned int userID) {
  const ServerConfig config = getServerConfig(TFA);
  const TFAClientOrLodiServerToTFAServer requestMessage = {
    .messageType = requestAuth,
    .userID = userID
  };

  char *requestBuffer = serializeTFAClientRequest(&requestMessage);
  char responseBuffer[TFA_CLIENT_REQUEST_SIZE];
  const int sendStatus = synchronousSend(requestBuffer, TFA_CLIENT_REQUEST_SIZE, responseBuffer,
                                         TFA_SERVER_RESPONSE_SIZE, config.address, atoi(config.port));
  free(requestBuffer);

  if (sendStatus == ERROR) {
    printf("Aborting push authentication...\n");
  } else {
    TFAServerToLodiServer *responseDeserialized = deserializeTFAServerLodiResponse(responseBuffer,
      TFA_SERVER_RESPONSE_SIZE);
    printf("Push auth successful! Received: messageType=%u, userID=%u", responseDeserialized->messageType,
           responseDeserialized->userID);
    free(responseDeserialized);
  }

  return sendStatus;
}

int main() {
  const unsigned short serverPort = atoi(getServerConfig(LODI).port);

  const int serverSocket = getServerSocket(serverPort, NULL);
  if (serverSocket < 0) {
    printf("Unable to create socket\n");
    exit(EXIT_FAILURE);
  }

  while (true) {
    struct sockaddr_in clientAddress;

    char receivedBuffer[LODI_CLIENT_REQUEST_SIZE];
    const int receivedSuccess = receiveMessage(serverSocket, receivedBuffer, LODI_CLIENT_REQUEST_SIZE, &clientAddress);

    if (receivedSuccess == ERROR) {
      printf("Failed to handle incoming PClientToLodiServer message.\n");
      continue;
    }

    PClientToLodiServer *receivedMessage = deserializeLodiServerRequest(receivedBuffer, LODI_CLIENT_REQUEST_SIZE);

    unsigned int publicKey;
    bool authenticated = false;
    if (getPublicKey(receivedMessage->userID, &publicKey) == ERROR) {
      printf("Failed to retrieve public key!\n");
    } else {
      const unsigned long decrypted = decryptTimestamp(receivedMessage->digitalSig, publicKey, MODULUS);
      if (decrypted == receivedMessage->timestamp) {
        authenticated = true;
        printf("Decrypted timestamp successfully! timestamp=%lu \n", decrypted);
      } else {
        printf("Failed to decrypt timestamp! timestamp=%lu, decrypted=%lu \n",
               receivedMessage->timestamp, decrypted);
      }
    }

    if (authenticated) {
      printf("Verifying login with TFA...\n");
      if (sendPushRequest(receivedMessage->userID) == ERROR) {
        printf("Failed to authenticate with push confirmation! Continuing without response...\n");
        continue;
      }
    } else {
      printf("Failed to authenticate with public key! Continuing without response...\n");
      continue;
    }

    printf("Validated TFA successfully!\n");

    LodiServerToLodiClientAcks toSendMessage = {
      ackLogin,
      receivedMessage->userID,
    };

    char *sendBuffer = serializeLodiServerResponse(&toSendMessage);

    const int sendSuccess = sendMessage(serverSocket, sendBuffer, LODI_SERVER_RESPONSE_SIZE, &clientAddress);
    if (sendSuccess == ERROR) {
      printf("Error while sending message.\n");
    }

    free(sendBuffer);
    free(receivedMessage);
  }
}
