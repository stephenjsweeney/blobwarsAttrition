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

static void tick(void);
static void action(void);
static void touch(Entity *other);

void initPowerPool(Entity *e)
{
	e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("PowerPool");

	e->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_IGNORE_BULLETS;

	e->plane = PLANE_FOREGROUND;

	e->isStatic = 1;
	
	e->tick = tick;
	e->action = action;
	e->touch = touch;
}

static void tick(void)
{
	if (self->active)
	{
		self->spriteTime--;
		if (self->spriteTime <= 0)
		{
			self->spriteFrame = (int) (1 + (rand() % 6));
			self->spriteTime = 12;
		}
	}
}

static void action(void)
{
	self->active = 1;

	if (self->spriteFrame == 0)
	{
		self->spriteFrame = (int) (1 + (rand() % 6));
		self->spriteTime = 12;
	}

	self->thinkTime = FPS * 99999;
}

static void touch(Entity *other)
{
	if (self->active && other->type == ET_BOB && world.bob->power < world.bob->powerMax)
	{
		world.bob->power = MIN(world.bob->power + 0.05, world.bob->powerMax);

		if (world.bob->power == world.bob->powerMax)
		{
			self->thinkTime = FPS * 10;
			self->spriteTime = -1;
			self->spriteFrame = 0;
			self->active = 0;
		}
	}
}
