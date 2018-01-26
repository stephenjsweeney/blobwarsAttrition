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

#include "consumable.h"

static void tick(void);
static void die(void);

void initConsumable(Entity *e)
{
	e->flags |= EF_IGNORE_BULLETS;

	e->health = FPS * 10;

	e->tick = tick;
	e->die = die;
}

static void tick(void)
{
	if (self->isOnGround)
	{
		self->dx *= 0.05;
	}

	self->health--;

	if ((int) self->health == FPS * 2)
	{
		self->flags |= EF_FLICKER;
	}
	else if (self->health <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

int touchedPlayer(Entity *other)
{
	return (other != NULL && other->type == ET_BOB && self->alive == ALIVE_ALIVE);
}

void pickupItem(void)
{
	self->alive = (self->environment == ENV_AIR) ? ALIVE_DYING : ALIVE_DEAD;
	self->health = FPS / 2;
	self->thinkTime = 0;
	self->dy = -2;
	self->dx = 0;
	self->flags |= EF_FLICKER | EF_WEIGHTLESS;
}

static void die(void)
{
	/* we will handle this ourselves! */
}
