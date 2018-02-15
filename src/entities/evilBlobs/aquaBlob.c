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

#include "aquaBlob.h"

static int canFire(Entity *target);

Entity *initAquaBlob(void)
{
	Unit *u;
	
	u = createEvilBlob();
	
	u->unitType = "AquaBlob";

	u->sprite[FACING_LEFT] = getSprite("AquaBlobLeft");
	u->sprite[FACING_RIGHT] = getSprite("AquaBlobRight");
	u->sprite[FACING_DIE] = getSprite("AquaBlobSpin");

	u->weaponType = WPN_AIMED_PISTOL;

	u->flags |= EF_SWIMS | EF_WATER_BREATHING;

	u->maxShotsToFire = 3;

	u->canFire = canFire;
	
	return (Entity*)u;
}

static int canFire(Entity *target)
{
	return 1;
}
