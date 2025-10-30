#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shared.h"
#include "messaging/tfa_messaging.h"
#include "messaging/udp.h"
#include "util/server_configs.h"

int main() {
    const unsigned short serverPort = atoi(getServerConfig(TFA).port);

    const int serverSocket = getServerSocket(serverPort, NULL);
    if (serverSocket < 0) {
        printf("Unable to create socket\n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        struct sockaddr_in clientAddress;

        char receivedBuffer[TFA_CLIENT_REQUEST_SIZE];
        const int receivedSuccess = receiveMessage(serverSocket, receivedBuffer, TFA_CLIENT_REQUEST_SIZE, &clientAddress);

        if (receivedSuccess == ERROR) {
            printf("Failed to handle incoming TFAClientOrLodiServerToTFAServer message.\n");
            continue;
        }

        TFAClientOrLodiServerToTFAServer *receivedMessage = deserializeTFAClientRequest(receivedBuffer, TFA_CLIENT_REQUEST_SIZE);

        // TODO connect to PKE and get publicKey, validate publicKey,
        // TODO connect to TFA server and get two factor auth confirmation

        TFAServerToTFAClient toSendMessage = {
            confirmTFA,
            receivedMessage->userID,
          };

        char* sendBuffer = serializeTFAServerResponse(&toSendMessage);

        const int sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_SERVER_RESPONSE_SIZE, &clientAddress);
        if (sendSuccess == ERROR) {
            printf("Error while sending message.\n");
        }

        free(sendBuffer);
        free(receivedMessage);
    }

}
