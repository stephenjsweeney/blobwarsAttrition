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

#include "grenade.h"

static void tick(void);
static void touch(Entity *other);
static float bounce(float x);
static float (*superBounce)(float x);
static void die(void);
static void explode(void);

void initGrenade(Bullet *b)
{
	b->flags = EF_BOUNCES | EF_IGNORE_BULLETS | EF_KILL_OFFSCREEN | EF_FRICTIONLESS | EF_NO_TELEPORT;
	
	superBounce = b->bounce;

	b->tick = tick;
	b->touch = touch;
	b->bounce = bounce;
	b->die = die;
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

	b = (Bullet*)self;

	if (b->alive == ALIVE_ALIVE)
	{
		/* don't bounce too high */
		if (b->dy < -8)
		{
			b->dy = -8;
		}

		if (other != b->owner)
		{
			if (other != NULL && (!(other->flags & EF_IGNORE_BULLETS)) && !other->isSolid && b->owner->type != other->type && other->type != ET_LIFT)
			{
				if (other->flags & EF_EXPLODES)
				{
					addSparkParticles(b->x, b->y);
				}
				else
				{
					addSmallFleshChunk(b->x, b->y);
				}

				if (b->owner->type == world.bob->type)
				{
					game.statShotsHit[WPN_GRENADES]++;
				}

				if (!(other->flags & EF_BOMB_SHIELD))
				{
					swapSelf(other);
					other->applyDamage(25);
					swapSelf(other);
				}

				explode();
			}
		}
	}
}

static float bounce(float x)
{
	Bullet *b;

	b = (Bullet*)self;

	if (b->environment == ENV_AIR)
	{
		playSound(SND_GRENADE_BOUNCE, CH_EFFECTS);
	}

	return superBounce(x);
}

static void die(void)
{
	explode();
}

static void explode(void)
{
	Bullet *b;
	int mx, my;

	b = (Bullet*)self;

	addExplosion(b->x, b->y, 100, b->owner);

	b->alive = ALIVE_DEAD;

	mx = (int) ((b->x + (b->w / 2)) / MAP_TILE_SIZE);
	my = (int) (b->y / MAP_TILE_SIZE) + 1;
	addScorchDecal(mx, my);
}
