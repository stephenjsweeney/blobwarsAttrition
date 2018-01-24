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

#include "atlas.h"

static void loadAtlasTexture(void);
static void loadAtlasData(void);

static Atlas atlasHead;
static Atlas *atlasTail;
/*
static Texture atlasTexture;
static int atlasSize;
*/

void initAtlas(void)
{
	memset(&atlasHead, 0, sizeof(Atlas));
	atlasTail = &atlasHead;
	
	loadAtlasTexture();

	loadAtlasData();
}

Atlas *getImageFromAtlas(char *filename)
{
	Atlas *a;
	
	for (a = atlasHead.next ; a != NULL ; a = a->next)
	{
		if (strcmp(a->filename, filename) == 0)
		{
			return a;
		}
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such atlas image '%s'", filename);
	exit(1);
}

static void loadAtlasTexture(void)
{
	
}

static void loadAtlasData(void)
{
	
}

void loadImageData(cJSON *root)
{
	
}

void createRectangle(char *filename, int x, int y, int w, int h)
{
	
}

