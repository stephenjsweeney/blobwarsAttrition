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

static char *description[WPN_ANY] = {
	"Pistol",
	"Plasma Rifle",
	"Spread Gun",
	"Laser Cannon",
	"Grenades"
};

void initWeaponPickup(Entity *e)
{
	Item *i;
	
	initConsumable(e);
	
	i = (Item*)e;

	i->weaponType = WPN_PISTOL;

	i->sprite[0] = i->sprite[1] = i->sprite[2] = getSprite("Weapon");
	i->spriteFrame = i->weaponType;
	i->spriteTime = -1;

	if (i->provided)
	{
		i->health = 9999;
	}

	itemTick = i->tick;

	i->tick = tick;
	i->touch = touch;
}

static void tick(void)
{
	Item *i;
	
	i = (Item*)self;
	
	itemTick();

	if (i->provided && i->alive == ALIVE_ALIVE)
	{
		i->health = 9999;
	}
}

static void touch(Entity *other)
{
	Item *i;
	
	i = (Item*)self;
	
	if (touchedPlayer(other))
	{
		world.bob->weaponType = i->weaponType;

		switch (i->weaponType)
		{
			case WPN_GRENADES:
				setGameplayMessage(MSG_STANDARD, _("Got some Grenades"));
				break;

			default:
				setGameplayMessage(MSG_STANDARD, _("Picked up a %s"), _(description[i->weaponType]));
				break;
		}

		pickupItem();

		playSound(SND_WEAPON, CH_ITEM);
	}
}