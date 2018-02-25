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

static void (*superTick)(void);
static void init(void);
static void tick(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initWeaponPickup(void)
{
	Item *i;
	
	i = (Item*)initConsumable();

	i->weaponType = WPN_PISTOL;
	
	superTick = i->tick;

	i->init = init;
	i->tick = tick;
	i->touch = touch;
	i->load = load;
	i->save = save;
	
	return (Entity*)i;
}

static void init(void)
{
	Item *i;
	
	i = (Item*)self;
	
	i->sprite[0] = i->sprite[1] = i->sprite[2] = getSprite("Weapon");
	i->spriteFrame = i->weaponType;
	i->spriteTime = -1;

	if (i->provided)
	{
		i->health = 9999;
	}
}

static void tick(void)
{
	Item *i;
	
	i = (Item*)self;
	
	superTick();

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
				setGameplayMessage(MSG_STANDARD, _("Picked up a %s"), getWeaponName(i->weaponType));
				break;
		}

		pickupItem();

		playSound(SND_WEAPON, i->uniqueId % MAX_SND_CHANNELS);

		game.stats[STAT_WEAPONS_PICKED_UP]++;
	}
}

static void load(cJSON *root)
{
	Item *i;
	
	i = (Item*)self;
	
	i->weaponType = lookup(cJSON_GetObjectItem(root, "weaponType")->valuestring);
	i->provided = cJSON_GetObjectItem(root, "provided")->valueint;
}

static void save(cJSON *root)
{
	/* boss missions, only - not going to worry about saving these */
}
