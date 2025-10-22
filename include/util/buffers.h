#ifndef COSC522_LODI_BUFFERS_H
#define COSC522_LODI_BUFFERS_H

void appendUint32(char *buffer, size_t *offset, const uint32_t value);

void appendUint64(char *buffer, size_t *offset, const uint64_t value);

uint32_t getUint32(const char *buffer, size_t *offset);

uint64_t getUint64(const char *buffer, size_t *offset);

#endif //COSC522_LODI_BUFFERS_H
