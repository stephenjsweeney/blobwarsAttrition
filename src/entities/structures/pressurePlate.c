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

#include "pressurePlate.h"

static void init(void);
static void tick(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initPressurePlate(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_PRESSURE_PLATE;
	
	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("PressurePlate");

	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	s->plane = PLANE_FOREGROUND;

	s->isStatic = 1;
	
	s->init = init;
	s->tick = tick;
	s->touch = touch;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (s->active)
	{
		s->spriteTime = -1;
		s->spriteFrame = 1;

		if (s->isWeighted)
		{
			s->weightApplied = 5;
		}
	}
}

static void tick(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (s->isWeighted)
	{
		s->weightApplied = MAX(s->weightApplied - 1, 0);

		if (s->active && s->weightApplied == 0)
		{
			s->active = 0;
			s->spriteFrame = 0;

			activateEntities(s->targetNames, 0);
		}
	}
}

static void touch(Entity *other)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (other->type == ET_BOB || other->type == ET_PUSHBLOCK)
	{
		if (!s->active)
		{
			activateEntities(s->targetNames, 1);

			playBattleSound(SND_PRESSURE_PLATE, s->uniqueId % MAX_SND_CHANNELS, s->x, s->y);
		}

		s->active = 1;
		s->spriteFrame = 1;
		s->weightApplied = 5;
	}
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (cJSON_GetObjectItem(root, "active"))
	{
		s->active = cJSON_GetObjectItem(root, "active")->valueint;
	}
	s->isWeighted = cJSON_GetObjectItem(root, "isWeighted")->valueint;
	STRNCPY(s->targetNames, cJSON_GetObjectItem(root, "targetNames")->valuestring, MAX_DESCRIPTION_LENGTH);
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "PressurePlate");
	cJSON_AddNumberToObject(root, "active", s->active);
	cJSON_AddNumberToObject(root, "isWeighted", s->isWeighted);
	cJSON_AddStringToObject(root, "targetNames", s->targetNames);
}
