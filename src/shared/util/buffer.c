#include <stdint.h>
#include <string.h>

#include <endian.h>
#include "util/buffers.h"

static void appendBytes(char *buffer, size_t *offset, const void *src, const size_t len) {
  memcpy(buffer + *offset, src, len);
  *offset += len;
}

void appendUint32(char *buffer, size_t *offset, const uint32_t value) {
  const uint32_t networkInt = htobe32(value);
  appendBytes(buffer, offset, &networkInt, sizeof(uint32_t));
}

void appendUint64(char *buffer, size_t *offset, const uint64_t value) {
  const uint64_t networkInt = htobe64(value);
  appendBytes(buffer, offset, &networkInt, sizeof(uint64_t));
}

static void getBytes(const char *buffer, size_t *offset, void *dest, const size_t len) {
  memcpy(dest, buffer + *offset, len);
  *offset += len;
}

uint32_t getUint32(const char *buffer, size_t *offset) {
  uint32_t value;
  getBytes(buffer, offset, &value, sizeof(uint32_t));
  value = be32toh(value);
  return value;
}

uint64_t getUint64(const char *buffer, size_t *offset) {
  uint64_t value;
  getBytes(buffer, offset, &value, sizeof(uint64_t));
  value = be64toh(value);
  return value;
}
