#ifndef COSC522_LODI_PKEMESSAGING_H
#define COSC522_LODI_PKEMESSAGING_H

#define PK_CLIENT_REQUEST_SIZE (3 * sizeof(uint32_t))
#define PK_SERVER_RESPONSE_SIZE (3 * sizeof(uint32_t))

typedef struct {
  enum { ackRegisterKey, responsePublicKey } messageType; /* same as unsigned int */
  unsigned int userID; /* user identifier or user identifier of requested public key*/
  unsigned int publicKey; /* registered public key or requested public key */
} PKServerToLodiClient;

typedef PKServerToLodiClient PKServerToPClientOrLodiServer;

typedef struct {
  enum { registerKey, requestKey } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user's identifier or requested user identifier*/
  unsigned int publicKey; /* user's public key or 0 if message_type is request_key */
} PClientToPKServer;

char *serializePKClientRequest(const PClientToPKServer *toSerialize);

PClientToPKServer *deserializePKClientRequest(const char *serialized, const size_t size);

char * serializePKServerResponse(const PKServerToLodiClient *toSerialize);

PKServerToLodiClient *deserializePKServerResponse(const char *serialized, const size_t size);

#endif //COSC522_LODI_PKEMESSAGING_H
