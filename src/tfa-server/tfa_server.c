#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shared.h"
#include "registration_repository.h"
#include "messaging/tfa_messaging.h"
#include "messaging/udp.h"
#include "util/server_configs.h"

int main() {
    init();
    const unsigned short serverPort = atoi(getServerConfig(TFA).port);

    const int serverSocket = getServerSocket(serverPort, NULL);
    if (serverSocket < 0) {
        printf("Unable to create socket\n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        struct sockaddr_in clientAddress;

        char receivedBuffer[TFA_CLIENT_REQUEST_SIZE];
        const int receivedSuccess = receiveMessage(serverSocket, receivedBuffer, TFA_CLIENT_REQUEST_SIZE,
                                                   &clientAddress);

        if (receivedSuccess == ERROR) {
            printf("Failed to handle incoming TFAClientOrLodiServerToTFAServer message.\n");
            continue;
        }

        TFAClientOrLodiServerToTFAServer *receivedMessage = deserializeTFAClientRequest(
            receivedBuffer, TFA_CLIENT_REQUEST_SIZE);


        TFAServerToTFAClient toSendMessage = {
            confirmTFA,
            receivedMessage->userID,
        };

        addIP(receivedMessage->userID, clientAddress.sin_addr);
        struct in_addr testRetrieve;
        getIP(receivedMessage->userID, &testRetrieve);

        char *sendBuffer = serializeTFAServerResponse(&toSendMessage);

        const int sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_SERVER_RESPONSE_SIZE, &clientAddress);
        if (sendSuccess == ERROR) {
            printf("Error while sending message.\n");
        }

        free(sendBuffer);
        free(receivedMessage);
    }
}
