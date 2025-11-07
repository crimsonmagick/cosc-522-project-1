#include <arpa/inet.h>

#include "domain.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "messaging/udp.h"

#define DEFAULT_TIMEOUT_MS 100

struct DomainService {
  int sock;
  struct sockaddr_in hostAddr;
  struct sockaddr_in remoteAddr;
  MessageSerializer outgoingSerializer;
  MessageDeserializer incomingDeserializer;
};

/**
 * Deallocates handle and domain service
 * @param handle a non-null handle, with *handle and (*handle)->domainService non-null as well
 * @return DOMAIN_INIT_FAILURE value
 */
int failInit(DomainServiceHandle **handle) {
  free((*handle)->domainService);
  free(*handle);
  *handle = NULL;
  return DOMAIN_INIT_FAILURE;
}

int allocateHandle(DomainServiceHandle **handle) {
  *handle = malloc(sizeof(DomainServiceHandle));
  if (*handle == NULL) {
    return DOMAIN_INIT_FAILURE;
  }
  (*handle)->domainService = calloc(1, sizeof(DomainService));
  if ((*handle)->domainService == NULL) {
    free(*handle);
    *handle = NULL;
    return DOMAIN_INIT_FAILURE;
  }
  return DOMAIN_SUCCESS;
}

int startService(const DomainServiceOpts options, DomainServiceHandle **handle) {
  if (allocateHandle(handle) == DOMAIN_INIT_FAILURE) {
    return DOMAIN_INIT_FAILURE;
  }
  DomainService *domainService = (*handle)->domainService;
  const long timeoutMs = options.timeoutMs == NULL ? DEFAULT_TIMEOUT_MS : *options.timeoutMs;
  const long timeoutS = timeoutMs / 1000;
  const long timeoutUs = timeoutMs % 1000 * 1000;
  const struct timeval timeout = {.tv_sec = timeoutS, .tv_usec = timeoutUs};
  if (options.localPort != 0) {
    domainService->hostAddr = getNetworkAddress(LOCALHOST, options.localPort);
    domainService->sock = getSocket(&domainService->hostAddr, &timeout);
  } else {
    domainService->sock = getSocket(NULL, &timeout);
  }
  if (domainService->sock < 0) {
    return failInit(handle);
  }
  domainService->remoteAddr = getNetworkAddress(options.remoteHost, options.remotePort);
  domainService->incomingDeserializer = options.incomingDeserializer;
  domainService->outgoingSerializer = options.outgoingSerializer;

  return DOMAIN_SUCCESS;
}

int stopService(DomainServiceHandle **handle) {
  if (handle != NULL && *handle != NULL && (*handle)->domainService != NULL) {
    if ((*handle)->domainService->sock >= 0) {
      close((*handle)->domainService->sock);
    }
    free((*handle)->domainService);
  }
  if (handle != NULL && *handle != NULL) {
    free(*handle);
    *handle = NULL;
  }
  return DOMAIN_SUCCESS;
}
