#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "messaging.h"

void DieWithError(char *errorMessage); /* External error handling function */

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    unsigned short serverPort = atoi(argv[1]);

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort);

    if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
        DieWithError("bind() failed");

    while (true) {
        PClientToPKServer receivedMessage;

        /* Block until receive message from a client */
        clientAddrLen = sizeof(clientAddr);
        if (recvfrom(sock, &receivedMessage, sizeof(receivedMessage), 0,
                     (struct sockaddr *) &clientAddr, &clientAddrLen) < 0)
            DieWithError("recvfrom() failed");

        char clientAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientAddress, INET_ADDRSTRLEN);
        printf("Handling client %s\n", clientAddress);

        PKServerToPClientOrLodiServer toSendMessage = {
            ackRegisterKey,
            receivedMessage.userID,
            receivedMessage.publicKey
        };

        if (sendto(sock, &toSendMessage, sizeof(toSendMessage), 0,
                   (struct sockaddr *) &clientAddr, sizeof(clientAddr)) != sizeof(toSendMessage)) {
            DieWithError("sendto() sent a different number of bytes than expected");
        }
    }
}
