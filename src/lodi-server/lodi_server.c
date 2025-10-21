#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "logging/logging.h"

#include "messaging/lodi_messaging.h"

int main(int argc, char *argv[]) {
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    const unsigned short serverPort = atoi(argv[1]);
    const int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        logError("socket() failed");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort);

    if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        logError("bind() failed");
        exit(1);
    }

    while (true) {
        PClientToLodiServer receivedMessage;

        size_t sizeOfReceivedMessage = sizeof(receivedMessage);
        socklen_t clientAddrLen = sizeof(clientAddr);
        if (recvfrom(sock, &receivedMessage, sizeOfReceivedMessage, 0,
                     (struct sockaddr *) &clientAddr, &clientAddrLen) < 0) {
            logError("recvfrom() failed");
            continue;
                     }

        char clientAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientAddress, INET_ADDRSTRLEN);
        printf("Handling client %s\n", clientAddress);

        // TODO connect to PKE and get publicKey, validate publicKey,
        // TODO connect to TFA server and get two factor auth confirmation

        LodiServerToLodiClientAcks toSendMessage = {
            ackLogin,
           receivedMessage.userID
        };

        if (sendto(sock, &toSendMessage, sizeof(toSendMessage), 0,
                   (struct sockaddr *) &clientAddr, sizeof(clientAddr)) != sizeof(toSendMessage)) {
            logError("sendto() sent a different number of bytes than expected");
                   }
    }
}
