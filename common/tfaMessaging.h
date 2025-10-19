#ifndef COSC522_LODI_TFAMESSAGING_H
#define COSC522_LODI_TFAMESSAGING_H
typedef struct {
  enum { registerTFA, ackRegTFA, ackPushTFA, requestAuth } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user identifier */
  unsigned long timestamp; /* timestamp */
  unsigned long digitalSig; /* encrypted timestamp */
} TFAClientOrLodiServerToTFAServer;

typedef struct {
  enum {confirmTFA , pushTFA } messageType; /* same as unsigned int */
  unsigned int userID; /* user identifier*/
} TFAServerToTFAClient;


#endif //COSC522_LODI_TFAMESSAGING_H