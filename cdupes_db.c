#include "cdupes_db.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

const uint8_t magic[] = CDUPES_MAGIC;

bool cdupes_check_header(FILE *f, uint32_t mloc_size, uint32_t mloc_sum)
{
	cdupes_header header;

	if (fread(&header, sizeof(cdupes_header), 1, f) != 1)
	{
		return false;
	}
	return ((header.magic == (*(uint32_t *)&magic)) && (header.filesize == mloc_size) && (header.checksum == mloc_sum));
}

