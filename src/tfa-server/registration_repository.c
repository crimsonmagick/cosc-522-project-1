/**
* Provides persistence for registered client ip addresses and ports
 **/

#include <string.h>

#include "registration_repository.h"

#include "shared.h"

#define SIZE 500

struct in_addr *addressStore[SIZE];

unsigned short portStore[SIZE];

void init() {
  memset(addressStore, 0, SIZE * sizeof(struct in_addr));
  memset(portStore, 0, SIZE * sizeof(unsigned short));
}

int addIP(unsigned int userId, struct in_addr clientAddress, unsigned short clientPort) {
  const unsigned int idx = userId % SIZE;
  portStore[idx] = clientPort;
  memcpy(&addressStore[idx], &clientAddress, sizeof(struct in_addr));
  return SUCCESS;
}

int getIP(unsigned int userId, struct in_addr *clientAddress, unsigned short *clientPort) {
  const unsigned int idx = userId % SIZE;
  if (addressStore[idx] == 0 || portStore[idx] == 0) {
    // key not found
    return ERROR;
  }

  *clientPort = portStore[idx];

  memcpy(clientAddress, &addressStore[idx], sizeof(struct in_addr));
  return SUCCESS;
}
