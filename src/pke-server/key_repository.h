#ifndef COSC522_LODI_KEY_REPOSITORY_H
#define COSC522_LODI_KEY_REPOSITORY_H

void init();

int addKey(unsigned int userId, unsigned int publicKey);

int getKey(unsigned int userId, unsigned int *publicKey);

#endif
