#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "adler32.h"
#include "mlocate.h"
#include "cdupes_db.h"

void file_entry(long dir, long file, const char *name, int type, long filesize)
{
	uint32_t checksum;

	switch (type)
	{
	case DBE_NORMAL:
		checksum = fnadler32(name, 1);
		printf("%08x %08x: %s (%u) [%08x]\n", dir, file, name, filesize, checksum);
	break;
	case DBE_LINK:
		printf("%08x %08x: %s@\n", dir, file, name);
	break;
	case DBE_DIRECTORY:
		printf("%08x %08x: %s/\n", dir, file, name);
	break;
	}
}

int main()
{
	FILE *f;

	f = mloc_open(MLOCATE_DB);
	if (f)
	{
		while (!feof(f))
		{
			mloc_read_directory(f, file_entry);
		}
		mloc_close(f);
	}
	else
	{
		fprintf(stderr, "Error opening database\n");
	}

	return EXIT_SUCCESS;
}
