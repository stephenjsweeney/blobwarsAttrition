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

static void tick(void);
static void touch(Entity *other);

void initPressurePlate(Entity *e)
{
	initEntity(e);
	
	e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("PressurePlate");

	e->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	e->plane = PLANE_FOREGROUND;

	e->isStatic = 1;
	
	e->tick = tick;
	e->touch = touch;
}

static void tick(void)
{
	if (self->isWeighted)
	{
		self->weightApplied = MAX(self->weightApplied - 1, 0);

		if (self->active && self->weightApplied == 0)
		{
			self->active = 0;
			self->spriteFrame = 0;

			activateEntities(self->targetNames, 0);
		}
	}
}

static void touch(Entity *other)
{
	if (other->type == ET_BOB || other->type == ET_PUSHBLOCK)
	{
		if (!self->active)
		{
			activateEntities(self->targetNames, 1);

			playSound(SND_PRESSURE_PLATE, CH_MECHANICAL);
		}

		self->active = 1;
		self->spriteFrame = 1;
		self->weightApplied = 5;
	}
}
