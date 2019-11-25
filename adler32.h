#ifndef ADLER32_H__
#define ADLER32_H__

#include <stdint.h>
#define BUFFER_SIZE 4096
//#define BUFFER_SIZE 1048576

uint32_t adler32_append(uint32_t adler, const char *buffer, size_t size);
uint32_t adler32(const char *buffer, size_t size);
uint32_t fnadler32(const char *file, uint32_t max_round);
uint32_t fadler32(const char *file);

#endif
