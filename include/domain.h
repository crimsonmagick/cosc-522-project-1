#ifndef COSC522_LODI_DOMAIN_H
#define COSC522_LODI_DOMAIN_H

#define DOMAIN_SUCCESS 0
#define MESSAGE_SERIALIZER_SUCCESS 0
#define MESSAGE_DESERIALIZER_SUCCESS 0
#define DOMAIN_FAILURE 1
#define DOMAIN_INIT_FAILURE 2

typedef struct DomainService DomainService;

typedef struct MessageSerializer {
  size_t messageSize;
  int (*serializer)(void *, char *);

} MessageSerializer;

typedef struct MessageDeserializer {
  size_t messageSize;
  int (*deserializer)(char *, void *);

} MessageDeserializer;

typedef struct DomainServiceHandle {
  DomainService *domainService;
} DomainServiceHandle;

typedef struct DomainServiceOpts {
  unsigned short localPort;
  unsigned short remotePort;
  unsigned int *timeoutMs;
  char * remoteHost;
  MessageSerializer outgoingSerializer;
  MessageDeserializer incomingDeserializer;
} DomainServiceOpts;

int startService(const DomainServiceOpts options, DomainServiceHandle **handle);


#endif
