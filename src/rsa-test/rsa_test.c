#include <stdio.h>

typedef struct {
    unsigned long private;
    unsigned long public;
    unsigned long modulus;
} KeyGenResult;

// adapted from https://en.wikipedia.org/wiki/Euclidean_algorithm
unsigned long gcd(unsigned long a, unsigned long b) {
    while (b != 0) {
        const unsigned long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

KeyGenResult generateKeys(const unsigned int p, const unsigned int q) {
    unsigned long n = p * q;
    unsigned long phiResult = (p - 1) * (q - 1);
    // TODO randomize
    unsigned long e = 12;
    while(gcd(e, phiResult) != 1) {
        e++;
    }
    unsigned long d = 1;
    while((d * e) % phiResult != 1) {
        d++;
    }
    KeyGenResult result = {
            .private = d,
            .public = e,
            .modulus = n
    };
    return result;
}


// Based on https://en.wikipedia.org/wiki/Modular_exponentiation
unsigned long modPow(unsigned long base, unsigned long exponent, unsigned long modulus) {
    if (modulus == 1) {
        return 0;
    }

    unsigned long result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = result * base % modulus;
        }
        base = base * base % modulus;
        exponent = exponent >> 1;
    }
    return result;
}

unsigned long encryptTimestamp(unsigned long timestamp, unsigned long privateKey, unsigned long modulus) {
    return modPow(timestamp, privateKey, modulus);
}

unsigned long decryptTimestamp(unsigned long encrypted, unsigned long publicKey, unsigned long modulus) {
    return modPow(encrypted, publicKey, modulus);
}

int main() {
    KeyGenResult keys = generateKeys(61, 53);
    long timestamp = 1625;
    //    time(&ts);
    unsigned long encrypted = encryptTimestamp(timestamp, keys.private, keys.modulus);
    unsigned long decrypted = decryptTimestamp(encrypted, keys.public, keys.modulus);
    printf("timestamp=%ld, encrypted=%ld, decrypted=%ld\n", timestamp, encrypted, decrypted);
    return 0;
}
