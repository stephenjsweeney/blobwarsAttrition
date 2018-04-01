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

#include "powerPoint.h"

static void init(void);
static void tick(void);
static void action(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initPowerPoint(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_POWER_POINT;

	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("PowerPoint");

	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	s->requiredPower = 100;

	s->isStatic = 1;
	
	s->init = init;
	s->tick = tick;
	s->action = action;
	s->touch = touch;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (s->requiredPower == 100 && game.cells != 0)
	{
		s->requiredPower = rrnd(game.cells * 0.7, game.cells * 0.9);
	}
	else if (s->requiredPower == 0)
	{
		s->spriteFrame = 3;
		s->active = 1;
	}
}

static void tick(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (!s->active)
	{
		s->bobTouching = MAX(s->bobTouching - 1, 0);

		if (s->bobTouching == 0)
		{
			s->spriteFrame = 0;
			s->spriteTime = -1;
			s->thinkTime = FPS / 2;
		}
	}
}

static void action(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->spriteFrame = MIN(s->spriteFrame + 1, 3);

	if (!s->active && s->spriteFrame == 3)
	{
		activateEntities(s->targetNames, 1);
		
		setGameplayMessage(MSG_GAMEPLAY, s->message);
		
		if (!dev.cheatPower)
		{
			world.bob->power -= s->requiredPower;
		}
		
		s->requiredPower = 0;
		
		s->active = 1;
	}
	else
	{
		s->thinkTime = FPS / 2;
	}
}

static void touch(Entity *other)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (!s->active && other->type == ET_BOB && other->dx == 0)
	{
		if (world.bob->power < s->requiredPower && s->bobTouching == 0 && !dev.cheatPower)
		{
			setGameplayMessage(MSG_GAMEPLAY, app.strings[ST_POWER_POINT], s->requiredPower);
		}
		else
		{
			s->bobTouching = 2;
		}
	}
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->requiredPower = cJSON_GetObjectItem(root, "requiredPower")->valueint;
	STRNCPY(s->targetNames, cJSON_GetObjectItem(root, "targetNames")->valuestring, MAX_DESCRIPTION_LENGTH);
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "PowerPoint");
	cJSON_AddNumberToObject(root, "requiredPower", s->requiredPower);
	cJSON_AddStringToObject(root, "targetNames", s->targetNames);
}
