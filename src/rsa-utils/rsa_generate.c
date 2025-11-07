/**
 * Generates a public key/private key pair, and validates that it works against a timestamp sourced from the
 * system clock.
 */

#include "util/rsa.h"

#include <stdio.h>
#include <time.h>

int main() {
    // generated large primes at https://www.browserling.com/tools/prime-numbers
    // note that q * p has to be bigger than the timestamp
    // TODO generate primes automatically
    const unsigned int p = 1000117;
    const unsigned int q = 1000151;
    const KeyGenResult keys = generateKeys(p, q);
    printf("publicKey=%ld, privateKey=%ld, modulus=%ld\n", keys.public, keys.private, keys.modulus);
    unsigned long timestamp;
    time(&timestamp);
    const unsigned long encrypted = encryptTimestamp(timestamp, keys.private, keys.modulus);
    const unsigned long decrypted = decryptTimestamp(encrypted, keys.public, keys.modulus);
    printf("timestamp=%ld, encrypted=%ld, decrypted=%ld\n", timestamp, encrypted, decrypted);
    if (timestamp == decrypted) {
        printf("Encryption/decryption success!\n");
    } else {
        printf("Encryption/decryption failure...\n");
    }
    return 0;
}
