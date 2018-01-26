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

#include "unit.h"

void unitTick(void);
static void attack(void);

void initUnit(Entity *e)
{
	initEntity(e);
	
	e->oxygen = MAX_OXYGEN;

	e->canCarryItem = rand() % 100 < 85;

	if (world.isOutpostMission)
	{
		e->canCarryItem = 1;
		e->health = e->healthMax = rrnd(1, 4);
	}

	e->spriteFrame = 0;

	e->startX = e->startY = -1;

	e->tick = unitTick;
	e->action = lookForPlayer;
	e->attack = attack;
}

void reInitUnit(Entity *e)
{
	if (e->startX == -1 && e->startY == -1)
	{
		e->startX = (int) e->x;
		e->startY = (int) e->y;
	}

	if (e->isMissionTarget)
	{
		e->flags |= EF_BOMB_SHIELD;
	}
}

void unitTick(void)
{
	if (self->alive == ALIVE_ALIVE)
	{
		self->reload = limit(self->reload - 1, 0, FPS);
	}

	switch (self->environment)
	{
		case ENV_AIR:
			self->oxygen = limit(self->oxygen + 4, 0, MAX_OXYGEN);
			break;

		case ENV_WATER:
			self->oxygen = limit(self->oxygen - 1, 0, MAX_OXYGEN);
			if (self->oxygen == 0 && world.frameCounter % 30 == 0)
			{
				self->health--;
			}
			break;

		case ENV_SLIME:
		case ENV_LAVA:
			if (self->alive == ALIVE_ALIVE)
			{
				self->health = 0;
			}
			break;
	}

	if (self->flags & EF_WATER_BREATHING)
	{
		self->oxygen = MAX_OXYGEN;
	}

	if (self->spawnedIn)
	{
		if (getDistance(self->x, self->y, world.bob->x, world.bob->y) < 1000)
		{
			self->spawnedInTimer = FPS * 5;
		}

		self->spawnedInTimer--;
		if (self->spawnedInTimer <= 0)
		{
			self->alive = ALIVE_DEAD;
		}
	}
}

void dropCarriedItem(Entity *e)
{
	if (e->carriedItem != NULL)
	{
		e->carriedItem->x = (e->x + e->w / 2) - e->carriedItem->w / 2;
		e->carriedItem->y = e->y;

		e->carriedItem->dx = e->carriedItem->dy = 0;

		world.entityTail->next = e->carriedItem;
		world.entityTail = e->carriedItem;
		world.entityTail->next = NULL;

		e->carriedItem = NULL;
	}
}

void unitReappear(void)
{
}

static void attack(void)
{
	if (canFire(world.bob))
	{
		switch (self->weaponType)
		{
			case WPN_AIMED_PISTOL:
				fireAimedShot(self);
				break;

			case WPN_MACHINE_GUN:
				fireMachineGun(self);
				break;

			case WPN_GRENADES:
				fireGrenade(self);
				break;

			case WPN_PLASMA:
				firePlasma(self);
				break;

			case WPN_SPREAD:
				fireSpread(self, 3);
				break;

			case WPN_LASER:
				fireLaser(self);
				break;

			case WPN_SHOTGUN:
				fireShotgun(self);
				break;

			case WPN_MISSILE:
				fireMissile(self);
				break;

			default:
				printf("Can't fire weapon: %d\n", self->weaponType);
				break;
		}
	}
}


