/**
 * Functions for generating public/private keys and performing encryption and decryption of timestamps.
 */

#include "util/rsa.h"

// adapted from https://en.wikipedia.org/wiki/Euclidean_algorithm
unsigned long gcd(unsigned long a, unsigned long b) {
    while (b != 0) {
        const unsigned long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// adapted from https://rosettacode.org/wiki/Modular_inverse#C
unsigned long modInverse(unsigned long a, unsigned long m) {
    long m0 = (long) m;
    long x0 = 0, x1 = 1;

    if (m == 1)
        return 0;

    while (a > 1) {
        long q = a / m;
        long t = m;

        m = a % m;
        a = t;
        t = x0;

        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) {
        x1 += m0;
    }

    return (unsigned long) x1;
}

KeyGenResult generateKeys(const unsigned int p, const unsigned int q) {
    unsigned long n = (unsigned long) p * (unsigned long) q;
    unsigned long phiResult = (unsigned long) (p - 1) * (unsigned long) (q - 1);
    // TODO randomize
    unsigned long e = 12;
    while (gcd(e, phiResult) != 1) {
        e++;
    }
    unsigned long d = modInverse(e, phiResult);
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
            result = (__uint128_t) result * (__uint128_t) base % modulus;
        }
        base = (__uint128_t) base * (__uint128_t) base % modulus;
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
