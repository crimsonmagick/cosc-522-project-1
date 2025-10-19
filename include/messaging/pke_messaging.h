#ifndef COSC522_LODI_PKEMESSAGING_H
#define COSC522_LODI_PKEMESSAGING_H
typedef struct {
  enum {ackRegisterKey, responsePublicKey} messageType; /* same as unsigned int */
  unsigned int userID; /* user identifier or user identifier of requested public key*/
  unsigned int publicKey; /* registered public key or requested public key */
} PKServerToLodiClient;
typedef PKServerToLodiClient PKServerToPClientOrLodiServer;

typedef struct {
  enum { registerKey, requestKey } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user's identifier or requested user identifier*/
  unsigned int publicKey; /* user's public key or 0 if message_type is request_key */
} PClientToPKServer;
#endif //COSC522_LODI_PKEMESSAGING_H