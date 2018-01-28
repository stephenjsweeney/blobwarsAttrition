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
static int canFire(Entity *target);

void initUnit(Entity *e)
{
	Unit *u;
	
	initEntity(e);
	
	u = (Unit*)e;
	
	u->oxygen = MAX_OXYGEN;

	u->canCarryItem = rand() % 100 < 85;

	if (world.isOutpostMission)
	{
		u->canCarryItem = 1;
		u->health = u->healthMax = rrnd(1, 4);
	}

	u->spriteFrame = 0;

	u->startX = u->startY = -1;

	u->tick = unitTick;
	u->action = lookForPlayer;
	u->attack = attack;
	u->canFire = canFire;
}

void reInitUnit(Entity *e)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (u->startX == -1 && u->startY == -1)
	{
		u->startX = (int) u->x;
		u->startY = (int) u->y;
	}

	if (u->isMissionTarget)
	{
		u->flags |= EF_BOMB_SHIELD;
	}
}

void unitTick(void)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (u->alive == ALIVE_ALIVE)
	{
		u->reload = limit(u->reload - 1, 0, FPS);
	}

	switch (u->environment)
	{
		case ENV_AIR:
			u->oxygen = limit(u->oxygen + 4, 0, MAX_OXYGEN);
			break;

		case ENV_WATER:
			u->oxygen = limit(u->oxygen - 1, 0, MAX_OXYGEN);
			if (u->oxygen == 0 && world.frameCounter % 30 == 0)
			{
				u->health--;
			}
			break;

		case ENV_SLIME:
		case ENV_LAVA:
			if (u->alive == ALIVE_ALIVE)
			{
				u->health = 0;
			}
			break;
	}

	if (u->flags & EF_WATER_BREATHING)
	{
		u->oxygen = MAX_OXYGEN;
	}

	if (u->spawnedIn)
	{
		if (getDistance(u->x, u->y, world.bob->x, world.bob->y) < 1000)
		{
			u->spawnedInTimer = FPS * 5;
		}

		u->spawnedInTimer--;
		if (u->spawnedInTimer <= 0)
		{
			u->alive = ALIVE_DEAD;
		}
	}
}

void unitReappear(void)
{
}

static void attack(void)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (u->canFire((Entity*)world.bob))
	{
		switch (u->weaponType)
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
				printf("Can't fire weapon: %d\n", u->weaponType);
				break;
		}
	}
}

static int canFire(Entity *target)
{
	return 0;
}
