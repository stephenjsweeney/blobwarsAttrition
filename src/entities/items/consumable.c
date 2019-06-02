/*
Copyright (C) 2018-2019 Parallel Realities

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

#include "consumable.h"

static void tick(void);
static void die(void);

Item *initConsumable(void)
{
	Item *i;

	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));

	initEntity((Entity*)i);

	i->type = ET_CONSUMABLE;

	i->flags |= EF_IGNORE_BULLETS | EF_CRUSHABLE;

	i->health = FPS * 10;

	i->tick = tick;
	i->die = die;

	return i;
}

static void tick(void)
{
	Item *i;

	i = (Item*)self;

	if (i->isOnGround)
	{
		i->dx *= 0.05;
	}

	i->health--;

	if ((int) i->health == FPS * 2)
	{
		i->flags |= EF_FLICKER;
	}
	else if (i->health <= 0)
	{
		i->alive = ALIVE_DEAD;
	}
}

int touchedPlayer(Entity *other)
{
	return (other != NULL && other->type == ET_BOB && self->alive == ALIVE_ALIVE);
}

void pickupItem(void)
{
	Item *i;

	i = (Item*)self;

	i->alive = (i->environment == ENV_AIR) ? ALIVE_DYING : ALIVE_DEAD;
	i->health = FPS / 2;
	i->thinkTime = 0;
	i->dy = -2;
	i->dx = 0;
	i->flags |= EF_FLICKER | EF_WEIGHTLESS;
}

static void die(void)
{
	/* we will handle this ourselves! */
}
