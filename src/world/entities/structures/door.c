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

#include "door.h"

static void tick(void);
static void touch(Entity *other);
static void openWithKey(void);
static int isClosed(void);
static int isOpening(void);
static int isClosing(void);

void initDoor(Entity *e)
{
	initEntity(e);
	
	e->isSolid = 1;

	e->flags |= EF_WEIGHTLESS | EF_NO_ENVIRONMENT | EF_NO_CLIP | EF_EXPLODES | EF_NO_TELEPORT;

	e->closedX = e->closedY = -1;

	e->state = DOOR_CLOSED;

	e->speed = 8;

	e->isLocked = 1;

	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("Door");
	
	if (e->closedX == -1 && e->closedY == -1)
	{
		e->closedX = (int) e->x;
		e->closedY = (int) e->y;
	}
	
	e->tick = tick;
	e->touch = touch;
}

void initBronzeDoor(Entity *e)
{
	initDoor(e);
	
	STRNCPY(e->requiredKey, "Bronze Key", MAX_NAME_LENGTH);

	e->speed = 2;

	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("BronzeDoor");
}

void initSilverDoor(Entity *e)
{
	initDoor(e);
	
	STRNCPY(e->requiredKey, "Silver Key", MAX_NAME_LENGTH);

	e->speed = 2;

	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("SilverDoor");
}

void initGoldDoor(Entity *e)
{
	initDoor(e);
	
	STRNCPY(e->requiredKey, "Gold Key", MAX_NAME_LENGTH);

	e->speed = 2;

	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("GoldDoor");
}

static void tick(void)
{
	self->dx = self->dy = 0;

	if (isOpening())
	{
		getSlope(self->tx, self->ty, self->x, self->y, &self->dx, &self->dy);

		self->dx *= self->speed;
		self->dy *= self->speed;

		if (abs(self->x - self->tx) < self->speed && abs(self->y - self->ty) < self->speed)
		{
			self->x = self->tx;
			self->y = self->ty;
		}

		self->isStatic = 0;
	}
	else if (isClosing())
	{
		getSlope(self->closedX, self->closedY, self->x, self->y, &self->dx, &self->dy);

		self->dx *= self->speed;
		self->dy *= self->speed;

		if (abs(self->x - self->closedX) < self->speed && abs(self->y - self->closedY) < self->speed)
		{
			self->x = self->closedX;
			self->y = self->closedY;
		}

		self->isStatic = 0;
	}

	if (self->dx == 0 && self->dy == 0 && !self->isStatic)
	{
		self->isStatic = 1;

		playSound(SND_DOOR_FINISH, CH_MECHANICAL);
	}
}

static void touch(Entity *other)
{
	if ((other->type != ET_BOB && self->isLocked) || (other->type != ET_BOB && other->type != ET_ENEMY))
	{
		return;
	}

	if (self->isLocked && !dev.cheatKeys)
	{
		if (isClosed())
		{
			if (strlen(self->requiredKey) != 0)
			{
				openWithKey();
			}
			else if (self->thinkTime == 0)
			{
				setGameplayMessage(MSG_GAMEPLAY, "Door is locked");

				playSound(SND_DENIED, CH_MECHANICAL);
			}

			self->thinkTime = 2;

			return;
		}
		else if (isClosing())
		{
			return;
		}
	}

	if (self->state != DOOR_OPEN)
	{
		playSound(SND_DOOR_START, CH_MECHANICAL);
	}

	self->state = DOOR_OPEN;
}

static void openWithKey(void)
{
	if (hasItem(self->requiredKey))
	{
		if (self->thinkTime <= 0)
		{
			setGameplayMessage(MSG_GAMEPLAY, "%s required", self->requiredKey);

			playSound(SND_DENIED, CH_MECHANICAL);
		}

		self->thinkTime = 2;

		return;
	}

	removeItem(self->requiredKey);

	setGameplayMessage(MSG_GAMEPLAY, "%s removed", self->requiredKey);

	STRNCPY(self->requiredKey, "", MAX_NAME_LENGTH);
	self->isLocked = 0;

	if (self->state != DOOR_OPEN)
	{
		playSound(SND_DOOR_START, CH_MECHANICAL);
	}

	self->state = DOOR_OPEN;
}

static int isOpening(void)
{
	return (self->state == DOOR_OPEN && ((int) self->x != (int) self->tx || (int) self->y != (int) self->ty));
}

static int isClosing(void)
{
	return (self->state == DOOR_CLOSED && ((int) self->x != self->closedX || (int) self->y != self->closedY));
}

static int isClosed(void)
{
	return (self->state == DOOR_CLOSED && ((int) self->x == self->closedX && (int) self->y == self->closedY));
}
