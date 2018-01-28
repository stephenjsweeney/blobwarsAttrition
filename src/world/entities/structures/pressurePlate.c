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
	Structure *s;
	
	initEntity(e);
	
	s = (Structure*)e;
	
	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSpriteIndex("PressurePlate");

	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	s->plane = PLANE_FOREGROUND;

	s->isStatic = 1;
	
	s->tick = tick;
	s->touch = touch;
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

			playSound(SND_PRESSURE_PLATE, CH_MECHANICAL);
		}

		s->active = 1;
		s->spriteFrame = 1;
		s->weightApplied = 5;
	}
}
