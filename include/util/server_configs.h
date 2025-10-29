#ifndef COSC522_LODI_SERVER_CONFIGS_H
#define COSC522_LODI_SERVER_CONFIGS_H

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

#endif //COSC522_LODI_SERVER_CONFIGS_H