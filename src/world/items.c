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

static Sprite *wpnIconSprite;
static Sprite *cherrySprite[3];
static Sprite *batterySprite;

void initItems(void)
{
	wpnIconSprite = getSprite("Weapon");
	batterySprite = getSprite("Battery");

	cherrySprite[0] = getSprite("Cherry");
	cherrySprite[1] = getSprite("DualCherry");
	cherrySprite[2] = getSprite("CherryBunch");
}

void addRandomWeapon(int x, int y)
{
	Item *i;
	int type;

	i = initWeaponPickup();
	
	i->x = x;
	i->y = y;
	
	type = getRandomPlayerWeaponAt(i->x, i->y);
	
	i->weaponType = type;
	i->sprite[0] = i->sprite[1] = i->sprite[2] = wpnIconSprite;
	i->spriteFrame = type;
	i->spriteTime = -1;

	throwItem(i);
}

static int getRandomPlayerWeaponAt(int x, int y)
{
	int type;
	
	type = getRandomPlayerWeapon(world.missionType == MT_BOSS);
	
	if (world.map.data[(x / MAP_TILE_SIZE)][(y / MAP_TILE_SIZE)] == MAP_TILE_WATER)
	{
		type = WPN_PISTOL;
	}
	else if (type == WPN_PISTOL && rand() % 100 < 65)
	{
		type = getRandomPlayerWeapon(world.missionType == MT_BOSS);
	}
	
	return type;
}

void dropRandomCherry(int x, int y)
{
	Item *i;
	int r;

	i = initCherry();
	
	r = rand() % 100;

	if (r < 1)
	{
		STRNCPY(i->name, _("bunch of cherries"), MAX_NAME_LENGTH);
		i->value = 10;
		i->sprite[0] = i->sprite[1] = i->sprite[2] = cherrySprite[2];
	}
	else if (r < 10)
	{
		STRNCPY(i->name, _("pair of cherries"), MAX_NAME_LENGTH);
		i->value = 3;
		i->sprite[0] = i->sprite[1] = i->sprite[2] = cherrySprite[1];
	}
	else
	{
		STRNCPY(i->name, _("small cherry"), MAX_NAME_LENGTH);
		i->value = 1;
		i->sprite[0] = i->sprite[1] = i->sprite[2] = cherrySprite[0];
	}

	i->x = x;
	i->y = y;
	i->spriteFrame = 0;

	throwItem(i);
}

static void dropBattery(int x, int y)
{
	Item *i;
	int r;
	
	i = initBattery();

	r = rand() % 100;

	if (r < 1)
	{
		STRNCPY(i->name, _("full battery"), MAX_NAME_LENGTH);
		i->power = 4;
	}
	else if (r < 10)
	{
		STRNCPY(i->name, _("battery"), MAX_NAME_LENGTH);
		i->power = 3;
	}
	else if (r < 25)
	{
		STRNCPY(i->name, _("used battery"), MAX_NAME_LENGTH);
		i->power = 2;
	}
	else
	{
		STRNCPY(i->name, _("weak battery"), MAX_NAME_LENGTH);
		i->power = 1;
	}

	i->sprite[0] = i->sprite[1] = i->sprite[2] = batterySprite;
	i->spriteTime = -1;
	i->spriteFrame = i->power;
	
	i->x = x;
	i->y = y;
	
	i->animate();

	throwItem(i);
}

void addRandomItems(int x, int y)
{
	if (rand() % 100 < 25)
	{
		dropRandomCherry(x, y);
	}

	if (rand() % 100 < 50)
	{
		dropBattery(x, y);
	}
}

static void throwItem(Item *i)
{
	i->dx = rrnd(-3, 3);
	i->dy = rrnd(-7, -5);
}
