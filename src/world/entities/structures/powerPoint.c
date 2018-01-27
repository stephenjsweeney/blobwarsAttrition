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

static void tick(void);
static void action(void);
static void touch(Entity *other);

void initPowerPoint(Entity *e)
{
	initEntity(e);

	e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("PowerPoint");

	e->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	e->requiredPower = 100;

	e->isStatic = 1;
	
	e->tick = tick;
	e->action = action;
	e->touch = touch;
}

static void tick(void)
{
	if (!self->active)
	{
		self->bobTouching = MAX(self->bobTouching - 1, 0);

		if (self->bobTouching == 0)
		{
			self->spriteFrame = 0;
			self->spriteTime = -1;
			self->thinkTime = FPS / 2;
		}
	}
}

static void action(void)
{
	self->spriteFrame = MIN(self->spriteFrame + 1, 3);

	if (!self->active && self->spriteFrame == 3)
	{
		activateEntities(self->targetNames, 1);
		
		setGameplayMessage(MSG_GAMEPLAY, self->message);
		
		if (!dev.cheatPower)
		{
			world.bob->power -= self->requiredPower;
		}
		
		self->requiredPower = 0;
		
		self->active = 1;
	}
	else
	{
		self->thinkTime = FPS / 2;
	}
}

static void touch(Entity *other)
{
	if (!self->active && other->type == ET_BOB && other->dx == 0)
	{
		if (world.bob->power < self->requiredPower && self->bobTouching == 0 && !dev.cheatPower)
		{
			setGameplayMessage(MSG_GAMEPLAY, "Not enough power (%d units required)", self->requiredPower);
		}
		else
		{
			self->bobTouching = 2;
		}
	}
}
