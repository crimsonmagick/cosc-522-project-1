/**
* Provides persistence for registered client ip addresses and ports
 **/

#include <string.h>

#include "registration_repository.h"

#include <stdlib.h>

#include "shared.h"

#define SIZE 500

struct in_addr *addressStore[SIZE];

unsigned short portStore[SIZE];

void initRepository() {
  memset(addressStore, 0, SIZE * sizeof(struct in_addr));
  memset(portStore, 0, SIZE * sizeof(unsigned short));
}

int addIP(unsigned int userId, struct in_addr clientAddress, unsigned short clientPort) {
  const unsigned int idx = userId % SIZE;
  portStore[idx] = clientPort;
  if (addressStore[idx] == NULL) {
    addressStore[idx] = malloc(sizeof(struct in_addr));
  }
  *addressStore[idx] = clientAddress;
  return SUCCESS;
}

int getIP(unsigned int userId, struct in_addr *clientAddress, unsigned short *clientPort) {
  const unsigned int idx = userId % SIZE;
  if (addressStore[idx] == 0 || portStore[idx] == 0) {
    // key not found
    return ERROR;
  }

  *clientPort = portStore[idx];
  *clientAddress = *addressStore[idx];

  return SUCCESS;
}
