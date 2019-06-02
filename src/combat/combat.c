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

#include "combat.h"

int hasLineOfSight(Entity *src, Entity *dest)
{
	int i, mx, my;
	float sx, sy, tx, ty, dx, dy;
	Entity **candidates, *e;
	SDL_Rect losBounds;

	sx = src->x + (src->w / 2);
	sy = src->y + (src->h / 2);
	tx = dest->x + (dest->w / 2);
	ty = dest->y + (dest->h / 2);

	losBounds.x = (int) MIN(src->x, dest->x);
	losBounds.y = (int) MIN(src->y, dest->y);
	losBounds.w = (int) (MAX(src->x, dest->x) - losBounds.x);
	losBounds.h = (int) (MAX(src->y, dest->y) - losBounds.y);

	candidates = getAllEntsWithin(losBounds.x, losBounds.y, losBounds.w, losBounds.h, NULL);

	getSlope(sx, sy, tx, ty, &dx, &dy);

	dx *= 8;
	dy *= 8;

	while (1)
	{
		sx -= dx;
		sy -= dy;

		mx = sx / MAP_TILE_SIZE;
		my = sy / MAP_TILE_SIZE;

		if (isSolid(mx, my))
		{
			return 0;
		}

		for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
		{
			if (e != src && collision(sx, sy, 8, 8, e->x, e->y, e->w, e->h))
			{
				if (e->isSolid)
				{
					return 0;
				}

				if (e == dest)
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

int enemyCanSeePlayer(Entity *e)
{
	return hasLineOfSight(e, (Entity*)world.bob);
}
