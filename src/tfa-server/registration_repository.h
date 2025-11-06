#ifndef COSC522_LODI_KEY_REPOSITORY_H
#define COSC522_LODI_KEY_REPOSITORY_H

#include <netinet/in.h>

void init();

int addIP(unsigned int userId, struct in_addr clientAddress, unsigned short clientPort);

int getIP(unsigned int userId, struct in_addr *clientAddress, unsigned short *clientPort);

#endif
