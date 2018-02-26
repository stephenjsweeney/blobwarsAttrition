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

#include "pushBlock.h"

static void init(void);
static void activate(int active);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initPushBlock(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_PUSHBLOCK;
	
	s->isSolid = 1;

	s->startX = s->startY = -1;
	
	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("Crate1");

	s->flags |= EF_EXPLODES | EF_ALWAYS_PROCESS;
	
	s->init = init;
	s->activate = activate;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	sprintf(s->spriteName, "Crate%d", rrnd(1, 4));

	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite(s->spriteName);

	if (s->startX == -1 && s->startY == -1)
	{
		s->startX = s->x;
		s->startY = s->y;
	}
}

static void activate(int active)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (active)
	{
		addTeleportStars(self);
		s->x = s->startX;
		s->y = s->startY;
		s->dx = s->dy = 0;
		addTeleportStars(self);
		playBattleSound(SND_APPEAR, s->uniqueId % MAX_SND_CHANNELS, s->x, s->y);
	}
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	STRNCPY(s->spriteName, cJSON_GetObjectItem(root, "spriteName")->valuestring, MAX_NAME_LENGTH);
	s->startX = cJSON_GetObjectItem(root, "startX")->valueint;
	s->startY = cJSON_GetObjectItem(root, "startY")->valueint;
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "PushBlock");
	cJSON_AddStringToObject(root, "spriteName", s->spriteName);
	cJSON_AddNumberToObject(root, "startX", s->startX);
	cJSON_AddNumberToObject(root, "startY", s->startY);
}
