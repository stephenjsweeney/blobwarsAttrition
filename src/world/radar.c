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

#include "radar.h"

static void logic(void);
static void draw(void);
static void getMapTileColor(int i, SDL_Color *c);
static void drawMap(void);
static void drawEntities(void);
static int isValidBlip(Entity *e);
static void getBlipColor(Entity *e, SDL_Color *c);

static SDL_Rect viewRect;

void initRadar(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	viewRect.x = (world.bob->x / MAP_TILE_SIZE) - (VIEW_SIZE / 2);
	viewRect.y = (world.bob->y / MAP_TILE_SIZE) - (VIEW_SIZE / 2);
	viewRect.w = VIEW_SIZE;
	viewRect.h = VIEW_SIZE;
}

static void logic(void)
{

}

static void draw(void)
{
	drawMap();
	
	drawEntities();
}

static void drawMap(void)
{
	int x, y, mx, my;
	SDL_Color c;
	
	for (x = 0 ; x < viewRect.w ; x++)
	{
		for (y = 0 ; y < viewRect.h ; y++)
		{
			mx = viewRect.x + x;
			my = viewRect.y + y;
			
			if (isWithinMap(mx, my))
			{
				if (world.map.data[mx][my] > MAP_TILE_AIR)
				{
					getMapTileColor(world.map.data[mx][my], &c);
					
					drawRect(OFFSET_X + (x * RADAR_TILE_SIZE), OFFSET_Y + (y * RADAR_TILE_SIZE), RADAR_TILE_SIZE - 1, RADAR_TILE_SIZE - 1, c.r, c.g, c.b, 255);
				}
			}
		}	
	}
}

static void getMapTileColor(int i, SDL_Color *c)
{
	c->r = c->g = c->b = 0;
	
	switch (i)
	{
		case MAP_TILE_WATER:
			c->r = c->g = c->b = 128;
			c->b = 255;
			break;
			
		case MAP_TILE_SLIME:
			c->r = c->g = c->b = 128;
			c->g = 255;
			break;
			
		case MAP_TILE_LAVA:
			c->r = c->g = c->b = 128;
			c->r = 255;
			break;
			
		default:
			if (i < MAP_TILE_NON_SOLID)
			{
				c->r = c->g = c->b = 168;
			}
			break;
	}

}

static void drawEntities(void)
{
	Entity *e;
	Entity **candidates;
	int i, x, y;
	SDL_Color c;

	candidates = getAllEntsWithin(viewRect.x * MAP_TILE_SIZE, viewRect.y * MAP_TILE_SIZE, viewRect.w * MAP_TILE_SIZE, viewRect.h * MAP_TILE_SIZE, NULL);
	
	for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
	{
		if (isValidBlip(e))
		{
			x = (e->x / MAP_TILE_SIZE) - viewRect.x;
			y = (e->y / MAP_TILE_SIZE) - viewRect.y;
			
			getBlipColor(e, &c);

			drawRect(OFFSET_X + (x * RADAR_TILE_SIZE), OFFSET_Y + (y * RADAR_TILE_SIZE), RADAR_TILE_SIZE - 1, RADAR_TILE_SIZE - 1, c.r, c.g, c.b, 255);
		}
	}
}

static int isValidBlip(Entity *e)
{
	switch (e->type)
	{
		case ET_BOB:
			return 1;
			
		case ET_ENEMY:
		case ET_TEEKA:
		case ET_BOSS:
		case ET_HEART_CELL:
		case ET_KEY:
		case ET_ITEM:
		case ET_MIA:
		case ET_DESTRUCTABLE:
			return 1;
			
		default:
			return 0;
	}
	
	return 0;
}

static void getBlipColor(Entity *e, SDL_Color *c)
{
	c->r = c->g = c->b = 0;
	
	switch (e->type)
	{
		case ET_BOB:
			break;
			
		case ET_ENEMY:
			c->r = 255;
			break;
			
		case ET_TEEKA:
		case ET_BOSS:
		case ET_HEART_CELL:
		case ET_KEY:
		case ET_ITEM:
		case ET_MIA:
		case ET_DESTRUCTABLE:
			c->r = c->b = 255;
			break;
			
		default:
			break;
	}
}
