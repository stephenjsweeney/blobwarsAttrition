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

#include "grenadeBlob.h"

static void preFire2(void);
static int canFire(Entity *target);

void initGrenadeBlob(Unit *u)
{
	initEvilBlob(u);
	
	u->sprite[FACING_LEFT] = getSpriteIndex("GrenadeBlobLeft");
	u->sprite[FACING_RIGHT] = getSpriteIndex("GrenadeBlobRight");
	u->sprite[FACING_DIE] = getSpriteIndex("GrenadeBlobSpin");

	u->weaponType = WPN_GRENADES;

	u->preFire = preFire2;
	u->canFire = canFire;
}

static void preFire2(void)
{
	preFire(this);

	if (u->shotsToFire == 0)
	{
		if (rand() % 100 < 25)
		{
			u->weaponType = WPN_AIMED_PISTOL;
		}
		else
		{
			u->weaponType = WPN_GRENADES;
		}
	}
}

static int canFire(Entity *target)
{
	return abs(target->y - self->y) <= MAP_TILE_SIZE * 2;
}
