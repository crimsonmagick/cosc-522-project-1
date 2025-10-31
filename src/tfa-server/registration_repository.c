#include <string.h>

#include "registration_repository.h"

#include "shared.h"

#define SIZE 500

struct in_addr *keyStore[SIZE];

void init() {
  memset(keyStore, 0, SIZE * sizeof(struct in_addr));
}

int addIP(unsigned int userId, struct in_addr clientAddress) {
  const unsigned int idx = userId % SIZE;
  memcpy(&keyStore[idx], &clientAddress, sizeof(struct in_addr));
  return SUCCESS;
}

int getIP(unsigned int userId, struct in_addr *clientAddress) {
  const unsigned int idx = userId % SIZE;
  if (keyStore[idx] == 0) {
    // key not found
    return ERROR;
  }
  memcpy(clientAddress, &keyStore[idx], sizeof(struct in_addr));
  return SUCCESS;
}
