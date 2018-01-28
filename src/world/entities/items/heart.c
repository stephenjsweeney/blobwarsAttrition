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

#include "heart.h"

static void action(void);
static void touch(Entity *other);

void initHeart(Entity *e)
{
	Item *i;
	
	initItem(e);
	
	i = (Item*)e;

	i->isMissionTarget = 1;

	STRNCPY(i->spriteName, "Heart", MAX_NAME_LENGTH);

	i->sprite[0] = i->sprite[1] = i->sprite[2] = getSpriteIndex("Heart");

	i->spriteFrame = 0;
	i->spriteTime = -1;

	i->action = action;
	i->touch = touch;
}

static void action(void)
{
	if (self->isOnGround)
	{
		self->dy = -5;
	}

	self->thinkTime = FPS * rrnd(1, 3);
}

static void touch(Entity *other)
{
	if (other != NULL && other->type == ET_BOB && self->alive == ALIVE_ALIVE)
	{
		game.hearts++;

		world.bob->health = world.bob->healthMax = game.hearts;

		setGameplayMessage(MSG_OBJECTIVE, "Found a heart - Max health increased!");

		playSound(SND_HEART_CELL, CH_ITEM);

		self->alive = ALIVE_DEAD;

		updateObjective("HEART_CELL");
	}
}
