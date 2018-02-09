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

#include "grenadeDroid.h"

static void (*superPreFire)(void);
static void preFire(void);
static int canFire(Entity *target);

Entity *initGrenadeDroid(void)
{
	Unit *u;
	
	u = createUnit();
	
	initEyeDroid(u);
	
	u->unitType = "GrenadeEyeDroid";
	
	u->sprite[FACING_LEFT] = getSprite("GrenadeDroidLeft");
	u->sprite[FACING_RIGHT] = getSprite("GrenadeDroidRight");
	u->sprite[FACING_DIE] = getSprite("GrenadeDroidDie");

	u->weaponType = WPN_GRENADES;
	u->maxShotsToFire = 1;
	
	superPreFire = u->preFire;

	u->preFire = preFire;
	u->canFire = canFire;
	
	return (Entity*)u;
}

static void preFire(void)
{
	Unit *u;

	u = (Unit*)self;

	superPreFire();

	if (u->shotsToFire == 0)
	{
		if (rand() % 100 < 25)
		{
			u->weaponType = WPN_AIMED_PISTOL;
			u->maxShotsToFire = 3;
		}
		else
		{
			u->weaponType = WPN_GRENADES;
			u->maxShotsToFire = 1;
		}
	}
}

static int canFire(Entity *target)
{
	return abs(target->y - self->y) <= MAP_TILE_SIZE * 2;
}
