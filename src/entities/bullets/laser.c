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

#include "laser.h"

static void tick(void);
static void touch(Entity *other);

void initLaser(Bullet *b)
{
	b->flags = EF_WEIGHTLESS | EF_BOUNCES | EF_FRICTIONLESS | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS | EF_KILL_OFFSCREEN | EF_NO_TELEPORT;

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
}

static void touch(Entity *other)
{
	Bullet *b;
	int canHit;

	b = (Bullet*)self;

	if (b->alive == ALIVE_ALIVE)
	{
		canHit = (
			other != NULL &&
			other != b->owner &&
			(!(other->flags & EF_IGNORE_BULLETS)) &&
			b->owner->type != other->type &&
			!((b->owner->type == ET_TEEKA && other->type == ET_BOB) || (b->owner->type == ET_BOB && other->type == ET_TEEKA))
		);
			
		if (canHit)
		{
			if (other->flags & EF_EXPLODES)
			{
				addSparkParticles(b->x, b->y);
				playBattleSound(SND_METAL_HIT, b->uniqueId % MAX_SND_CHANNELS, b->x, b->y);
			}
			else
			{
				addSmallFleshChunk(b->x, b->y);
				playBattleSound(SND_FLESH_HIT, b->uniqueId % MAX_SND_CHANNELS, b->x, b->y);
			}

			swapSelf(other);
			other->applyDamage(2);
			swapSelf(other);

			if (!(b->flags & EF_BULLET_HIT) && b->owner->type == world.bob->type && (other->type == ET_ENEMY || other->type == ET_BOSS || other->type == ET_DESTRUCTABLE))
			{
				b->flags |= EF_BULLET_HIT;
				game.stats[STAT_SHOTS_HIT]++;
			}

			if (other->type == ET_BOB && world.bob->stunTimer == 0)
			{
				other->dx = rrnd(-8, 8);
				other->dy = rrnd(-12, 0);
				stunBob();
			}
		}
	}
}
