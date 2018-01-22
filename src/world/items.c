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

#include "items.h"

static void throwItem(Entity *e);
static int getRandomPlayerWeaponAt(int x, int y);

static int wpnIconSprite;
static int cherrySprite[3];
static int batterySprite;

void initItems(void)
{
	wpnIconSprite = getSpriteIndex("Weapon");
	batterySprite = getSpriteIndex("Battery");

	cherrySprite[0] = getSpriteIndex("Cherry");
	cherrySprite[1] = getSpriteIndex("DualCherry");
	cherrySprite[2] = getSpriteIndex("CherryBunch");
}

void addRandomWeapon(double x, double y)
{
	Entity *wpn;
	int type;
	
	wpn = malloc(sizeof(Entity));
	memset(wpn, 0, sizeof(Entity));
	world.entityTail->next = wpn;
	world.entityTail = wpn;
	
	wpn->x = x;
	wpn->y = y;
	
	type = getRandomPlayerWeaponAt(wpn->x, wpn->y);
	
	switch (type)
	{
		case 0:
			wpn->touch = NULL;
			break;
			
		default:
			break;
	}
	
	wpn->sprite[0] = wpn->sprite[1] = wpn->sprite[2] = wpnIconSprite;
	wpn->spriteFrame = type;
	wpn->spriteTime = -1;

	throwItem(wpn);
}

static int getRandomPlayerWeaponAt(int x, int y)
{
	int type;
	
	type = getRandomPlayerWeapon(world.isBossMission);
	
	if (world.map.data[(x / MAP_TILE_SIZE)][(y / MAP_TILE_SIZE)] == MAP_TILE_WATER)
	{
		type = WPN_PISTOL;
	}
	else if (type == WPN_PISTOL && rand() < 0.25)
	{
		type = getRandomPlayerWeapon(world.isBossMission);
	}
	
	return type;
}

static void throwItem(Entity *e)
{
	e->dx = rrnd(-3, 3);
	e->dy = rrnd(-7, -5);
}
