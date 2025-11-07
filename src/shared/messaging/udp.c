#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "shared.h"
#include "logging/logging.h"
#include "messaging/udp.h"

#define TIMEOUT_SECONDS 0
#define SOCK_FAILURE (-1)

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

int getSocket(const struct sockaddr_in *address, const struct timeval *timeout) {
  const int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) {
    perror("socket() failed");
    return SOCK_FAILURE;
  }

  if (timeout) {
    const int optResult = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, timeout, sizeof(*timeout));
    if (optResult < 0) {
      perror("Unable to set socket options");
      close(sock);
      return SOCK_FAILURE;
    }
  }

  if (address && bind(sock, (struct sockaddr *) address, sizeof(*address)) < 0) {
    perror("bind() failed");
    close(sock);
    return SOCK_FAILURE;
  }

  return sock;
}

int getUnboundSocket(const struct timeval *timeout) {
  const int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock >= 0 && timeout) {
    const int optResult = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, timeout, sizeof(*timeout));
    if (optResult < 0) {
      close(sock);
      return -1;
    }
  }
  return sock;
}

int closeSocket(const int socket) {
  return close(socket);
}

struct sockaddr_in getNetworkAddress(const char *ipAddress, const unsigned short serverPort) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (ipAddress) {
    inet_pton(AF_INET, ipAddress, &addr.sin_addr);
  } else {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  addr.sin_port = htons(serverPort);
  return addr;
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

int errorClose(const char *errorMessage, const int socket) {
  printf(errorMessage);
  close(socket);
  return ERROR;
}

int synchronousSend(const char *bufferIn, const size_t bufferInSize, char *bufferOut, const size_t bufferOutSize,
                    const char *serverIP, const unsigned short serverPort) {
  return synchronousSendWithClientPort(bufferIn, bufferInSize, bufferOut, bufferOutSize, serverIP, serverPort, 0);
}

int synchronousSendWithClientPort(const char *bufferIn, size_t bufferInSize, char *bufferOut, size_t bufferOutSize, const char *serverIP,
    unsigned short serverPort, unsigned short clientPort) {
  struct timeval timeout = {.tv_sec = TIMEOUT_SECONDS, .tv_usec = 0};
  const int clientSocket = getUnboundSocket(&timeout);

  if (clientSocket < 0) {
    return errorClose("Error: Unable to open socket.\n", clientSocket);
  }

  if (clientPort != 0) {
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = ntohs(clientPort);
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(clientSocket, (const struct sockaddr*) &clientAddr, sizeof(clientAddr)) < 0) {
      printf("Error: Failed to bind client port.\n");
      return ERROR;
    };
  }
  const struct sockaddr_in serverAddress = getNetworkAddress(serverIP, serverPort);
  const int sendStatus = sendMessage(clientSocket, bufferIn, bufferInSize, &serverAddress);
  if (sendStatus == ERROR) {
    printf("Error: Failed to send message.\n");
    return errorClose("Error: Failed to send message.\n", clientSocket);
  }

  struct sockaddr_in receiveAddress;
  const int receiveStatus = receiveMessage(clientSocket, bufferOut, bufferOutSize, &receiveAddress);
  if (receiveStatus == ERROR) {
    printf("Error while receiving message.\n");
    return errorClose("Error while receiving message.\n", clientSocket);
  }
  if (serverAddress.sin_addr.s_addr != receiveAddress.sin_addr.s_addr) {
    return errorClose("Error: received a packet from an unknown source.\n", clientSocket);
  }
  closeSocket(clientSocket);
  return SUCCESS;
}
