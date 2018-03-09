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

#include "cell.h"

static void touch(Entity *other);

Entity *initCell(void)
{
	Item *i;
	
	i = (Item*)createItem();
	
	i->type = ET_CELL;

	i->isMissionTarget = 1;

	STRNCPY(i->spriteName, "Battery", MAX_NAME_LENGTH);

	i->sprite[0] = i->sprite[1] = i->sprite[2] = getSprite("Battery");

	i->spriteFrame = 0;
	i->spriteTime = -1;

	i->touch = touch;
	
	return (Entity*)i;
}

static void touch(Entity *other)
{
	if (other != NULL && other->type == ET_BOB && self->alive == ALIVE_ALIVE)
	{
		game.cells++;

		game.stats[STAT_CELLS_FOUND]++;

		world.bob->power = world.bob->powerMax = game.cells;

		setGameplayMessage(MSG_OBJECTIVE, _("Found a battery cell - Max power increased!"));

		playSound(SND_HEART_CELL, self->uniqueId % MAX_SND_CHANNELS);

		self->alive = ALIVE_DEAD;

		updateHeartCellObjective();
	}
}
