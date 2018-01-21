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

#include "map.h"

static int animTimer;

void initMap(void)
{
	memset(&world.map, 0, sizeof(Map));
	
	world.map.bounds.x = MAP_WIDTH * MAP_TILE_SIZE;
	world.map.bounds.y = MAP_HEIGHT * MAP_TILE_SIZE;
	world.map.bounds.w = 0;
	world.map.bounds.h = 0;
	
	animTimer = 0;
}

void animateMap(void)
{
	if (--animTimer < 0)
	{
		animTimer = 4;
	}
}

int isWithinMap(int x, int y)
{
	return (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT);
}

int isSolid(int x, int y)
{
	if (!isWithinMap(x, y))
	{
		return 1;
	}

	if ((world.map.data[x][y] >= MAP_TILE_SOLID) && (world.map.data[x][y] < MAP_TILE_NON_SOLID))
	{
		return 1;
	}

	return 0;
}

void addBloodDecal(int x, int y)
{
	if (isSolid(x, y) && world.map.decal[x][y] == 0)
	{
		world.map.decal[x][y] = (int) rrnd(1, 4);
	}
}

void addScorchDecal(int x, int y)
{
	if (isSolid(x, y) && world.map.decal[x][y] == 0)
	{
		world.map.decal[x][y] = (int) rrnd(5, 8);
	}
}

int isLiquid(int x, int y)
{
	return isWithinMap(x, y) && world.map.data[x][y] > MAP_TILE_AIR && world.map.data[x][y] < MAP_TILE_SOLID;
}

int isWalkable(int x, int y)
{
	return isSolid(x, y);
}

int isBreakable(int x, int y)
{
	return 0;
}

void calculateMapBounds(void)
{
	int x, y;
	
	for (y = 0 ; y < MAP_HEIGHT; y++)
	{
		for (x = 0 ; x < MAP_WIDTH; x++)
		{
			if (world.map.data[x][y] != 0)
			{
				if (x < world.map.bounds.x)
				{
					world.map.bounds.x = x;
				}

				if (x > world.map.bounds.w)
				{
					world.map.bounds.w = x;
				}

				if (y < world.map.bounds.y)
				{
					world.map.bounds.y = y;
				}

				if (y > world.map.bounds.h)
				{
					world.map.bounds.h = y;
				}
			}
		}
	}

	if (world.map.bounds.h - RENDER_HEIGHT < world.map.bounds.y)
	{
		world.map.bounds.y = 0;
	}

	world.map.bounds.x = (int) limit(world.map.bounds.x, 0, MAP_WIDTH - RENDER_WIDTH);
	world.map.bounds.y = (int) limit(world.map.bounds.y, 0, MAP_HEIGHT - RENDER_HEIGHT);

	world.map.bounds.x *= MAP_TILE_SIZE;
	world.map.bounds.y *= MAP_TILE_SIZE;
	world.map.bounds.w *= MAP_TILE_SIZE;
	world.map.bounds.h *= MAP_TILE_SIZE;

	world.map.bounds.w -= SCREEN_WIDTH;
	world.map.bounds.h -= SCREEN_HEIGHT;

	world.map.bounds.w += MAP_TILE_SIZE;
	world.map.bounds.h += MAP_TILE_SIZE;
}
