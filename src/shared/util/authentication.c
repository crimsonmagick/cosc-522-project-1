/**
* WIP Public authentication functions for simplifying digital sig validation. Not currently used.
*/

#include "authentication.h"

#include "shared.h"
#include "util/rsa.h"

int authenticate(unsigned long signature, unsigned long nonce, unsigned long publicKey) {
  const unsigned long decrypted = decryptTimestamp(signature, publicKey, MODULUS);
  if (decrypted != signature) {
    return ERROR;
  }
}
