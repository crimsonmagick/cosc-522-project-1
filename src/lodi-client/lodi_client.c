#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>
#include <math.h>
#include <stdbool.h>

#include "messaging/pke_messaging.h"
#include "messaging/lodi_messaging.h"
#include "messaging/udp.h"
#include "util/rsa.h"
#include "util/server_configs.h"

#define REGISTER_OPTION 1
#define LOGIN_OPTION 2
#define QUIT_OPTION 3

int getMainOption();

unsigned long getLongInput(char *inputName);

int registerPublicKey(unsigned int userID, unsigned int publicKey);

int lodiLogin(unsigned int userID, long timestamp, long digitalSignature);


int main() {
    printf("Welcome to the Lodi Client!\n");
    unsigned int userID = getLongInput("user ID");
    printf("Now choose from the following options:\n");

    int selected = 0;
    while (selected != QUIT_OPTION) {
        selected = getMainOption();

        unsigned int publicKey;
        unsigned int privateKey;
        unsigned long timestamp;
        unsigned long digitalSignature;
        switch (selected) {
            case REGISTER_OPTION:
                // TODO look into generating public/private key pair either here or as another option
                publicKey = getLongInput("public key");
                registerPublicKey(userID, publicKey);
                break;
            case LOGIN_OPTION:
                // TODO look into reusing generated private key
                privateKey = getLongInput("private key");
                time(&timestamp);
                digitalSignature = encryptTimestamp(timestamp, privateKey, MODULUS);
                lodiLogin(userID, timestamp, digitalSignature);
                break;
            case QUIT_OPTION:
                printf("See you later!\n");
                break;
            default:
                printf("Please enter a valid option: %d, %d, or %d\n",
                       REGISTER_OPTION, LOGIN_OPTION, QUIT_OPTION);
                break;
        }
    }

    exit(0);
}

int getMainOption() {
    printf("1. Register your Lodi Key\n");
    printf("2. Login to Lodi\n");
    printf("3. Exit\n");

    int selected = 0;
    char line[64];

    if (fgets(line, sizeof(line), stdin)) {
        sscanf(line, "%d", &selected);
    }
    return selected;
}

int getLodiLoopOption() {
    printf("1. Exit to main menu\n");

    int selected = 0;
    char line[64];

    if (fgets(line, sizeof(line), stdin)) {
        sscanf(line, "%d", &selected);
    }
    return selected;
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

int registerPublicKey(const unsigned int userID, const unsigned int publicKey) {
    const ServerConfig config = getServerConfig(PK);
    const PClientToPKServer requestMessage = {
        registerKey,
        userID,
        publicKey
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
        printf("Registration successful! Received: messageType=%u, userID=%u, publicKey=%u\n",
               responseDeserialized->messageType, responseDeserialized->userID, responseDeserialized->publicKey);
        free(responseDeserialized);
    }

    return sendStatus;
}

int lodiLogin(const unsigned int userID, const long timestamp, const long digitalSignature) {
    const ServerConfig config = getServerConfig(LODI);
    const PClientToLodiServer requestMessage = {
        .messageType = login,
        .userID = userID,
        .recipientID = 0,
        .timestamp = timestamp,
        .digitalSig = digitalSignature
    };

    char *requestBuffer = serializeLodiServerRequest(&requestMessage);
    char responseBuffer[LODI_CLIENT_REQUEST_SIZE];
    const int sendStatus = synchronousSend(requestBuffer, LODI_CLIENT_REQUEST_SIZE, responseBuffer,
                                           LODI_SERVER_RESPONSE_SIZE, config.address, atoi(config.port));
    free(requestBuffer);

    if (sendStatus == ERROR) {
        printf("Aborting registration...\n");
    } else {
        LodiServerToLodiClientAcks *responseDeserialized = deserializeLodiServerResponse(
            responseBuffer, LODI_SERVER_RESPONSE_SIZE);
        printf("Login successful! Received: messageType=%u, userID=%u\n",
               responseDeserialized->messageType, responseDeserialized->userID);
        free(responseDeserialized);
    }

    printf("Please select from our many amazing Lodi options:\n");

    int selected = 0;
    while (selected != 1) {
        selected = getLodiLoopOption();

        if (selected != 1) {
            printf("Please enter a valid option: 1\n");
        }
    }
    return sendStatus;
}
