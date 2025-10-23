#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>

#include "messaging/pke_messaging.h"
#include "messaging/lodi_messaging.h"
#include "messaging/udp.h"
#include "logging/logging.h"

#define REGISTER_OPTION 1
#define LOGIN_OPTION 2
#define QUIT_OPTION 3

int getOption();

unsigned int getIntInput(char *inputName);

long encryptTimestamp(long timestamp, unsigned int privateKey);

int registerPublicKey(unsigned int userID, unsigned int publicKey, char *serverIP, unsigned short serverPort);

void lodiLogin(unsigned int userID, long timestamp, long digitalSignature, char *servIP, unsigned short servPort);

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>]\n", argv[0]);
        exit(1);
    }

    char *servIP = argv[1];

    unsigned short servPort; /* Echo server port */

    if (argc == 3)
        servPort = atoi(argv[2]); /* Use given port, if any */
    else
        servPort = 7;

    printf("Welcome to the Lodi Client!\n");
    unsigned int userID = getIntInput("user ID");
    printf("Now choose from the following options:\n");

    int selected = 0;
    while (selected != QUIT_OPTION) {
        selected = getOption();

        unsigned int publicKey;
        unsigned int privateKey;
        long timestamp;
        long digitalSignature;
        switch (selected) {
            case REGISTER_OPTION:
                publicKey = getIntInput("public key");
                registerPublicKey(userID, publicKey, servIP, servPort);
                break;
            case LOGIN_OPTION:
                privateKey = getIntInput("private key");
                time(&timestamp);
                digitalSignature = encryptTimestamp(timestamp, privateKey);
                lodiLogin(userID, timestamp, digitalSignature, servIP, 9092);
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

long encryptTimestamp(long timestamp, unsigned int privateKey) {
    // TODO encrpytion nonesense
    return timestamp + 1;
}

int getOption() {
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

unsigned int getIntInput(char *inputName) {
    int input = -1;
    while (input < 0) {
        printf("Please enter your %s:\n", inputName);
        char line[64];

        if (fgets(line, sizeof(line), stdin)) {
            sscanf(line, "%d", &input);
            if (sscanf(line, "%d", &input) != 1 || input < 0) {
                printf("Invalid %s entered. Please try again!\n", inputName);
            }
        } else {
            printf("Failed to read user input. Please try again:\n");
        }
    }

    return (unsigned int) input;
}

int registerPublicKey(unsigned int userID, unsigned int publicKey, char *serverIP, unsigned short serverPort) {
    const PClientToPKServer requestMessage = {
        registerKey,
        userID,
        publicKey
    };

    char *requestBuffer = serializePKClientRequest(&requestMessage);
    char responseBuffer[PK_SERVER_RESPONSE_SIZE];
    const int sendStatus = synchronousSend(requestBuffer, PK_CLIENT_REQUEST_SIZE, responseBuffer,
                                           PK_SERVER_RESPONSE_SIZE, serverIP, serverPort);
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

void lodiLogin(unsigned int userID, long timestamp, long digitalSignature, char *servIP, unsigned short servPort) {
    const PClientToLodiServer clientMessage = {
        registerKey,
        userID,
        0,
        timestamp,
        digitalSignature
    };

    char *requestSerialized = malloc(32);
    char *responseSerialized = malloc(8);
    // serializePClientToLodiServerRequest(clientMessage, requestSerialized);
    // sendAndReceiveMessage(requestSerialized, responseSerialized, 32, 8,
                          // servIP, servPort);
    PKServerToLodiClient responseDeserialized;
    // deserializePKServerResponse(responseSerialized, &responseDeserialized);
    printf("Registration successful! Received: messageType=%u, userID=%u, publicKey=%u\n",
           responseDeserialized.messageType, responseDeserialized.userID, responseDeserialized.publicKey);
    free(requestSerialized);
    free(responseSerialized);
}
