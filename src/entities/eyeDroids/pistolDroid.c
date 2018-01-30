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

#include "pistolDroid.h"

static int canFire(Entity *target);

Entity *initPistolDroid(void)
{
	Unit *u;
	
	u = createUnit();
	
	initEyeDroid(u);
	
	u->sprite[FACING_LEFT] = getSprite("PistolDroidLeft");
	u->sprite[FACING_RIGHT] = getSprite("PistolDroidRight");
	u->sprite[FACING_DIE] = getSprite("PistolDroidDie");

	u->weaponType = WPN_AIMED_PISTOL;

	u->maxShotsToFire = 3;

	u->canFire = canFire;
	
	return (Entity*)u;
}

static int canFire(Entity *target)
{
	return 1;
}
