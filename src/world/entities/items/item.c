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

#include "item.h"

static void reset(void);
static void tick(void);
static void touch(Entity *other);
static void changeEnvironment(void);
static void die(void);
static void destructablePickupItem(Entity *e);
static void enemyPickupItem(Entity *e);
static void bobPickupItem(void);

void initItem(Entity *e)
{
	initEntity(e);

	STRNCPY(e->spriteName, "Weapon", MAX_NAME_LENGTH);

	e->flags |= EF_IGNORE_BULLETS;

	e->isMissionTarget = 1;
	e->canBePickedUp = 1;
	e->canBeCarried = 0;
	e->collected = 0;

	e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex(e->spriteName);

	e->tick = tick;
	e->touch = touch;
	e->changeEnvironment = changeEnvironment;
	e->reset = reset;
	e->die = die;
}

static void reset(void)
{
	self->startX = (int) self->x;
	self->startY = (int) self->y;
}

static void tick(void)
{
	if (self->isOnGround)
	{
		self->dx *= 0.95;
	}

	/* hack, to ensure the size doesn't exceed the maxmium of something carrying it */
	self->w = self->w > 32 ? 32 : self->w;
}

static void touch(Entity *other)
{
	if (self->alive == ALIVE_ALIVE && other != NULL && self->canBePickedUp)
	{
		if (other->type == ET_BOB && !world.bob->stunTimer)
		{
			bobPickupItem();
		}
		else if (other->type == ET_ENEMY)
		{
			enemyPickupItem(other);
		}
		else if (other->type == ET_DESTRUCTABLE)
		{
			destructablePickupItem(other);
		}
	}
}

static void bobPickupItem(void)
{
	if (!self->isMissionTarget)
	{
		if (self->thinkTime == 0)
		{
			self->alive = ALIVE_DEAD;
			addKey(self->name);
			game.keysFound++;
			updateObjective("KEY");

			setGameplayMessage(MSG_STANDARD, "Picked up a %s", self->name);

			playSound(SND_KEY, CH_ITEM);
		}
		else
		{
			setGameplayMessage(MSG_GAMEPLAY, "Can't carry any more keys");
		}
	}
	else if (self->canBeCarried)
	{
		if (numCarriedItems() < MAX_ITEMS)
		{
			self->flags |= EF_GONE;

			if (!self->collected)
			{
				updateObjective(self->name);
				self->collected = 1;
			}

			addBobItem(self);
			
			setGameplayMessage(MSG_STANDARD, "Picked up a %s", self->name);

			playSound(SND_ITEM, CH_ITEM);
		}
		else
		{
			setGameplayMessage(MSG_GAMEPLAY, "Can't carry any more items");
		}
	}
	else
	{
		self->alive = ALIVE_DEAD;
		updateObjective(self->name);

		if (strcmp(world.id, "teeka") != 0)
		{
			setGameplayMessage(MSG_STANDARD, "Picked up a %s", self->name);
		}

		playSound(SND_ITEM, CH_ITEM);
	}
}

static void enemyPickupItem(Entity *e)
{
	if (e->canCarryItem && e->carriedItem == NULL && e->alive == ALIVE_ALIVE)
	{
		e->carriedItem = self;

		self->flags |= EF_GONE;
	}
}

static void destructablePickupItem(Entity *e)
{
	if (e->carriedItem == NULL && e->alive == ALIVE_ALIVE)
	{
		e->carriedItem = self;

		self->flags |= EF_GONE;
	}
}

static void changeEnvironment(void)
{
	if (self->environment == ENV_SLIME || self->environment == ENV_LAVA)
	{
		addTeleportStars(self);
		self->x = self->startX;
		self->y = self->startY;
		addTeleportStars(self);
		playSound(SND_APPEAR, CH_ANY);
	}
}

static void die(void)
{
	/* we will handle this ourselves! */
}
