/*
Copyright (C) 2018 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "io.h"

static int stringComparator(const void *a, const void *b);

int fileExists(const char *filename)
{
	struct stat buffer;

	return (stat(filename, &buffer) == 0);
}

const char *getFileLocation(const char *filename)
{
	static char path[MAX_FILENAME_LENGTH];
	memset(path, '\0', MAX_FILENAME_LENGTH);

	if (fileExists(filename))
	{
		return filename;
	}

	sprintf(path, DATA_DIR"/%s", filename);
	
	if (!fileExists(path))
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such file '%s'", path);
		exit(1);
	}

	return path;
}

char *readFile(const char *filename)
{
	char *buffer = 0;
	unsigned long length;
	FILE *file = fopen(getFileLocation(filename), "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = malloc(length + 1);
		memset(buffer, 0, length + 1);
		fread(buffer, 1, length, file);

		fclose(file);
		
		buffer[length] = '\0';
	}

	return buffer;
}

char *readCompressedFile(const char *filename)
{
	unsigned char *buffer, *cBuffer;
	uint32_t l1, l2;
	unsigned long length, cLength;
	FILE *file = fopen(getFileLocation(filename), "rb");
	
	buffer = 0;
	cBuffer = 0;

	if (file)
	{
		fread(&l1, sizeof(uint32_t), 1, file);
		fread(&l2, sizeof(uint32_t), 1, file);
		
		l1 = SDL_SwapLE32(l1);
		l2 = SDL_SwapLE32(l2);
		
		length = l1;
		cLength = l2;
		
		buffer = malloc(length + 1);
		memset(buffer, 0, length + 1);
		
		cBuffer = malloc(cLength + 1);
		memset(cBuffer, 0, cLength + 1);
		
		fread(cBuffer, 1, cLength, file);
		
		uncompress(buffer, &length, cBuffer, cLength);

		fclose(file);
		
		buffer[length] = '\0';
		
		free(cBuffer);
		
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Decompressed '%s' %ld -> %ld", filename, cLength, length);
	}

	return (char*) buffer;
}

int writeFile(const char *filename, const char *data)
{
	FILE *file = fopen(filename, "wb");

	if (file)
	{
		fprintf(file, "%s\n", data);
		fclose(file);
		return 1;
	}

	return 0;
}

char **getFileList(const char *dir, int *count)
{
	DIR *d;
	int i;
	struct dirent *ent;
	char **filenames;

	i = 0;
	filenames = NULL;

	if ((d = opendir(getFileLocation(dir))) != NULL)
	{
		while ((ent = readdir(d)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				i++;
			}
		}

		if (i > 0)
		{
			filenames = malloc(sizeof(char*) * i);
			memset(filenames, 0, sizeof(char*) * i);

			rewinddir(d);

			i = 0;

			while ((ent = readdir(d)) != NULL)
			{
				if (ent->d_name[0] != '.')
				{
					filenames[i] = malloc(sizeof(char) * MAX_FILENAME_LENGTH);

					STRNCPY(filenames[i], ent->d_name, MAX_FILENAME_LENGTH);

					i++;
				}
			}
		}

		closedir(d);
	}

	*count = i;

	if (filenames)
	{
		qsort(filenames, i, sizeof(char*), stringComparator);
	}

	return filenames;
}

static int stringComparator(const void *a, const void *b)
{
    char **s1 = (char **)a;
    char **s2 = (char **)b;
    return strcmp(*s1, *s2);
}
