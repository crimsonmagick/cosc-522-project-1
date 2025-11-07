#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "shared.h"
#include "registration_repository.h"
#include "messaging/tfa_messaging.h"
#include "messaging/pke_messaging.h"
#include "messaging/udp.h"
#include "util/rsa.h"
#include "util/server_configs.h"

static DomainServiceHandle *pkeDomain = NULL;
static struct sockaddr_in pkServerAddr;

int main() {
    // initialize domains
    initPKEClientDomain(&pkeDomain);
    pkServerAddr = getServerAddr(PK);
    // initialize repository
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
        int receivedSuccess = receiveMessage(serverSocket, receivedBuffer, TFA_CLIENT_REQUEST_SIZE,
                                             &clientAddress);

        if (receivedSuccess == ERROR) {
            printf("Failed to handle incoming TFAClientOrLodiServerToTFAServer message.\n");
            continue;
        }


        TFAClientOrLodiServerToTFAServer *receivedMessage = deserializeTFAClientRequest(
            receivedBuffer, TFA_CLIENT_REQUEST_SIZE);

        if (receivedMessage->messageType == registerTFA) {
            unsigned int publicKey;
            getPublicKey(pkeDomain, &pkServerAddr, receivedMessage->userID, &publicKey);

            // GOT PUBLIC KEY, NOW AUTHENTICATE
            const unsigned long digitalSig = receivedMessage->digitalSig;
            const unsigned long timestamp = receivedMessage->timestamp;
            const unsigned long decryptedTimesstamp = decryptTimestamp(digitalSig, publicKey, MODULUS);
            if (decryptedTimesstamp != timestamp) {
                printf("Authentication failed! Aborting TFA client registration...\n");
                continue;
            }
            printf("Authentication succeeded! Continuing with TFA client registration!\n");

            // WE AUTHENTICATED! SEND SUCCESS MESSAGE TO CLIENT
            TFAServerToTFAClient registrationSuccessMessage = {
                confirmTFA,
                receivedMessage->userID,
            };
            char *sendBuffer = serializeTFAServerResponse(&registrationSuccessMessage);

            int sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_SERVER_RESPONSE_SIZE, &clientAddress);

            free(sendBuffer);
            free(receivedMessage);

            if (sendSuccess == ERROR) {
                printf("Error while sending message.\n");
                continue;
            }

            // RECEIVE ACK MESSAGE OF DESTINY
            receivedSuccess = receiveMessage(serverSocket, receivedBuffer, TFA_CLIENT_REQUEST_SIZE,
                                             &clientAddress);

            if (receivedSuccess == ERROR) {
                printf("Failed to handle incoming ACK TFAClientOrLodiServerToTFAServer message.\n");
                continue;
            }

            receivedMessage = deserializeTFAClientRequest(receivedBuffer, TFA_CLIENT_REQUEST_SIZE);
            if (receivedMessage->messageType != ackRegTFA) {
                printf("Did not receive expected ack register message, aborting registration...\n");
                free(receivedMessage);
                continue;
            }
            printf("Received expected ack register message! Finishing registration.\n");

            // REGISTER CLIENT
            addIP(receivedMessage->userID, clientAddress.sin_addr, ntohs(clientAddress.sin_port));
            printf("Registered client! Sending TFA confirmation message!\n");

            sendBuffer = serializeTFAServerResponse(&registrationSuccessMessage);

            sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_SERVER_RESPONSE_SIZE, &clientAddress);

            free(sendBuffer);
            if (sendSuccess == ERROR) {
                printf("Error while sending final ack message for client registration.\n");
            }
        } else if (receivedMessage->messageType == requestAuth) {
            struct in_addr registeredAddress;
            unsigned short port;
            if (getIP(receivedMessage->userID, &registeredAddress, &port) == ERROR) {
                printf("IP Address was not registered with TFA server! Aborting auth request...\n");
                continue;
            }

            TFAServerToTFAClient pushRequest = {
                .messageType = pushTFA,
                receivedMessage->userID,
            };


            struct sockaddr_in tfaClientAddr;
            memset(&tfaClientAddr, 0, sizeof(tfaClientAddr));
            tfaClientAddr.sin_family = AF_INET;
            tfaClientAddr.sin_addr = registeredAddress;
            tfaClientAddr.sin_port = htons(port);

            char *sendBuffer = serializeTFAServerResponse(&pushRequest);
            int sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_SERVER_RESPONSE_SIZE, &tfaClientAddr);
            if (sendSuccess == ERROR) {
                printf("Failed to send push auth request to TFA client, aborting...\n");
                continue;
            }

            receivedSuccess = receiveMessage(serverSocket, receivedBuffer, TFA_CLIENT_REQUEST_SIZE,
                                             &tfaClientAddr);
            free(sendBuffer);
            if (receivedSuccess == ERROR) {
                printf("Error while receiving TFA client push auth message.\n");
                continue;
            }

            TFAClientOrLodiServerToTFAServer *tfaClientResponse = deserializeTFAClientRequest(
                receivedBuffer, TFA_CLIENT_REQUEST_SIZE);
            if (tfaClientResponse->messageType != ackPushTFA) {
                printf("Did not receive expected ack push message, aborting registration...\n");
                continue;
            }

            TFAServerToLodiServer pushNotificationResponse = {
                responseAuth,
                receivedMessage->userID
            };
            sendBuffer = serializeTFAServerLodiResponse(&pushNotificationResponse);
            sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_SERVER_RESPONSE_SIZE, &clientAddress);
            if (sendSuccess == ERROR) {
                printf("Error while sending push response to Lodi server\n");
            }

            free(sendBuffer);
        }

        free(receivedMessage);
    }
}
