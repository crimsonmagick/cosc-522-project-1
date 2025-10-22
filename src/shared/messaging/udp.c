#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "logging/logging.h"
#include "messaging/udp.h"

int sendAndReceiveMessage(char *clientMessageIn, char *serverMessageOut, size_t messageInSize, size_t messageOutSize,
                          char *serverIP, unsigned short serverPort) {
  int sock;
  struct sockaddr_in fromAddr;

  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    logError("socket() failed");
    return ERROR;
  }

  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);
  serverAddr.sin_port = htons(serverPort);

  if (sendto(sock, clientMessageIn, messageInSize, 0, (struct sockaddr *)
             &serverAddr, sizeof(serverAddr)) != messageInSize) {
    logError("sendto() sent a different number of bytes than expected");
    return ERROR;
  }

  socklen_t fromSize = sizeof(fromAddr);

  if (recvfrom(sock, serverMessageOut, messageOutSize, 0,
               (struct sockaddr *) &fromAddr, &fromSize) != messageOutSize) {
    logError("recvfrom() failed");
    return ERROR;
  }

  if (serverAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    fprintf(stderr, "Error: received a packet from unknown source.\n");
    return ERROR;
  }

  close(sock);
  return SUCCESS;
}

int getSocket(const unsigned short serverPort, const char *address) {
  const int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) {
    logError("socket() failed");
    return sock;
  }

  struct sockaddr_in serverAddr;

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  if (address) {
    inet_pton(AF_INET, address, &serverAddr.sin_addr);
  } else {
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  serverAddr.sin_port = htons(serverPort);

  if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
    logError("bind() failed");
    return sock;
  }

  return sock;
}


int receiveMessage(const int socket, char *message, const size_t messageSize, struct sockaddr_in *clientAddress) {
  socklen_t clientAddrLen = sizeof(*clientAddress);
  const ssize_t numBytes = recvfrom(socket, message, messageSize, 0,
                                    (struct sockaddr *) clientAddress, &clientAddrLen);
  if (numBytes < 0) {
    logError("recvfrom() failed");
    return ERROR;
  }

  char printableAddress[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &clientAddress->sin_addr, printableAddress, INET_ADDRSTRLEN);
  printf("Received message from client %s\n", printableAddress);

  if (numBytes != (ssize_t) messageSize) {
    printf("Received more bytes than expected: received %zd, expected %zd. Output is truncated.\n", numBytes,
           messageSize);
    return ERROR;
  }

  return SUCCESS;
}

int sendMessage(const int socket, const char *messageBuffer, const size_t messageSize,
                const struct sockaddr_in *destinationAddress) {
  const ssize_t numBytes = sendto(socket, messageBuffer, messageSize, 0, (struct sockaddr *) destinationAddress,
                                  sizeof(*destinationAddress));

  if (numBytes < 0) {
    logError("sendTo() failed");
    return ERROR;
  }

  if (numBytes != (ssize_t) messageSize) {
    printf("sendto() sent a different number of bytes than expected\n");
    return ERROR;
  }

  return SUCCESS;
}
