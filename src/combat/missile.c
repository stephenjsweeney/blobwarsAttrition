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

#include "missile.h"

static void tick(void);
static void touch(Entity *other);

void initMissile(Bullet *b)
{
	b->flags = EF_WEIGHTLESS | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS | EF_KILL_OFFSCREEN | EF_NO_TELEPORT;

	b->tick = tick;
	b->touch = touch;
}

static void tick(void)
{
	Bullet *b;

	b = (Bullet*)self;

	b->health--;

	if (b->x <= 0 || b->y <= 0 || b->x >= (MAP_WIDTH * MAP_TILE_SIZE) - b->w || b->y >= (MAP_HEIGHT * MAP_TILE_SIZE) - b->h)
	{
		b->alive = ALIVE_DEAD;
	}

	if (b->health % 2)
	{
		addFlameParticles(b->x + (b->facing * 35) + rrnd(-1, 1), b->y);
	}
}

static void touch(Entity *other)
{
	Bullet *b;

	b = (Bullet*)self;

	if (other == NULL)
	{
		addExplosion(b->x, b->y, 65, b->owner);

		b->alive = ALIVE_DEAD;
	}
	else if (other != b->owner && (!(other->flags & EF_IGNORE_BULLETS)) && b->owner->type != other->type)
	{
		addExplosion(b->x, b->y, 65, b->owner);

		b->alive = ALIVE_DEAD;
	}
}
