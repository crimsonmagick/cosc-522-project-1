#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>
#include <math.h>
#include <stdbool.h>

#include "messaging/tfa_messaging.h"
#include "messaging/udp.h"
#include "util/rsa.h"
#include "util/server_configs.h"

#define REGISTER_OPTION 1
#define LOGIN_OPTION 2
#define QUIT_OPTION 3

int getMainOption();

unsigned long getLongInput(char *inputName);

int registerTFAClient(const unsigned int userID, unsigned long timestamp, unsigned long digitalSignature);

int lodiLogin(unsigned int userID, long timestamp, long digitalSignature);


int main() {
    printf("Welcome to the TFA Client!\n");
    unsigned int userID = getLongInput("user ID");
    unsigned long privateKey = getLongInput("private key");
    unsigned long timestamp;
    unsigned long digitalSignature;

    time(&timestamp);
    digitalSignature = encryptTimestamp(timestamp, privateKey, MODULUS);

    registerTFAClient(userID, timestamp, digitalSignature);

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

int registerTFAClient(const unsigned int userID, unsigned long timestamp, unsigned long digitalSignature) {
    const ServerConfig config = getServerConfig(TFA);
    const TFAClientOrLodiServerToTFAServer requestMessage = {
        userID,
        timestamp,
        digitalSignature
    };

    char *requestBuffer = serializeTFAClientRequest(&requestMessage);
    char responseBuffer[TFA_SERVER_RESPONSE_SIZE];
    const int sendStatus = synchronousSend(requestBuffer, TFA_CLIENT_REQUEST_SIZE, responseBuffer,
                                           TFA_SERVER_RESPONSE_SIZE, config.address, atoi(config.port));
    free(requestBuffer);

    if (sendStatus == ERROR) {
        printf("Aborting registration...\n");
    } else {
        TFAServerToTFAClient *responseDeserialized = deserializeTFAServerResponse(
            responseBuffer, TFA_SERVER_RESPONSE_SIZE);
        printf("TFA registration successful! Received: messageType=%u, userID=%u",
               responseDeserialized->messageType, responseDeserialized->userID);
        free(responseDeserialized);
    }

    return sendStatus;
}
