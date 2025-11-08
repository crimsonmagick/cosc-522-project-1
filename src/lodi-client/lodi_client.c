/**
 * This source file implements the "Lodi Client" functionality:
 *
 * 1) Registers user's public key against the PKE Server.
 *    See registerPublicKey() for the implementation of this functionality
 * 2) Authenticates against the Lodi Server
 *    See lodiLogin() for the implementation of this functionality.
 **/

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>

#include "messaging/pke_messaging.h"
#include "messaging/lodi_messaging.h"
#include "messaging/udp.h"
#include "shared.h"
#include "util/rsa.h"
#include "util/server_configs.h"

#define REGISTER_OPTION 1
#define LOGIN_OPTION 2
#define QUIT_OPTION 3

static DomainServiceHandle *pkeDomain = NULL;
static DomainServiceHandle *lodiDomain= NULL;
static struct sockaddr_in pkServerAddr;
static struct sockaddr_in lodiServerAddr;

int getMainOption();

unsigned long getLongInput(char *inputName);

int registerPublicKey(unsigned int userID, unsigned int publicKey);

int lodiLogin(unsigned int userID, long timestamp, long digitalSignature);

int main() {
    // initialize domains
    initPKEClientDomain(&pkeDomain);
    initLodiClientDomain(&lodiDomain);
    pkServerAddr = getServerAddr(PK);
    lodiServerAddr = getServerAddr(LODI);

    printf("Welcome to the Lodi Client!\n");
    unsigned int userID = getLongInput("user ID");
    printf("Now choose from the following options:\n");

    int selected = 0;
    while (selected != QUIT_OPTION) {
        selected = getMainOption();

        unsigned long publicKey;
        unsigned long privateKey;
        unsigned long timestamp;
        unsigned long digitalSignature;
        unsigned long decrypted = 0;
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
    const PClientToPKServer requestMessage = {
        registerKey,
        userID,
        publicKey
    };

    if (toDomainHost(pkeDomain, (void *) &requestMessage, &pkServerAddr) == DOMAIN_FAILURE) {
        printf("Unable to send registration, aborting ...\n");
        return ERROR;
    }

    PKServerToLodiClient responseMessage;

    struct sockaddr_in receiveAddress;
    if (fromDomainHost(pkeDomain, &responseMessage, &receiveAddress) == DOMAIN_FAILURE) {
        printf("Failed to receive registration confirmation, aborting ...\n");
        return ERROR;
    }

    printf("Registration successful! Received: messageType=%u, userID=%u, publicKey=%u\n",
           responseMessage.messageType, responseMessage.userID, responseMessage.publicKey);

    return SUCCESS;
}

int lodiLogin(const unsigned int userID, const long timestamp, const long digitalSignature) {
    const PClientToLodiServer request = {
        .messageType = login,
        .userID = userID,
        .recipientID = 0,
        .timestamp = timestamp,
        .digitalSig = digitalSignature
    };

    if (toDomainHost(lodiDomain, (void *) &request, &lodiServerAddr) == ERROR) {
        printf("Failed to send login message, aborting...\n");
        return ERROR;
    }

    struct sockaddr_in receiveAddress;

    LodiServerToLodiClientAcks response;

    if (fromDomainHost(lodiDomain, &response, &receiveAddress) == DOMAIN_FAILURE) {
        printf("Failed to receive login message, aborting...\n");
        return ERROR;
    }

    printf("Login successful! Received: messageType=%u, userID=%u\n",
           response.messageType, response.userID);

    printf("Please select from our many amazing Lodi options:\n");

    int selected = 0;
    while (selected != 1) {
        selected = getLodiLoopOption();

        if (selected != 1) {
            printf("Please enter a valid option: 1\n");
        }
    }
    return SUCCESS;
}
