#include <stdio.h>

void logError(const char *errorMessage) {
	perror(errorMessage);
}
