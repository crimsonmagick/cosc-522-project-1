#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "logging/logging.h"
#include "messaging/utils.h"

#define SUCCESS 0
#define ERROR 1

int sendMessage(char *clientMessageIn, char *serverMessageOut, char *serverIP, unsigned short serverPort) {
  int sock; /* Socket descriptor */
  struct sockaddr_in fromAddr; /* Source address of echo */

  /* Create a datagram/UDP socket */
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    logError("socket() failed");
    return ERROR;
  }

  /* Construct the server address structure */
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);
  serverAddr.sin_port = htons(serverPort);

  if (sendto(sock, &clientMessageIn, sizeof(clientMessageIn), 0, (struct sockaddr *)
             &serverAddr, sizeof(serverAddr)) != sizeof(clientMessageIn)) {
    logError("sendto() sent a different number of bytes than expected");
    return ERROR;
  }

  socklen_t fromSize = sizeof(fromAddr);

  if (recvfrom(sock, &serverMessageOut, sizeof(serverMessageOut), 0,
               (struct sockaddr *) &fromAddr, &fromSize) != sizeof(serverMessageOut)) {
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
