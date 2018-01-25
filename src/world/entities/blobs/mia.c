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

#include "mia.h"

static void reset(void);
static void tick(void);
static void touch(Entity *other);
static void preTeleport(void);
static void teleport(void);
static void addRescuedMIA(void);

void initMIA(Entity *e)
{
	e->tx = e->ty = -1;

	e->sprite[FACING_LEFT] = getSpriteIndex("MIA");
	e->sprite[FACING_RIGHT] = getSpriteIndex("MIA");
	e->sprite[FACING_DIE] = getSpriteIndex("MIA");

	e->flags |= EF_IGNORE_BULLETS;

	/* blink at random intervals */
	e->spriteFrame = 0;
	e->spriteTime = rand() % 180;

	e->currentAction = nothing;
	e->reset = reset;
	e->tick = tick;
	e->touch = touch;

	e->isMissionTarget = 1;
}

void reinitMIA(Entity *e)
{
	if (e->tx == -1 && e->ty == -1)
	{
		e->tx = e->x;
		e->ty = e->y;
	}
}

static void reset(void)
{
	self->tx = self->ty = -1;
}

static void tick(void)
{
	self->shudderTimer--;
	if (self->shudderTimer <= 0)
	{
		self->x = (self->tx + rand() % 4);
		self->shudderTimer = 2;
	}

	if (self->currentAction != nothing)
	{
		self->starTimer--;
		if (self->starTimer <= 0)
		{
			addMIATeleportStars(self->x + rand() % self->w, self->y + rand() % self->h);
			self->starTimer = 1;
		}
	}
}

static void touch(Entity *other)
{
	if (self->currentAction == nothing && other == world.bob)
	{
		self->currentAction = preTeleport;
		self->teleportTimer = FPS * 3;
		setGameplayMessage(MSG_OBJECTIVE, "Rescued %s", self->name);
		self->isMissionTarget = 0;
		self->flags |= EF_ALWAYS_PROCESS;
		playSound(SND_MIA, CH_ANY);
	}
}

static void preTeleport(void)
{
	self->teleportTimer--;
	if (self->teleportTimer <= FPS)
	{
		self->currentAction = teleport;
		self->flags |= (EF_NO_CLIP | EF_WEIGHTLESS);
		self->dy = -5;
	}
}

static void teleport(void)
{
	self->teleportTimer--;
	if (self->teleportTimer <= 0)
	{
		addTeleportStars(self);
		addRescuedMIA();
		updateObjective("MIA");
		self->alive = ALIVE_DEAD;
	}
}

static void addRescuedMIA(void)
{
	int i;
	
	for (i = 0 ; i < game.totalMIAs ; i++)
	{
		if (strcmp(game.mias[i], "") == 0)
		{
			STRNCPY(game.mias[i], self->name, MAX_NAME_LENGTH);
			return;
		}
	}
}
