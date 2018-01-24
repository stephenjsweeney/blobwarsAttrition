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

#include "sprites.h"

static void animateSprite(Sprite *s);

static Tuple spriteMapHead;
static Sprite spriteHead;
/*static Sprite *spriteTail;*/

int getSpriteIndex(char *key)
{
	Tuple *t;
	
	for (t = spriteMapHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, key) == 0)
		{
			return t->value.i;
		}
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such sprite '%s'", key);
	exit(1);

	return -1;
}

Sprite *getSpriteByIndex(int x)
{
	Sprite *s;
	int i;
	
	for (s = spriteHead.next ; s != NULL ; s = s->next)
	{
		if (i == x)
		{
			return s;
		}
		
		i++;
	}
	
	return NULL;
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

void addSpriteFrame(Sprite *s, SDL_Rect frame, int time)
{
	s->frames[s->numFrames] = frame;
	s->times[s->numFrames] = time;
	
	s->numFrames++;
}

static void animateSprite(Sprite *s)
{
	if (s->currentTime != -1)
	{
		s->currentTime--;

		if (s->currentTime <= 0)
		{
			s->currentFrame = wrap(++s->currentFrame, 0, s->numFrames - 1);
			s->currentTime = s->times[s->currentFrame];
		}
	}
}

SDL_Rect getCurrentFrame(Sprite *s)
{
	return s->frames[s->currentFrame];
}

void loadGameSprites(void)
{
	
}

void loadSpriteList(char *filename)
{
}

void loadSprites(cJSON *root)
{
	
}

void destroySprites(void)
{
}

void addSpriteFrames(Sprite *sprite, cJSON *root)
{
	
}
