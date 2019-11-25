#ifndef FUTILS_H__
#define FUTILS_H__

#include <stdio.h>

typedef struct _bstream
{
	FILE *file;
	char *buffer;
	int buffer_size;
	int buffer_index;
} bstream;

bool file_write(FILE *f, void *buffer, size_t size);
void file_write_secure(FILE *f, void *buffer, size_t size, const char *source, int line);
bool file_read(FILE *f, void *buffer, size_t size);
void file_read_secure(FILE *f, void *buffer, size_t size, const char *source, int line);

bstream *bstream_create(FILE *file, size_t size);
void bstream_flush(bstream *s);
void bstream_push(bstream *s, void *data, size_t size);
void bstream_destory(bstream *s);

#endif
