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

#include "weaponPickup.h"

static void (*itemTick)(void);
static void tick(void);
static void touch(Entity *other);

static char *description[] = {
	"Pistol",
	"Plasma Rifle",
	"Spread Gun",
	"Laser Cannon",
	"Grenades"
};

void initWeaponPickup(Entity *e)
{
	initConsumable(e);

	e->weaponType = WPN_PISTOL;

	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("Weapon");
	e->spriteFrame = e->weaponType;
	e->spriteTime = -1;
	
	setEntitySize(e);

	if (e->provided)
	{
		e->health = 9999;
	}

	itemTick = e->tick;

	e->tick = tick;
	e->touch = touch;
}

static void tick(void)
{
	itemTick();

	if (self->provided && self->alive == ALIVE_ALIVE)
	{
		self->health = 9999;
	}
}

static void touch(Entity *other)
{
	if (touchedPlayer(other))
	{
		world.bob->weaponType = self->weaponType;

		switch (self->weaponType)
		{
			case WPN_GRENADES:
				setGameplayMessage(MSG_STANDARD, "Got some Grenades");
				break;

			default:
				setGameplayMessage(MSG_STANDARD, "Picked up a %s", description[self->weaponType]);
				break;
		}

		pickupItem();

		playSound(SND_WEAPON, CH_ITEM);
	}
}
