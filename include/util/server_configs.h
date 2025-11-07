#ifndef COSC522_LODI_SERVER_CONFIGS_H
#define COSC522_LODI_SERVER_CONFIGS_H
#include <netinet/in.h>

enum Server {
  PK,
  LODI,
  TFA,
  TFA_CLIENT
};

typedef struct {
  char *address;
  char *port;
} ServerConfig;

ServerConfig getServerConfig(const enum Server server);
struct sockaddr_in getServerAddr(const enum Server server);

#endif