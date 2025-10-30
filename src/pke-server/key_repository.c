#include <stdio.h>
#include <string.h>

#define SIZE 50

unsigned int keyStore[SIZE];

void init() {
  memset(keyStore, 0, SIZE * sizeof(unsigned int));
}

int addKey(unsigned int userId, unsigned int publicKey) {
  const unsigned int idx = userId % SIZE;
  keyStore[idx] = publicKey;
  return 0;
}

int getKey(unsigned int userId, unsigned int *publicKey) {
  const unsigned int idx = userId % SIZE;
  const unsigned int retrieved = keyStore[idx];
  if (retrieved == 0) {
    // key not found
    return 1;
  }
  *publicKey = retrieved;
  return 0;
}
