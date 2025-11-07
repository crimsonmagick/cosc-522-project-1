/**
* This source file implements the "TFA Client" functionality:
 *
 * 1) Registers clients IP address and port in the TFA Server
 * 2  Responds to push authentication requests sent from the Lodi Server
 **/

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>
#include <stdbool.h>

#include "messaging/tfa_messaging.h"
#include "messaging/udp.h"
#include "shared.h"
#include "util/rsa.h"
#include "util/server_configs.h"

#define REGISTER_OPTION 1
#define LOGIN_OPTION 2
#define QUIT_OPTION 3

int getMainOption();

unsigned long getLongInput(char *inputName);

int registerTFAClient(const unsigned int userID, unsigned long timestamp, unsigned long digitalSignature);

int lodiLogin(unsigned int userID, long timestamp, long digitalSignature);

int handleTFAPush();


int main() {
    printf("Welcome to the TFA Client!\n");
    unsigned int userID = getLongInput("user ID");
    unsigned long privateKey = getLongInput("private key");
    unsigned long timestamp;
    unsigned long digitalSignature;

    time(&timestamp);
    digitalSignature = encryptTimestamp(timestamp, privateKey, MODULUS);

    registerTFAClient(userID, timestamp, digitalSignature);

    while (true) {
        handleTFAPush();
    }

    exit(0);
}

unsigned long getLongInput(char *inputName) {
    long input = -1;
    while (input < 0) {
        printf("Please enter your %s:\n", inputName);
        char line[64];

        if (fgets(line, sizeof(line), stdin)) {
            sscanf(line, "%ld", &input);
            if (sscanf(line, "%d", &input) != 1 || input < 0) {
                printf("Invalid %s entered. Please try again!\n", inputName);
            }
        } else {
            printf("Failed to read user input. Please try again:\n");
        }
    }

    return (unsigned long) input;
}

int handleTFAPush() {
    const unsigned short serverPort = atoi(getServerConfig(TFA_CLIENT).port);

    const int serverSocket = getServerSocket(serverPort, NULL);

    if (serverSocket < 0) {
        printf("Unable to create socket\n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        struct sockaddr_in clientAddress;

        char receivedBuffer[TFA_SERVER_RESPONSE_SIZE];
        const int receivedSuccess = receiveMessage(serverSocket, receivedBuffer, TFA_SERVER_RESPONSE_SIZE, &clientAddress);

        if (receivedSuccess == ERROR) {
            printf("Failed to handle incoming TFAClientOrLodiServerToTFAServer message.\n");
            continue;
        }

        TFAServerToTFAClient *receivedMessage = deserializeTFAServerResponse(receivedBuffer, TFA_SERVER_RESPONSE_SIZE);

        TFAClientOrLodiServerToTFAServer toSendMessage = {
            .messageType = ackPushTFA,
            .userID = receivedMessage->userID,
            .timestamp = 0,
            .digitalSig = 0
          };

        char* sendBuffer = serializeTFAClientRequest(&toSendMessage);

        const int sendSuccess = sendMessage(serverSocket, sendBuffer, TFA_CLIENT_REQUEST_SIZE, &clientAddress);
        if (sendSuccess == ERROR) {
            printf("Error while sending message.\n");
        }

        free(sendBuffer);
        free(receivedMessage);
        printf("Responded to push auth request successfully!\n");
    }
}

int registerTFAClient(const unsigned int userID, unsigned long timestamp, unsigned long digitalSignature) {
    const ServerConfig config = getServerConfig(TFA);
    const ServerConfig clientConfig = getServerConfig(TFA_CLIENT);
    TFAClientOrLodiServerToTFAServer requestMessage = {
        .messageType = registerTFA,
        .userID = userID,
        .timestamp = timestamp,
        .digitalSig = digitalSignature
    };

    char *requestBuffer = serializeTFAClientRequest(&requestMessage);
    char responseBuffer[TFA_SERVER_RESPONSE_SIZE];
    int sendStatus = synchronousSendWithClientPort(requestBuffer, TFA_CLIENT_REQUEST_SIZE, responseBuffer,
                                           TFA_SERVER_RESPONSE_SIZE, config.address, atoi(config.port), atoi(clientConfig.port));
    free(requestBuffer);

    if (sendStatus == ERROR) {
        printf("Aborting registration...\n");
    } else {
        TFAServerToTFAClient *responseDeserialized = deserializeTFAServerResponse(
            responseBuffer, TFA_SERVER_RESPONSE_SIZE);
        printf("TFA registration successful! Received: messageType=%u, userID=%u\n",
               responseDeserialized->messageType, responseDeserialized->userID);
        free(responseDeserialized);
    }

    requestMessage.messageType = ackRegTFA;
    requestBuffer = serializeTFAClientRequest(&requestMessage);
    sendStatus = synchronousSendWithClientPort(requestBuffer, TFA_CLIENT_REQUEST_SIZE, responseBuffer,
                                           TFA_SERVER_RESPONSE_SIZE, config.address, atoi(config.port), atoi(clientConfig.port));
    if (sendStatus == ERROR) {
        printf("Key registration failed in final step...\n");
    } else {
        printf("Key registration successful!\n");
    }
    free(requestBuffer);

    return sendStatus;
}
