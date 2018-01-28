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

static void throwItem(Item *i);
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
	Item *wpn;
	int type;
	
	wpn = malloc(sizeof(Item));
	memset(wpn, 0, sizeof(Item));
	world.entityTail->next = (Entity*)wpn;
	world.entityTail = (Entity*)wpn;
	
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

void dropRandomCherry(double x, double y)
{
	Item *i;
	double r;

	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));
	world.entityTail->next = (Entity*)i;
	world.entityTail = (Entity*)i;
	
	r = rand() % 100;

	if (r < 1)
	{
		STRNCPY(i->name, "bunch of cherries", MAX_NAME_LENGTH);
		i->value = 10;
		i->sprite[0] = i->sprite[1] = i->sprite[2] = cherrySprite[2];
	}
	else if (r < 10)
	{
		STRNCPY(i->name, "pair of cherries", MAX_NAME_LENGTH);
		i->value = 3;
		i->sprite[0] = i->sprite[1] = i->sprite[2] = cherrySprite[1];
	}
	else
	{
		STRNCPY(i->name, "small cherry", MAX_NAME_LENGTH);
		i->value = 1;
		i->sprite[0] = i->sprite[1] = i->sprite[2] = cherrySprite[0];
	}

	i->x = x;
	i->y = y;

	throwItem(i);
}

void dropBattery(double x, double y)
{
	Item *i;
	double r;

	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));
	world.entityTail->next = (Entity*)i;
	world.entityTail = (Entity*)i;

	r = rand() % 100;

	if (r < 1)
	{
		STRNCPY(i->name, "full battery", MAX_NAME_LENGTH);
		i->value = 4;
	}
	else if (r < 10)
	{
		STRNCPY(i->name, "battery", MAX_NAME_LENGTH);
		i->value = 3;
	}
	else if (r < 25)
	{
		STRNCPY(i->name, "used battery", MAX_NAME_LENGTH);
		i->value = 2;
	}
	else
	{
		STRNCPY(i->name, "weak battery", MAX_NAME_LENGTH);
		i->value = 1;
	}

	i->sprite[0] = i->sprite[1] = i->sprite[2] = batterySprite;
	i->spriteTime = -1;
	i->spriteFrame = i->value;
	
	i->x = x;
	i->y = y;

	throwItem(i);
}

void addRandomItems(double x, double y)
{
	if (rand() % 100 < 25)
	{
		dropRandomCherry(x, y);
	}

	if (rand() % 100 < 20)
	{
		dropBattery(x, y);
	}
}

static void throwItem(Item *i)
{
	i->dx = rrnd(-3, 3);
	i->dy = rrnd(-7, -5);
}
