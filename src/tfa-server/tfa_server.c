#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shared.h"
#include "registration_repository.h"
#include "messaging/tfa_messaging.h"
#include "messaging/pke_messaging.h"
#include "messaging/udp.h"
#include "util/rsa.h"
#include "util/server_configs.h"

int main() {
    init();
    const unsigned short serverPort = atoi(getServerConfig(TFA).port);
    const ServerConfig pkServerConfig = getServerConfig(PK);
    const unsigned int pkPort = atoi(pkServerConfig.port);
    const char * pkAddress = pkServerConfig.address;

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

        // GET PUBLIC KEY
        PClientToPKServer pkRequest = {
            .messageType= requestKey,
            .userID = receivedMessage->userID
        };

        char *pkRequestBuffer = serializePKClientRequest(&pkRequest);
        char pkResponseBuffer[PK_SERVER_RESPONSE_SIZE];
        const int pkSendStatus = synchronousSend(pkRequestBuffer, PK_CLIENT_REQUEST_SIZE, pkResponseBuffer,
                                           PK_SERVER_RESPONSE_SIZE, pkAddress, pkPort);
        free(pkRequestBuffer);

        if (pkSendStatus == ERROR) {
            printf("Error while sending PK public key request message. Continuing...\n");
            continue;
        }

        PKServerToLodiClient *pkResponseDeserialized = deserializePKServerResponse(
            pkResponseBuffer, PK_SERVER_RESPONSE_SIZE);
        printf("Got public key! Received: messageType=%u, userID=%u, publicKey=%u\n",
               pkResponseDeserialized->messageType, pkResponseDeserialized->userID, pkResponseDeserialized->publicKey);
        const unsigned long publicKey = pkResponseDeserialized->publicKey;
        free(pkResponseDeserialized);

        // GOT PUBLIC KEY, NOW AUTHENTICATE
        const unsigned long digitalSig = receivedMessage->digitalSig;
        const unsigned long timestamp = receivedMessage->timestamp;
        const unsigned long decryptedTimesstamp = decryptTimestamp(digitalSig, publicKey, MODULUS);
        if (decryptedTimesstamp != timestamp) {
            printf("Authentication failed! Aborting TFA client registration...\n");
            continue;
        }
        printf("Authentication succeeded! Continuing with TFA client registration!\n");

        // REGISTER CLIENT
        addIP(receivedMessage->userID, clientAddress.sin_addr, ntohs(clientAddress.sin_port));
        printf("Registered client! Sending TFA confirmation message!\n");

        // WE AUTHENTICATED! SEND SUCCESS MESSAGE TO CLIENT
        TFAServerToTFAClient registrationSuccessMessage = {
            confirmTFA,
            receivedMessage->userID,
        };
        char *sendBuffer = serializeTFAServerResponse(&registrationSuccessMessage);

        const int sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_SERVER_RESPONSE_SIZE, &clientAddress);
        if (sendSuccess == ERROR) {
            printf("Error while sending message.\n");
        }

        free(sendBuffer);
        free(receivedMessage);
    }
}
