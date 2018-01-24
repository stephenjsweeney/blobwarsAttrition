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

#include "combat.h"

static int isBlockedByMap(Entity *src, Entity *dest);
static int isBlockedByEntities(Entity *src, Entity *dest);

void lookForPlayer(void)
{
	
}

void preFire(Entity *e)
{
	if (e->flags & EF_WEIGHTLESS)
	{
		if (world.bob->y < e->y && e->isOnGround && (int) (rand() % 4) == 0)
		{
			e->dy = JUMP_POWER;
		}
	}

	e->facing = (world.bob->x < e->x) ? FACING_LEFT : FACING_RIGHT;

	if (e->reload > 0)
	{
		return;
	}

	e->attack();

	e->shotsToFire--;

	if (e->shotsToFire == 0)
	{
		e->walk();
	}
}

int hasLineOfSight(Entity *src, Entity *dest)
{
	return (!isBlockedByMap(src, dest) && !isBlockedByEntities(src, dest));
}

static int isBlockedByMap(Entity *src, Entity *dest)
{
	SDL_Rect mapBlock;
	int x, y, x1, y1, x2, y2, mx, my;
	
	x1 = (int) MIN(src->x, dest->x);
	y1 = (int) MIN(src->y, dest->y);
	x2 = (int) MAX(src->x, dest->x);
	y2 = (int) MAX(src->y, dest->y);

	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			mx = x / MAP_TILE_SIZE;
			my = y / MAP_TILE_SIZE;

			if (isSolid(mx, my))
			{
				mapBlock.x = x;
				mapBlock.y = y;
				mapBlock.w = MAP_TILE_SIZE;
				mapBlock.h = MAP_TILE_SIZE;

				if (lineIntersectsRect(mapBlock, src->x, src->y, dest->x, dest->y))
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

static int isBlockedByEntities(Entity *src, Entity *dest)
{
	Entity **candidates, *e;
	SDL_Rect losBounds;
	int i;
	
	losBounds.x = (int) MIN(src->x, dest->x);
	losBounds.y = (int) MIN(src->y, dest->y);
	losBounds.w = (int) (MAX(src->x, dest->x) - losBounds.x);
	losBounds.h = (int) (MAX(src->y, dest->y) - losBounds.y);
	
	candidates = getAllEntsWithin(losBounds.x, losBounds.y, losBounds.w, losBounds.h, NULL);

	for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
	{
		if (e == src || e == dest || !e->isSolid)
		{
			continue;
		}
		
		if (lineIntersectsRect(e->bounds, src->x, src->y, dest->x, dest->y))
		{
			return 1;
		}
	}
	
	return 0;
}

int enemyCanSeePlayer(Entity *e)
{
	return hasLineOfSight(e, world.bob);
}

int canFire(Entity *target)
{
	return 0;
}
