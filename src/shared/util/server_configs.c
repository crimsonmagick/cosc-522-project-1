#include "util/server_configs.h"

#include <stdlib.h>

#define LOCALHOST "127.0.0.1"

static char *SERVER_ADDRESS_KEYS[] = {
  "PUBLIC_KEY_ADDRESS",
  "LODI_ADDRESS",
  "TFA_ADDRESS"
};

static char *SERVER_PORT_KEYS[] = {
  "PUBLIC_KEY_PORT",
  "LODI_PORT",
  "TFA_PORT"
};

static char *SERVER_DEFAULT_PORTS[] = {
  "9091",
  "9092",
  "9093"
};

ServerConfig getServerConfig(const enum Server server) {
  char *address = getenv(SERVER_ADDRESS_KEYS[server]);
  char *port = getenv(SERVER_PORT_KEYS[server]);
  if (!address) {
    address = LOCALHOST;
  }
  if (!port) {
    port = SERVER_DEFAULT_PORTS[server];
  }
  return (ServerConfig){
    .address = address,
    .port = port
  };
}
