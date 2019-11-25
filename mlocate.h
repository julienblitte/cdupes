#ifndef MLOCATE_H__
#define MLOCATE_H__

#include <stdbool.h>

#define MAX_PATH 4096

#include "db.h"

#define DIRECTORY_SEPARATOR "/"

enum { DBE_LINK = 3,
       DBE_SYS = 4
};

int mloc_read_string(FILE *f, char *buffer, size_t max_size);
int mloc_read_directory(FILE *f, void (*directory_entry)(long dir_offset, long file_offset, const char *name, int type, long size));
FILE *mloc_open(const char *database);
void mloc_close(FILE *f);

#endif
