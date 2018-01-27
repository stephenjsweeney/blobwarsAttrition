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

#include "laserTrap.h"

static void init(void);
static void tick(void);
static void action(void);
static void touch(Entity *other);
static void activate(int active);

void initLaserTrap(Entity *e)
{
	initEntity(e);
	
	e->flags |= EF_WEIGHTLESS | EF_IGNORE_BULLETS | EF_NO_ENVIRONMENT | EF_NO_CLIP | EF_ALWAYS_PROCESS;

	e->onTime = FPS * 2;
	e->offTime = FPS * 2;

	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("LaserTrap");

	e->active = 1;
	
	e->init = init;
	e->tick = tick;
	e->action = action;
	e->touch = touch;
	e->activate = activate;
}

static void init(void)
{
	if (!self->active)
	{
		self->flags |= EF_GONE;
	}
}

static void tick(void)
{
	if (!self->active && self->spriteTime == -1)
	{
		self->flags |= EF_GONE;
	}
}

static void action(void)
{
	if (self->offTime != 0)
	{
		if (!self->active)
		{
			self->thinkTime = self->onTime;
			self->spriteFrame = 0;
			self->spriteTime = 0;
			self->flags &= ~EF_GONE;
		}
		else if (self->offTime > 0)
		{
			self->thinkTime = self->offTime;
			self->spriteTime = 0;
		}

		self->active = !self->active;
	}
	else
	{
		self->spriteTime = -1;
		self->spriteFrame = 4;
	}
}

static void touch(Entity *other)
{
	if (other != NULL && (other->type == ET_BOB || other->type == ET_ENEMY))
	{
		if (!(other->flags & EF_IMMUNE))
		{
			if (other->health > 0 || rand() % 100 < 5)
			{
				other->dx = rrnd(-12, 12);
				other->dy = rrnd(-8, 0);

				if (self->offTime != 0)
				{
					other->applyDamage((int) (other->healthMax / 4));
				}
				else
				{
					/* instant kill */
					other->applyDamage((int) other->health);
				}
			}

			if (other == world.bob && world.bob->stunTimer == 0)
			{
				stunPlayer();
			}
		}

		if (other->flags & EF_EXPLODES)
		{
			addSparkParticles(self->x, self->y);
		}
		else
		{
			addSmallFleshChunk(self->x, self->y);
		}
	}
}

static void activate(int active)
{
	self->active = !active;

	if (!self->active)
	{
		self->flags |= EF_GONE;
	}
	else
	{
		self->flags &= ~EF_GONE;
	}

	if (!self->active)
	{
		observeActivation(self);

		if (!isOnScreen(self))
		{
			setGameplayMessage(MSG_GAMEPLAY, "Lasers disabled ...");
		}
	}
}
