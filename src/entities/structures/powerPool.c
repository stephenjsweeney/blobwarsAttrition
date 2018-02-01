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

#include "powerPool.h"

static void init(void);
static void tick(void);
static void action(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initPowerPool(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_POOL;
	
	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("PowerPool");

	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_IGNORE_BULLETS;

	s->plane = PLANE_FOREGROUND;

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
	/* nothing to do */
}

static void tick(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (s->active)
	{
		s->spriteTime--;
		if (s->spriteTime <= 0)
		{
			s->spriteFrame = (int) (1 + (rand() % 6));
			s->spriteTime = 12;
		}
	}
}

static void action(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->active = 1;

	if (s->spriteFrame == 0)
	{
		s->spriteFrame = (int) (1 + (rand() % 6));
		s->spriteTime = 12;
	}

	s->thinkTime = FPS * 99999;
}

static void touch(Entity *other)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (s->active && other->type == ET_BOB && world.bob->power < world.bob->powerMax)
	{
		world.bob->power = MIN(world.bob->power + 0.05, world.bob->powerMax);

		if (world.bob->power == world.bob->powerMax)
		{
			s->thinkTime = FPS * 10;
			s->spriteTime = -1;
			s->spriteFrame = 0;
			s->active = 0;
		}
	}
}

static void load(cJSON *root)
{
	/* nothing to do */
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "type", "PowerPool");
}
