#ifndef COSC522_LODI_MESSAGING_H
#define COSC522_LODI_MESSAGING_H

#define SUCCESS 0
#define ERROR 1

int getSocket(const unsigned short serverPort, const char *address);

int sendAndReceiveMessage(char *clientMessageIn, char *serverMessageOut, size_t messageInSize, size_t messageOutSize,
                char *serverIP, unsigned short serverPort);

int receiveMessage(const int socket, char *message, const size_t messageSize, struct sockaddr_in *clientAddress);

int sendMessage(const int socket, const char *messageBuffer, const size_t messageSize,
  const struct sockaddr_in *destinationAddress);

#endif //COSC522_LODI_MESSAGING_H
