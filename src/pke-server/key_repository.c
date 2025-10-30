#include <string.h>

#include "key_repository.h"
#include "shared.h"

#define SIZE 500

unsigned int keyStore[SIZE];

void init() {
  memset(keyStore, 0, SIZE * sizeof(unsigned int));
}

int addKey(unsigned int userId, unsigned int publicKey) {
  const unsigned int idx = userId % SIZE;
  keyStore[idx] = publicKey;
  return SUCCESS;
}

int getKey(unsigned int userId, unsigned int *publicKey) {
  const unsigned int idx = userId % SIZE;
  const unsigned int retrieved = keyStore[idx];
  if (retrieved == 0) {
    // key not found
    return ERROR;
  }
  *publicKey = retrieved;
  return SUCCESS;
}
