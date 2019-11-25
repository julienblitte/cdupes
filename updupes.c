#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "adler32.h"
#include "mlocate.h"
#include "cdupes_db.h"
#include "futils.h"

uint32_t cdupes_seek_last_directory(FILE *f)
{
	int i, max;
	cdupes_set current_set;
	uint32_t last_directory;

	fseek(f, 0, SEEK_END);
	max = (ftell(f)-sizeof(cdupes_header))/sizeof(cdupes_set);

	if (max < 2)
		return 0;

	fseek(f, -sizeof(cdupes_set), SEEK_END);
	fread(&current_set, sizeof(current_set), 1, f);
	last_directory = current_set.dir;

	for(i = 2; i < max; i++)
	{
		fseek(f, -i*sizeof(cdupes_set), SEEK_END);
		if (fread(&current_set, sizeof(current_set), 1, f) != 1)
		{
			return 0;
		}
		if (current_set.dir != last_directory)
		{
			return last_directory-sizeof(struct db_entry);
		}
	}
	return 0;
}

static bstream *output;

void file_entry(long dir_offset, long file_offset, const char *name, int type, long filesize)
{
	uint32_t checksum;
	cdupes_set new_set;

	if (type == DBE_NORMAL && filesize != 0)
	{
		checksum = fnadler32(name, 1);
		new_set.dir = dir_offset;
		new_set.file = file_offset;
		new_set.size = filesize;
		new_set.checksum = checksum;
		bstream_push(output, &new_set, sizeof(new_set));
	}
}

int main()
{
	FILE *m;
	FILE *fout;
	cdupes_header header;
	uint32_t checksum;

	uint32_t last_directory;
	int progress, _progress;

	struct stat statinfo;

	if (lstat(MLOCATE_DB, &statinfo) == 0)
	{
		checksum = fadler32(MLOCATE_DB);
	}
	else
	{
		fprintf(stderr, "Error accessing mlocate database\n");
		return EXIT_FAILURE;
	}

	m = mloc_open(MLOCATE_DB);
	if (!m)
	{
		fprintf(stderr, "Error reading mlocate database\n");
		return EXIT_FAILURE;
	}

	// try to continue existing database
	fout = fopen(CDUPES_DB, "rb+");
	if (fout)
	{
		if (cdupes_check_header(fout, (uint32_t)statinfo.st_size, checksum))
		{
			last_directory = cdupes_seek_last_directory(fout);
			if (last_directory != 0)
			{
				fseek(m, last_directory, SEEK_SET);
			}
		}
		else
		{
			fclose(fout); // must close to erase all content
			fout = NULL;
		}
	}

	if (!fout)
	{
		fout = fopen(CDUPES_DB, "wb");
		if (!fout)
		{
			fprintf(stderr, "Error creating cdupes database\n");
			return EXIT_FAILURE;
		}

		header.magic = *((uint32_t *)&magic);
		header.filesize = (uint32_t)statinfo.st_size;
		header.checksum = checksum;
		header.version = CDUPES_VERSION;
		file_write_secure(fout, &header, sizeof(cdupes_header), __FILE__, __LINE__);
	}

	output = bstream_create(fout, 4096*sizeof(cdupes_set));
	if (!output)
	{
		fprintf(stderr, "Error allocating bufferized file stream\n");
		fclose(fout);
		fclose(m);
		return EXIT_FAILURE;
	}

	progress = -1;
	while (!feof(m))
	{
		mloc_read_directory(m, file_entry);
		_progress = (100*ftell(m))/statinfo.st_size;
		if (_progress > progress)
		{
			progress = _progress;
			printf("\rUpdating database...% 3d%%", progress); fflush(stdout);
		}
	}
	printf("\n");
	mloc_close(m);
	bstream_destory(output);
	fclose(fout);

	return EXIT_SUCCESS;
}
