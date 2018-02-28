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
static void drawMarkers(void);
static void initMarker(int i, int x, int y, int angle, int type);
static void initBlips(void);
static void enableMarker(int type, int i);

static SDL_Rect viewRect;
static Texture *atlasTexture;
static Atlas *background;
static Atlas *arrow;
static int blinkTimer;
static Marker marker[MAX_MARKERS];
static Entity *blips[MAX_BLIPS];

void initRadar(void)
{
	SDL_Rect limits;
	
	startSectionTransition();
	
	memset(blips, 0, sizeof(Entity*) * MAX_BLIPS);
	memset(marker, 0, sizeof(Marker) * MAX_MARKERS);
	
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	viewRect.x = (world.bob->x / MAP_TILE_SIZE) - (VIEW_SIZE_X / 2);
	viewRect.y = (world.bob->y / MAP_TILE_SIZE) - (VIEW_SIZE_Y / 2);
	viewRect.w = VIEW_SIZE_X;
	viewRect.h = VIEW_SIZE_Y;
	
	limits.x = world.map.bounds.x / MAP_TILE_SIZE;
	limits.y = world.map.bounds.y / MAP_TILE_SIZE;
	limits.w = (world.map.bounds.w / MAP_TILE_SIZE) - (VIEW_SIZE_X - MAP_RENDER_WIDTH) - 1;
	limits.h = (world.map.bounds.h / MAP_TILE_SIZE) - (VIEW_SIZE_Y - MAP_RENDER_HEIGHT);
	
	viewRect.x = limit(viewRect.x, limits.x, limits.w);
	viewRect.y = limit(viewRect.y, limits.y, limits.h);
	
	if (viewRect.y > limits.h)
	{
		viewRect.y -= (viewRect.y - limits.h);
	}

	blinkTimer = 0;

	atlasTexture = getTexture("gfx/atlas/atlas.png");

	background = getImageFromAtlas("gfx/radar/background.png");
	arrow = getImageFromAtlas("gfx/radar/arrow.png");
	
	/* top */
	initMarker(0, SCREEN_WIDTH / 2 - 275, SCREEN_HEIGHT / 2 - 275, 0, M_MIA);
	initMarker(1, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 275, 0, M_ITEM);
	initMarker(2, SCREEN_WIDTH / 2 + 275, SCREEN_HEIGHT / 2 - 275, 0, M_ENEMY);
	
	/* bottom */
	initMarker(3, SCREEN_WIDTH / 2 - 275, SCREEN_HEIGHT / 2 + 275, 180, M_MIA);
	initMarker(4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 275, 180, M_ITEM);
	initMarker(5, SCREEN_WIDTH / 2 + 275, SCREEN_HEIGHT / 2 + 275, 180, M_ENEMY);
	
	/* left */
	initMarker(6, SCREEN_WIDTH / 2 - 450, SCREEN_HEIGHT / 2 - 200, 270, M_MIA);
	initMarker(7, SCREEN_WIDTH / 2 - 450, SCREEN_HEIGHT / 2, 270, M_ITEM);
	initMarker(8, SCREEN_WIDTH / 2 - 450, SCREEN_HEIGHT / 2 + 200, 270, M_ENEMY);
	
	/* right */
	initMarker(9, SCREEN_WIDTH / 2 + 450, SCREEN_HEIGHT / 2 - 200, 90, M_MIA);
	initMarker(10, SCREEN_WIDTH / 2 + 450, SCREEN_HEIGHT / 2, 90, M_ITEM);
	initMarker(11, SCREEN_WIDTH / 2 + 450, SCREEN_HEIGHT / 2 + 200, 90, M_ENEMY);
	
	initBlips();
	
	endSectionTransition();
}

static void initMarker(int i, int x, int y, int angle, int type)
{
	marker[i].x = x;
	marker[i].y = y;
	marker[i].angle = angle;
	marker[i].type = type;
}

static void initBlips(void)
{
	Entity *e;
	int i, x, y;
	
	i = 0;
	
	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (isValidBlip(e))
		{
			x = (e->x / MAP_TILE_SIZE);
			y = (e->y / MAP_TILE_SIZE);
			
			if (y < viewRect.y)
			{
				enableMarker(e->type, M_MIA);
			}
			else if (y > viewRect.y + viewRect.h)
			{
				enableMarker(e->type, M_MIA + (M_MAX));
			}
			else if (x < viewRect.x)
			{
				enableMarker(e->type, M_MIA + (M_MAX * 2));
			}
			else if (x > viewRect.x + viewRect.w)
			{
				enableMarker(e->type, M_MIA + (M_MAX * 3));
			}
			else if (i < MAX_BLIPS)
			{
				blips[i++] = e;
			}
		}
	}
}

static void enableMarker(int type, int i)
{
	switch (type)
	{
		case ET_BOB:
		case ET_MIA:
		case ET_TEEKA:
			marker[i].visible = 1;
			break;
			
		case ET_HEART_CELL:
		case ET_KEY:
		case ET_ITEM:
		case ET_DESTRUCTABLE:
			marker[i + 1].visible = 1;
			break;
			
		case ET_ENEMY:
		case ET_BOSS:
			marker[i + 2].visible = 1;
			break;
			
		default:
			break;
	}
}

static void logic(void)
{
	int i;
	Marker *m;
	
	blinkTimer++;
	blinkTimer %= 60;
	
	for (i = 0 ; i < MAX_MARKERS ; i++)
	{
		m = &marker[i];
		
		if (i / 3 % 2 == 0)
		{
			m->value += 0.1;
		}
		else
		{
			m->value -= 0.1;
		}
		
		if (i < 6)
		{
			m->y += (float) sin(m->value);
		}
		else
		{
			m->x += (float) sin(m->value);
		}
	}
	
	if (isControl(CONTROL_MAP))
	{
		pauseSound(0);
		clearControl(CONTROL_MAP);
		exitRadar();
	}
}

static void draw(void)
{
	blitRectScaled(atlasTexture->texture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &background->rect, 0);

	drawMap();
	
	drawEntities();
	
	drawMarkers();
	
	drawRect((SCREEN_WIDTH / 2) - 230, SCREEN_HEIGHT - 58, 14, 14, 255, 255, 0, 255);
	drawText((SCREEN_WIDTH / 2) - 200, SCREEN_HEIGHT - 65, 20, TA_LEFT, colors.yellow, "MIAs");
	
	drawRect((SCREEN_WIDTH / 2) - 30, SCREEN_HEIGHT - 58, 14, 14, 0, 255, 255, 255);
	drawText((SCREEN_WIDTH / 2), SCREEN_HEIGHT - 65, 20, TA_LEFT, colors.cyan, "Items");
	
	drawRect((SCREEN_WIDTH / 2) + 170, SCREEN_HEIGHT - 58, 14, 14, 255, 0, 0, 255);
	drawText((SCREEN_WIDTH / 2) + 200, SCREEN_HEIGHT - 65, 20, TA_LEFT, colors.red, "Targets");
}

static void drawMap(void)
{
	int x, y, mx, my, i;
	SDL_Color c;
	
	for (x = 0 ; x < viewRect.w ; x++)
	{
		for (y = 0 ; y < viewRect.h ; y++)
		{
			mx = viewRect.x + x;
			my = viewRect.y + y;
			
			drawRect(OFFSET_X + (x * RADAR_TILE_SIZE), OFFSET_Y + (y * RADAR_TILE_SIZE), RADAR_TILE_SIZE - 1, RADAR_TILE_SIZE - 1, 0, 0, 0, 255);
			
			if (isWithinMap(mx, my))
			{
				i = world.map.data[mx][my];
				
				if (i > MAP_TILE_AIR && i < MAP_TILE_NON_SOLID)
				{
					getMapTileColor(i, &c);
					
					drawRect(OFFSET_X + (x * RADAR_TILE_SIZE), OFFSET_Y + (y * RADAR_TILE_SIZE), RADAR_TILE_SIZE - 1, RADAR_TILE_SIZE - 1, c.r, c.g, c.b, 255);
				}
			}
		}
	}
	
	drawOutlineRect(OFFSET_X, OFFSET_Y, viewRect.w * RADAR_TILE_SIZE, viewRect.h * RADAR_TILE_SIZE, 0, 128, 0, 255);
}

static void getMapTileColor(int i, SDL_Color *c)
{
	c->r = c->g = c->b = 0;
	
	switch (i)
	{
		case MAP_TILE_WATER:
			c->b = 255;
			break;
			
		case MAP_TILE_SLIME:
			c->g = 255;
			break;
			
		case MAP_TILE_LAVA:
			c->g = 128;
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
	int i, x, y;
	SDL_Color c;
	
	for (i = 0 ; i < MAX_BLIPS ; i++)
	{
		e = blips[i];
		
		if (e != NULL)
		{
			x = (e->x / MAP_TILE_SIZE);
			y = (e->y / MAP_TILE_SIZE);
			
			if (x >= viewRect.x && x < viewRect.x + viewRect.w && y >= viewRect.y && y < viewRect.y + viewRect.h)
			{
				x -= viewRect.x;
				y -= viewRect.y;
				
				getBlipColor(e, &c);

				if (blinkTimer < 30)
				{
					drawRect(OFFSET_X + (x * RADAR_TILE_SIZE), OFFSET_Y + (y * RADAR_TILE_SIZE), RADAR_TILE_SIZE - 1, RADAR_TILE_SIZE - 1, c.r, c.g, c.b, 255);
				}
			}
		}
	}
}

static int isValidBlip(Entity *e)
{
	if (!(e->flags & (EF_GONE | EF_TELEPORTING)))
	{
		switch (e->type)
		{
			case ET_BOB:
			case ET_MIA:
			case ET_TEEKA:
			case ET_BOSS:
			case ET_HEART_CELL:
			case ET_KEY:
			case ET_ITEM:
			case ET_DESTRUCTABLE:
				return 1;
				
			case ET_ENEMY:
				return e->isMissionTarget || world.isEliminateAllEnemies;
				
			default:
				return 0;
		}
	}
	
	return 0;
}

static void getBlipColor(Entity *e, SDL_Color *c)
{
	c->r = c->g = c->b = 0;
	
	switch (e->type)
	{
		case ET_BOB:
			c->r = c->g = c->b = 255;
			break;
			
		case ET_ENEMY:
		case ET_BOSS:
			c->r = 255;
			break;
			
		case ET_TEEKA:
		case ET_MIA:
			c->r = c->g = 255;
			break;

		
		case ET_HEART_CELL:
		case ET_KEY:
		case ET_ITEM:
			c->g = 168;
			c->b = 255;
			break;

		case ET_DESTRUCTABLE:
			c->r = c->b = 255;
			break;
			
		default:
			break;
	}
}

static void drawMarkers(void)
{
	int i;
	
	for (i = 0 ; i < MAX_MARKERS ; i++)
	{
		if (marker[i].visible)
		{
			switch (i % M_MAX)
			{
				case M_MIA:
					SDL_SetTextureColorMod(atlasTexture->texture, 255, 255, 0);
					break;
					
				case M_ITEM:
					SDL_SetTextureColorMod(atlasTexture->texture, 0, 192, 255);
					break;
					
				case M_ENEMY:
					SDL_SetTextureColorMod(atlasTexture->texture, 255, 0, 0);
					break;
			}
			
			blitRectRotated(atlasTexture->texture, marker[i].x, marker[i].y, &arrow->rect, marker[i].angle);
		}
	}
	
	SDL_SetTextureColorMod(atlasTexture->texture, 255, 255, 255);
}
