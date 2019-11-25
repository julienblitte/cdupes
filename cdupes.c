#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "adler32.h"
#include "mlocate.h"
#include "cdupes_db.h"

int main(int argc, char *argv[])
{
	FILE *m;
	FILE *d;

	int i;
	uint32_t quick_checksum;
	uint32_t full_checksum;
	uint32_t filesize;

	struct stat statinfo;

	cdupes_set current_set;
	uint32_t found_checksum;
	char file[MAX_PATH];
	char dir[MAX_PATH];

	if (lstat(MLOCATE_DB, &statinfo) != 0)
	{
		fprintf(stderr, "Error accessing mlocate database!\n");
		return EXIT_FAILURE;
	}

	d = fopen(CDUPES_DB, "rb");
	if (!d)
	{
		fprintf(stderr, "Error reading cdupes database.\nPlease run command 'updupes' to fix this problem.\n");
		return EXIT_FAILURE;
	}

	if (!cdupes_check_header(d, (uint32_t)statinfo.st_size, fadler32(MLOCATE_DB)))
	{
		fprintf(stderr, "The mlocate database has changed! Please run command 'updupes'.\n");
		fclose(d);
		return EXIT_FAILURE;
	}

	m = fopen(MLOCATE_DB, "rb");
	if (!m)
	{
		fprintf(stderr, "Error reading mlocate database.\n");
		fclose(d);
		return EXIT_FAILURE;
	}

	for(i=1; i< argc; i++)
	{
		printf("Duplicates of '%s':\n", argv[i]);
		if (lstat(argv[i], &statinfo) == 0)
		{
			filesize = (uint32_t)statinfo.st_size;
			quick_checksum = fnadler32(argv[i], 1);
			full_checksum = fadler32(argv[i]);
		
		}
		else
		{
			fprintf(stderr, "Error accessing '%s'\n", argv[i]);
			continue;
		}

		fseek(d, sizeof(cdupes_header), SEEK_SET);
		while (!feof(d))
		{
			if (fread(&current_set, sizeof(current_set), 1, d) == 1)
			{
				if ((current_set.size == filesize) && (current_set.checksum == quick_checksum))
				{
					fseek(m, current_set.dir, SEEK_SET);
					if (mloc_read_string(m, dir, sizeof(dir)) == 0)
					{
						dir[0] = '\0';
					}
					fseek(m, current_set.file, SEEK_SET);
					if (mloc_read_string(m, file, sizeof(file)) != 0)
					{
						strncat(dir, DIRECTORY_SEPARATOR, sizeof(dir));
						strncat(dir, file, sizeof(dir));
						if (strcmp(dir, argv[i]) != 0)
						{
							found_checksum = fadler32(argv[i]);
							if (found_checksum == full_checksum)
							{
								printf("* %s\n", dir);
							}
						}
					}
				}
			}
		}
	}

	close(d);
	close(m);

	return EXIT_SUCCESS;
}
