#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "logging/logging.h"
#include "messaging/udp.h"

#define TIMEOUT_SECONDS 5

int getServerSocket(const unsigned short serverPort, const char *address) {
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

int getClientSocket(const struct timeval *timeout) {
  const int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock >= 0 && timeout) {
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, timeout, sizeof(*timeout));
  }
  return sock;
}

int closeSocket(const int socket) {
  return close(socket);
}

struct sockaddr_in getNetworkAddress(const char *serverIP, const unsigned short serverPort) {
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);
  serverAddr.sin_port = htons(serverPort);
  return serverAddr;
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

int synchronousSend(const char *bufferIn, const size_t bufferInSize, char *bufferOut, const size_t bufferOutSize,
                    const char *serverIP, const unsigned short serverPort) {
  struct timeval timeout = {.tv_sec = TIMEOUT_SECONDS, .tv_usec = 0};
  const int clientSocket = getClientSocket(&timeout);
  if (clientSocket < 0) {
    printf("Error: Unable to open socket.\n");
    return ERROR;
  }
  const struct sockaddr_in serverAddress = getNetworkAddress(serverIP, serverPort);
  const int sendStatus = sendMessage(clientSocket, bufferIn, bufferInSize, &serverAddress);
  if (sendStatus == ERROR) {
    printf("Error: Failed to send message.\n");
    closeSocket(clientSocket);
    return ERROR;
  }

  struct sockaddr_in receiveAddress;
  const int receiveStatus = receiveMessage(clientSocket, bufferOut, bufferOutSize, &receiveAddress);
  if (receiveStatus == ERROR) {
    printf("Error while receiving message.\n");
    closeSocket(clientSocket);
    return ERROR;
  }
  if (serverAddress.sin_addr.s_addr != receiveAddress.sin_addr.s_addr) {
    printf("Error: received a packet from unknown source.");
    closeSocket(clientSocket);
    return ERROR;
  }
  closeSocket(clientSocket);
  return SUCCESS;
}
