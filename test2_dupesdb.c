#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "adler32.h"
#include "mlocate.h"
#include "cdupes_db.h"

int main()
{
	FILE *m;
	FILE *d;
	uint32_t mloc_sig[2];
	cdupes_header header;
	struct stat statinfo;
	cdupes_set current_set;
	char buffer[1024];

	if (lstat(MLOCATE_DB, &statinfo) != 0)
	{
		fprintf(stderr, "Error accessing file '%s'!\n", MLOCATE_DB);
		fclose(d);
		return EXIT_FAILURE;
	}

	d = fopen(CDUPES_DB, "rb");
	if (!d)
	{
		fprintf(stderr, "Error reading file '%s'!\n", CDUPES_DB);
		return EXIT_FAILURE;
	}

	if (!cdupes_check_header(d, (uint32_t)statinfo.st_size, fadler32(MLOCATE_DB)))
	{
		fprintf(stderr, "File '%s' has changed!\nPlease run command 'updupes'\n", MLOCATE_DB);
		fclose(d);
		return EXIT_FAILURE;
	}

	m = fopen(MLOCATE_DB, "rb");
	if (!m)
	{
		fprintf(stderr, "Error reading file '%s'!\n", MLOCATE_DB);
		fclose(d);
		return EXIT_FAILURE;
	}

	while (!feof(d))
	{
		if (file_read(d, &current_set, sizeof(current_set), __FILE__, __LINE__))
		{
			printf("{fsize:%08x, cksum:%08x} ", current_set.size, current_set.checksum);
			fseek(m, current_set.dir, SEEK_SET);
			if (mloc_read_string(m, buffer, sizeof(buffer)) != 0)
			{
				printf("%s/", buffer);
			}
			fseek(m, current_set.file, SEEK_SET);
			if (mloc_read_string(m, buffer, sizeof(buffer)) != 0)
			{
				printf("%s\n", buffer);
			}
		}
	}
	close(m);
	close(d);

	return EXIT_SUCCESS;
}
