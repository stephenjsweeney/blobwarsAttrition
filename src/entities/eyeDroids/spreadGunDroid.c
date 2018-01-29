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

#include "spreadGunDroid.h"

static int canFire(Entity *target);

void initSpreadGunDroid(Unit *u)
{
	initEyeDroid(u);

	u->sprite[FACING_LEFT] = getSpriteIndex("SpreadGunDroidLeft");
	u->sprite[FACING_RIGHT] = getSpriteIndex("SpreadGunDroidRight");
	u->sprite[FACING_DIE] = getSpriteIndex("SpreadGunDroidDie");

	u->weaponType = WPN_SPREAD;

	u->maxShotsToFire = 3;

	u->canFire = canFire;
}

static int canFire(Entity *target)
{
	return abs(target->y - self->y) <= MAP_TILE_SIZE * 5;
}
