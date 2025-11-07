/**
* Interface for WIP Public authentication functions for simplifying digital sig validation. Not currently used.
*/

#ifndef COSC522_LODI_AUTHENTICATION_H
#define COSC522_LODI_AUTHENTICATION_H

int authenticate(unsigned long signature, unsigned long nonce, unsigned long publicKey);

#endif