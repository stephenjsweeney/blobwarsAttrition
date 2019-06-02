/*
Copyright (C) 2018-2019 Parallel Realities

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

#include "sprites.h"

static void animateSprite(Sprite *s);
static void loadGameSprites(void);
static void loadSpriteList(char *filename);
void loadSprite(cJSON *root);

static Sprite spriteHead;
static Sprite *spriteTail;

void initSprites(void)
{
	memset(&spriteHead, 0, sizeof(Sprite));
	spriteTail = &spriteHead;

	loadGameSprites();
}

Sprite *getSprite(char *name)
{
	Sprite *s;

	for (s = spriteHead.next ; s != NULL ; s = s->next)
	{
		if (strcmp(s->name, name) == 0)
		{
			return s;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such sprite '%s'", name);
	exit(1);

	return NULL;
}

void animateSprites(void)
{
	Sprite *s;

	for (s = spriteHead.next ; s != NULL ; s = s->next)
	{
		animateSprite(s);
	}
}

static void animateSprite(Sprite *s)
{
	if (s->currentTime != -1)
	{
		s->currentTime--;

		if (s->currentTime <= 0)
		{
			s->currentFrame = wrap(s->currentFrame + 1, 0, s->numFrames - 1);
			s->currentTime = s->times[s->currentFrame];
		}
	}
}

SDL_Rect *getCurrentFrame(Sprite *s)
{
	return &s->frames[s->currentFrame]->rect;
}

static void loadGameSprites(void)
{
	char **filenames;
	char path[MAX_FILENAME_LENGTH];
	int count, i;

	filenames = getFileList("data/sprites", &count);

	for (i = 0 ; i < count ; i++)
	{
		sprintf(path, "data/sprites/%s", filenames[i]);

		loadSpriteList(path);

		free(filenames[i]);
	}

	free(filenames);
}

static void loadSpriteList(char *filename)
{
	cJSON *root, *node;
	char *text;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	text = readFile(filename);
	root = cJSON_Parse(text);

	for (node = root->child ; node != NULL ; node = node->next)
	{
		loadSprite(node);
	}

	cJSON_Delete(root);

	free(text);
}

void loadSprite(cJSON *root)
{
	Sprite *s;
	cJSON *frame;
	char *filename;
	int i;

	s = malloc(sizeof(Sprite));
	memset(s, 0, sizeof(Sprite));
	spriteTail->next = s;
	spriteTail = s;

	STRNCPY(s->name, cJSON_GetObjectItem(root, "name")->valuestring, MAX_NAME_LENGTH);

	for (frame = cJSON_GetObjectItem(root, "frame")->child ; frame != NULL ; frame = frame->next)
	{
		s->numFrames++;
	}

	s->times = malloc(sizeof(int) * s->numFrames);
	s->filenames = malloc(sizeof(char*) * s->numFrames);
	s->frames = malloc(sizeof(Atlas*) * s->numFrames);

	i = 0;

	for (frame = cJSON_GetObjectItem(root, "frame")->child ; frame != NULL ; frame = frame->next)
	{
		s->times[i] = cJSON_GetObjectItem(frame, "time")->valueint;

		if (cJSON_GetObjectItem(frame, "content") != NULL)
		{
			filename = cJSON_GetObjectItem(frame, "content")->valuestring;
			s->filenames[i] = malloc(strlen(filename) + 1);
			STRNCPY(s->filenames[i], filename, strlen(filename));

			s->frames[i] = getImageFromAtlas(filename);
		}

		i++;
	}

	s->w = s->frames[0]->rect.w;
	s->h = s->frames[0]->rect.h;
}

void destroySprites(void)
{
}
