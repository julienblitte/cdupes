#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "db.h"
#include "mlocate.h"

static const uint8_t magic[] = DB_MAGIC;
static int check_visibility;

int mloc_read_string(FILE *f, char *buffer, size_t max_size)
{
	int i;

	i = 0;
	while(i < max_size-1)
	{
		file_read_secure(f, &(buffer[i]), 1, __LINE__);
		if (buffer[i] == '\0')
		{
			break;
		}
		i++;
		if (i == max_size-1)
		{
			buffer[i] = '\0';
			i++;
			break;
		}
	}
	return i;
}

int mloc_read_directory(FILE *f, void (*directory_entry)(long dir_offset, long file_offset, const char *name, int type, long size))
{
	struct db_directory dir_header;
	struct db_entry file_header;
	char directory[MAX_PATH];
	char file[MAX_PATH];
	char absolute[MAX_PATH];
	int files;
	long dir_offset, file_offset;
	struct stat statinfo;

	if (!file_read(f, &dir_header, sizeof(dir_header)))
	{
		return -1;
	}
	dir_offset = ftell(f);
	mloc_read_string(f, directory, sizeof(directory));
	if (strcmp(directory, DIRECTORY_SEPARATOR) == 0)
	{
		directory[0] = '\0';
	}

	files = 0;
	while(!feof(f))
	{
		file_read_secure(f, (void *)&file_header, sizeof(file_header), __LINE__);
		file_offset = ftell(f);
		switch (file_header.type)
		{
		case DBE_NORMAL:
			mloc_read_string(f, file, sizeof(file));
			strncpy(absolute, directory, sizeof(absolute));
			strncat(absolute, DIRECTORY_SEPARATOR, sizeof(absolute));
			strncat(absolute, file, sizeof(absolute));
			if (lstat(absolute, &statinfo) == 0)
			{
				if (strncmp(directory, "/dev", 4) == 0 || strncmp(directory, "/sys", 4) == 0 || strncmp(directory, "/proc", 5) == 0)
				{
					directory_entry(dir_offset, file_offset, absolute, DBE_SYS, statinfo.st_size);
				}
				else if (S_ISREG(statinfo.st_mode))
				{
					directory_entry(dir_offset, file_offset, absolute, DBE_NORMAL, statinfo.st_size);
				}
				else if (S_ISLNK(statinfo.st_mode))
				{
					directory_entry(dir_offset, file_offset, absolute, DBE_LINK, 0);
				}
			}
		break;
		case DBE_DIRECTORY:
			mloc_read_string(f, file, sizeof(file));
			mloc_read_string(f, file, sizeof(file));
			strncpy(absolute, directory, sizeof(absolute));
			strncat(absolute, DIRECTORY_SEPARATOR, sizeof(absolute));
			directory_entry(dir_offset, file_offset, absolute, DBE_DIRECTORY, 0);
		break;
		case DBE_END:
			return files;
		break;
		}
		files++;
	}
	return files;
}

FILE *mloc_open(const char *database)
{
	FILE *f;
	struct db_header header;
	char fs[MAX_PATH];

	f = fopen(database, "rb");
	if (f)
	{
		file_read_secure(f, &header, sizeof(header), __LINE__);
		if (memcmp(header.magic, magic, sizeof(magic)) != 0)
		{
			fprintf(stderr, "Error header of database invalid\n");
			fclose(f);
			return NULL;
		}
		if (header.version != DB_VERSION_0)
		{
			fprintf(stderr, "Error unsupported version of database\n");
			fclose(f);
			return NULL;
		}
		header.conf_size = htonl(header.conf_size);
		check_visibility = header.check_visibility;

		mloc_read_string(f, fs, sizeof(fs));
		fseek(f, header.conf_size, SEEK_CUR);
	}

	return f;
}

void mloc_close(FILE *f)
{
	if (f)
		fclose(f);
}

