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

static void loadMapData(void);
static void loadCommonTiles(void);
static void loadTileset(void);
static void loadDecals(void);
static SDL_Rect *loadTile(char *filename);

static int MAX_Y;
static Texture *atlasTexture;
static SDL_Rect *tiles[MAP_TILE_MAX];
static SDL_Rect *decals[8];

void initMap(void)
{
	memset(&world.map, 0, sizeof(Map));
	
	world.map.bounds.x = MAP_WIDTH * MAP_TILE_SIZE;
	world.map.bounds.y = MAP_HEIGHT * MAP_TILE_SIZE;
	world.map.bounds.w = 0;
	world.map.bounds.h = 0;
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	loadMapData();
	
	loadCommonTiles();
	
	loadTileset();
	
	loadDecals();
}

void drawMap(void)
{
	int mx, x1, x2, my, y1, y2, tile, decal, x, y;
	
	mx = camera.x / MAP_TILE_SIZE;
	x1 = (camera.x % MAP_TILE_SIZE) * -1;
	x2 = x1 + RENDER_WIDTH * MAP_TILE_SIZE + (x1 == 0 ? 0 : MAP_TILE_SIZE);

	my = camera.y / MAP_TILE_SIZE;
	y1 = (camera.y % MAP_TILE_SIZE) * -1;
	y2 = y1 + RENDER_HEIGHT * MAP_TILE_SIZE + (y1 == 0 ? 0 : MAP_TILE_SIZE);

	tile = 0;
	decal = 0;

	for (x = x1; x < x2; x += MAP_TILE_SIZE)
	{
		for (y = y1; y < y2; y += MAP_TILE_SIZE)
		{
			if (mx < MAP_WIDTH && my < MAP_HEIGHT && my <= MAX_Y)
			{
				tile = world.map.data[mx][my];

				if (world.mapAnimTimer == 0)
				{
					if (tile >= MAP_TILE_ANIMATED_WATER && tile < MAP_TILE_ANIMATED_SLIME)
					{
						world.map.data[mx][my] = tile = rrnd(MAP_TILE_ANIMATED_WATER, MAP_TILE_ANIMATED_SLIME - 1);
					}

					if (tile >= MAP_TILE_ANIMATED_SLIME && tile < MAP_TILE_ANIMATED_LAVA)
					{
						world.map.data[mx][my] = tile = rrnd(MAP_TILE_ANIMATED_SLIME, MAP_TILE_ANIMATED_LAVA - 1);
					}

					if (tile >= MAP_TILE_ANIMATED_LAVA && tile < MAP_TILE_OUTSIDE)
					{
						world.map.data[mx][my] = tile = rrnd(MAP_TILE_ANIMATED_LAVA, MAP_TILE_OUTSIDE - 1);
					}
				}

				if (tile != MAP_TILE_AIR)
				{
					if (tile >= MAP_TILE_SOLID && tile < MAP_TILE_NON_SOLID)
					{
						blitRect(atlasTexture->texture, x + 2, y + 2, tiles[MAP_TILE_OUTSIDE], 0);
					}
					
					blitRect(atlasTexture->texture, x, y, tiles[tile], 0);

					decal = world.map.decal[mx][my];

					if (decal != 0)
					{
						blitRect(atlasTexture->texture, x, y, decals[decal - 1], 0);
					}
				}
			}
			else
			{
				blitRect(atlasTexture->texture, x, y, tiles[MAP_TILE_OUTSIDE], 0);
			}

			my++;
		}

		my = camera.y / MAP_TILE_SIZE;
		mx++;
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

static void calculateMapBounds(void)
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
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Map bounds [%d, %d, %d, %d]", world.map.bounds.x, world.map.bounds.y, world.map.bounds.w, world.map.bounds.h);
	
	MAX_Y = 0;

	for (y = 0; y < MAP_HEIGHT; y++)
	{
		for (x = 0; x < MAP_WIDTH; x++)
		{
			if (world.map.data[x][y] != 0)
			{
				if (y > MAX_Y)
				{
					MAX_Y = y;
				}
			}
		}
	}
}

static void loadMapData(void)
{
	char filename[MAX_FILENAME_LENGTH];
	char *data, *p;
	int i, x, y;
	
	sprintf(filename, "data/maps/raw/%s.raw.z", world.id);
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
	
	data = readCompressedFile(filename);
	
	p = data;
	
	for (y = 0 ; y < MAP_HEIGHT ; y++)
	{
		for (x = 0 ; x < MAP_WIDTH ; x++)
		{
			sscanf(p, "%d", &i);
			
			if (!world.isOutpostMission)
			{
				if (i >= 4 && i <= 7)
				{
					i = rrnd(4, 7);
				}
			}
			else
			{
				if (i >= 4 && i <= 8)
				{
					i = rrnd(4, 8);
				}
			}

			if (i >= 200 && i <= 203)
			{
				i = rrnd(200, 203);
			}
			
			world.map.data[x][y] = i;
			
			do {p++;} while (*p != ' ');
		}
	}
	
	free(data);
	
	calculateMapBounds();
}

static void loadCommonTiles(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];
	
	tiles[1] = loadTile("gfx/tiles/common/1.png");
	tiles[2] = loadTile("gfx/tiles/common/2.png");
	tiles[3] = loadTile("gfx/tiles/common/3.png");

	for (i = MAP_TILE_ANIMATED_WATER ; i < MAP_TILE_MAX ; i++)
	{
		sprintf(filename, "gfx/tiles/common/%d.png", i);
		tiles[i] = loadTile(filename);
	}
}

static void loadTileset(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];
	
	for (i = MAP_TILE_SOLID ; i < MAP_TILE_ANIMATED_WATER ; i++)
	{
		sprintf(filename, "gfx/tiles/%s/%d.png", world.tileset, i);
		
		tiles[i] = loadTile(filename);
	}
}

static SDL_Rect *loadTile(char *filename)
{
	return &getImageFromAtlas(filename)->rect;
}

static void loadDecals(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];
	
	for (i = 0 ; i < 4 ; i++)
	{
		sprintf(filename, "gfx/decals/blood%d.png", (i + 1));
		decals[i] = loadTile(filename);
		
		sprintf(filename, "gfx/decals/scorch%d.png", (i + 1));
		decals[i + 4] = loadTile(filename);
	}
}
