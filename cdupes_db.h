#ifndef CDUPES_DB_H__
#define CDUPES_DB_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define CDUPES_VERSION 0x00000000
#define CDUPES_MAGIC { '\0', 'd', 'u', 'p' }

extern const uint8_t magic[];

typedef struct { uint32_t magic; uint32_t filesize; uint32_t checksum; uint32_t version; } cdupes_header;
typedef struct { uint32_t dir; uint32_t file; uint32_t size; uint32_t checksum; } cdupes_set;

bool cdupes_check_header(FILE *f, uint32_t mloc_size, uint32_t mloc_sum);

#endif
