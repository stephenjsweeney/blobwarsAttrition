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

#include "tankTrack.h"

static Entity *tankCommander;
static void tick(void);
static void touch(Entity *other);
static SDL_Rect *getBounds(void);
static void animate(void);
static void (*superAnimate)(void);

void initTankTrack(Entity *e, Entity *tank)
{
	initBoss(e);

	superAnimate = e->animate;

	e->flags |= EF_EXPLODES | EF_NO_CLIP | EF_WEIGHTLESS | EF_IMMUNE;

	e->isMissionTarget = 0;

	e->sprite[FACING_LEFT] = getSprite("TankTrackLeft");
	e->sprite[FACING_RIGHT] = getSprite("TankTrackRight");
	e->sprite[FACING_DIE] = getSprite("TankTrackLeft");

	e->tick = tick;
	e->touch = touch;
	e->getBounds = getBounds;
	e->animate = animate;

	tankCommander = tank;
}

static void tick(void)
{
	if (self->health > 0)
	{
		self->facing = tankCommander->facing;

		if (self->facing == FACING_LEFT)
		{
			self->x = tankCommander->x + 10;
		}
		else
		{
			self->x = tankCommander->x - 60;
		}

		self->y = tankCommander->y + 66;
	}
}

static void touch(Entity *other)
{
	if (other != NULL)
	{
		if (other == (Entity*)world.bob && !world.bob->stunTimer && (world.bob->flags & EF_IMMUNE) == 0)
		{
			playSound(SND_FLESH_HIT, CH_ANY);
			world.bob->dx = rrnd(-5, 5);
			world.bob->dy = JUMP_POWER;
			world.bob->applyDamage(3);
			world.bob->stunTimer = 1;
		}

		if (other->type == ET_ENEMY)
		{
			other->health = 0;
		}
	}
}

static SDL_Rect *getBounds(void)
{
	self->bounds.x = self->x + 16;
	self->bounds.y = self->y;
	self->bounds.w = self->w - 32;
	self->bounds.h = self->h;

	return &self->bounds;
}

static void animate(void)
{
	if (tankCommander->dx != 0)
	{
		superAnimate();
	}
}
