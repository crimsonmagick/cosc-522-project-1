#ifndef COSC522_LODI_MESSAGING_H
#define COSC522_LODI_MESSAGING_H

#define LOCALHOST "127.0.0.1"

int getSocket(const struct sockaddr_in *address, const struct timeval *timeout);

int getServerSocket(const unsigned short serverPort, const char *address);

int getUnboundSocket(const struct timeval *timeout);

int closeSocket(const int socket);

struct sockaddr_in getNetworkAddress(const char *ipAddress, const unsigned short serverPort);

int sendAndReceiveMessage(char *clientMessageIn, char *serverMessageOut, size_t messageInSize, size_t messageOutSize,
                          char *serverIP, unsigned short serverPort);

int receiveMessage(const int socket, char *message, const size_t messageSize, struct sockaddr_in *clientAddress);

int sendMessage(const int socket, const char *messageBuffer, const size_t messageSize,
                const struct sockaddr_in *destinationAddress);

int synchronousSend(const char *bufferIn, size_t bufferInSize, char *bufferOut, size_t bufferOutSize, const char *serverIP,
    unsigned short serverPort);

int synchronousSendWithClientPort(const char *bufferIn, size_t bufferInSize, char *bufferOut, size_t bufferOutSize, const char *serverIP,
    unsigned short serverPort, unsigned short clientPort);

#endif //COSC522_LODI_MESSAGING_H
