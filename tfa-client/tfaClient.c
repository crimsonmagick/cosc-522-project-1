#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "pkeMessaging.h"

#define REGISTER_OPTION 1
#define LOGIN_OPTION 2
#define QUIT_OPTION 3

void logError(const char *errorMessage); /* External error handling function */
int getOption();

unsigned int getIntInput(char *inputName);

int registerPublicKey(unsigned int userID, unsigned int publicKey, char *serverIP, unsigned short serverPort);

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
        switch (selected) {
            case REGISTER_OPTION:
                publicKey = getIntInput("public key");
                registerPublicKey(userID, publicKey, servIP, servPort);
                break;
            case LOGIN_OPTION:
                printf("TODO login dialogue\n");
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
    int sock; /* Socket descriptor */
    struct sockaddr_in fromAddr; /* Source address of echo */
    PKServerToPClientOrLodiServer serverMessage;
    const PClientToPKServer clientMessage = {
        registerKey,
        userID,
        publicKey
    };

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        logError("socket() failed");

    /* Construct the server address structure */
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);

    if (sendto(sock, &clientMessage, sizeof(clientMessage), 0, (struct sockaddr *)
               &serverAddr, sizeof(serverAddr)) != sizeof(clientMessage))
        logError("sendto() sent a different number of bytes than expected");

    socklen_t fromSize = sizeof(fromAddr);
    if (recvfrom(sock, &serverMessage, sizeof(serverMessage), 0,
                 (struct sockaddr *) &fromAddr, &fromSize) != sizeof(serverMessage))
        logError("recvfrom() failed");

    if (serverAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        exit(1);
    }

    printf("Received: %u, %u, %u\n", serverMessage.messageType, serverMessage.userID, serverMessage.publicKey);

    close(sock);
    return 0;
}
