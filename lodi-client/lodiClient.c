#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define ECHOMAX 255     /* Longest string to echo */

#define REGISTER_OPTION 1
#define LOGIN_OPTION 2
#define QUIT_OPTION 3

void DieWithError(char *errorMessage); /* External error handling function */
int getOption();

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>]\n", argv[0]);
        exit(1);
    }

    unsigned short echoServPort; /* Echo server port */

    if (argc == 3)
        echoServPort = atoi(argv[2]); /* Use given port, if any */
    else
        echoServPort = 7;

    printf("Welcome to the Lodi Client! Please choose from the following options:\n");


    int selected = 0;
    while (selected != QUIT_OPTION) {
        selected = getOption();

        switch (selected) {
            case REGISTER_OPTION:
                printf("TODO register key dialogue\n");
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

int registerPublicKey(char *serverIP, unsigned short serverPort, char *key) {
    int sock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr; /* Source address of echo */
    unsigned int fromSize; /* In-out of address size for recvfrom() */
    char echoBuffer[ECHOMAX + 1]; /* Buffer for receiving echoed string */
    int echoStringLen; /* Length of string to echo */
    int respStringLen; /* Length of received response */

    if ((echoStringLen = strlen(key)) > ECHOMAX) /* Check input length */
        DieWithError("Echo word too long");

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(serverIP); /* Server IP address */
    echoServAddr.sin_port = htons(serverPort); /* Server port */

    /* Send the string to the server */
    if (sendto(sock, key, echoStringLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
        DieWithError("sendto() sent a different number of bytes than expected");

    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                                  (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
        DieWithError("recvfrom() failed");

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        exit(1);
    }

    /* null-terminate the received data */
    echoBuffer[respStringLen] = '\0';
    printf("Received: %s\n", echoBuffer); /* Print the echoed arg */

    close(sock);
    return 0;
}
