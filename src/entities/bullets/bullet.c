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

#include "bullet.h"

static void tick(void);
static void touch(Entity *other);

Bullet *createBaseBullet(Unit *owner, int bulletWidth)
{
	Bullet *bullet;
	
	bullet = malloc(sizeof(Bullet));
	memset(bullet, 0, sizeof(Bullet));

	initEntity((Entity*)bullet);
	
	bullet->x = owner->x + (owner->w / 2) - (bulletWidth / 2);
	bullet->y = (owner->y + owner->h / 2) - 3;
	bullet->dx = owner->facing == FACING_RIGHT ? 15 : -15;
	bullet->facing = owner->facing;
	bullet->damage = 1;
	bullet->owner = (Entity*)owner;
	bullet->health = FPS * 3;
	bullet->flags |= EF_WEIGHTLESS | EF_IGNORE_BULLETS | EF_NO_ENVIRONMENT | EF_KILL_OFFSCREEN | EF_NO_TELEPORT;

	bullet->tick = tick;
	bullet->touch = touch;
	
	bullet->spriteFrame = 0;

	return bullet;
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

	/* don't allow the player to kill everything on the map by firing constantly */
	if (b->owner->type == ET_BOB)
	{
		if (b->x < camera.x || b->y < camera.y || b->x > camera.x + SCREEN_WIDTH || b->y > camera.y + SCREEN_HEIGHT)
		{
			b->alive = ALIVE_DEAD;
		}
	}
}

static void touch(Entity *other)
{
	Bullet *b;
	int canHit;

	b = (Bullet*)self;

	if (b->alive == ALIVE_ALIVE)
	{
		if (other == NULL)
		{
			addSparkParticles(b->x, b->y);

			b->alive = ALIVE_DEAD;

			if (rand() % 2)
			{
				playBattleSound(SND_RICO_1, b->uniqueId % MAX_SND_CHANNELS, b->x, b->y);
			}
			else
			{
				playBattleSound(SND_RICO_2, b->uniqueId % MAX_SND_CHANNELS, b->x, b->y);
			}
		}
		else
		{
			canHit = (
				other != b->owner &&
				(!(other->flags & EF_IGNORE_BULLETS)) &&
				b->owner->type != other->type &&
				!((b->owner->type == ET_TEEKA && other->type == ET_BOB) || (b->owner->type == ET_BOB && other->type == ET_TEEKA))
			);
			
			if (canHit)
			{
				swapSelf(other);
				other->applyDamage(b->damage);
				swapSelf(other);

				if (other->flags & EF_EXPLODES)
				{
					playBattleSound(SND_METAL_HIT, b->uniqueId % MAX_SND_CHANNELS, b->x, b->y);

					addSparkParticles(b->x, b->y);
				}
				else
				{
					playBattleSound(SND_FLESH_HIT, b->uniqueId % MAX_SND_CHANNELS, b->x, b->y);

					addSmallFleshChunk(b->x, b->y);
				}

				b->alive = ALIVE_DEAD;

				if (b->owner->type == world.bob->type && (other->type == ET_ENEMY || other->type == ET_BOSS || other->type == ET_DESTRUCTABLE))
				{
					game.stats[STAT_SHOTS_HIT]++;
				}
			}
		}
	}
}
