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

#include "battery.h"

static void touch(Entity *other);

Item *initBattery(void)
{
	Item *i;
	
	i = initConsumable();
	
	i->spriteFrame = 0;
	i->spriteTime = -1;
	
	i->touch = touch;
	
	return i;
}

static void touch(Entity *other)
{
	Item *i;
	
	i = (Item*)self;
	
	if (touchedPlayer(other))
	{
		world.bob->power = MIN(world.bob->power + i->power, world.bob->powerMax);

		setGameplayMessage(MSG_STANDARD, _("Picked up a %s"), i->name);

		pickupItem();

		playSound(SND_ITEM, CH_ITEM);

		game.stats[STAT_BATTERIES_PICKED_UP]++;
	}
}
