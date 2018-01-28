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
static void loadImageData(cJSON *root);

static Atlas atlasHead;
static Atlas *atlasTail;
static Texture *atlasTexture;
static int atlasSize;

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
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	SDL_QueryTexture(atlasTexture->texture, NULL, NULL, &atlasSize, &atlasSize);
}

static void loadAtlasData(void)
{
	cJSON *root, *node;
	char *text;
	
	text = readFile("data/atlas/atlas.json");

	root = cJSON_Parse(text);
	
	for (node = cJSON_GetObjectItem(root, "images")->child ; node != NULL ; node = node->next)
	{
		loadImageData(node);
	}
	
	cJSON_Delete(root);
	
	free(text);
}

static void loadImageData(cJSON *root)
{
	Atlas *atlas;
	char *filename;
	int x, y, w, h;
	
	filename = cJSON_GetObjectItem(root, "filename")->valuestring;
	x = cJSON_GetObjectItem(root, "x")->valueint;
	y = cJSON_GetObjectItem(root, "y")->valueint;
	w = cJSON_GetObjectItem(root, "w")->valueint;
	h = cJSON_GetObjectItem(root, "h")->valueint;
	
	atlas = malloc(sizeof(Atlas));
	memset(atlas, 0, sizeof(Atlas));
	atlasTail->next = atlas;
	atlasTail = atlas;
	
	STRNCPY(atlas->filename, filename, MAX_FILENAME_LENGTH);
	atlas->rect.x = x;
	atlas->rect.y = y;
	atlas->rect.w = w;
	atlas->rect.h = h;
}
