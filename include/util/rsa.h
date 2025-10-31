#ifndef COSC522_LODI_RSA_H

// TODO remove hard-coding p and q - generate these when necessary
#define P 1000117
#define Q 1000151
#define MODULUS ((unsigned long) P * (unsigned long) Q)

typedef struct {
  unsigned long private;
  unsigned long public;
  unsigned long modulus;
} KeyGenResult;

KeyGenResult generateKeys(const unsigned int p, const unsigned int q);

unsigned long encryptTimestamp(unsigned long timestamp, unsigned long privateKey, unsigned long modulus);

unsigned long decryptTimestamp(unsigned long encrypted, unsigned long publicKey, unsigned long modulus);

#define COSC522_LODI_RSA_H

#endif
