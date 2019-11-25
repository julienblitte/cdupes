#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "futils.h"

bool file_write(FILE *f, void *buffer, size_t size)
{
	return (fwrite(buffer, size, 1, f) == 1);
}

void file_write_secure(FILE *f, void *buffer, size_t size, const char *source, int line)
{
	if (!file_write(f, buffer, size))
	{
		fprintf(stderr, "Error writting file, %s:%d\n", ftell(f), source, line);
		fclose(f);
		exit(EXIT_FAILURE);
	}
}

bool file_read(FILE *f, void *buffer, size_t size)
{
	return (fread(buffer, size, 1, f) == 1);
}

void file_read_secure(FILE *f, void *buffer, size_t size, const char *source, int line)
{
	if (!file_read(f, buffer, size))
	{
		fprintf(stderr, "Error reading file, file offset @%08x, %s:%d\n", ftell(f), source, line);
		fclose(f);
		exit(EXIT_FAILURE);
	}
}

/*
typedef struct _bstream
{
	FILE *file;
	void *buffer;
	size_t buffer_size;
	size_t buffer_index;
} bstream;
*/
bstream *bstream_create(FILE *file, size_t size)
{
	bstream *result;
	result = (bstream *)malloc(sizeof(bstream));
	if (!result)
	{
		return NULL;
	}
	result->file = file;
	result->buffer_index = 0;
	result->buffer_size = size;
	result->buffer = malloc(size);
	if (!result->buffer)
	{
		free(result);
		return NULL;
	}
	return result;
}

void bstream_flush(bstream *s)
{
	if (s->buffer_index)
	{
		file_write_secure(s->file, s->buffer, s->buffer_index, __FILE__, __LINE__);
		s->buffer_index = 0;
	}
}

void bstream_push(bstream *s, void *data, size_t size)
{
	if (s->buffer_index + size >= s->buffer_size)
	{
		bstream_flush(s);
	}
	memcpy(s->buffer+s->buffer_index, data, size);
	s->buffer_index += size;
}

void bstream_destory(bstream *s)
{
	bstream_flush(s);
	free(s->buffer);
	free(s);
}
