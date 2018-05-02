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

#include "hud.h"

static void drawHealth(void);
static void drawPower(void);
static void drawOxygen(void);
static void drawInventory(void);
static void drawBossHealth(void);

static int messageTime;
static int infoMessageTime;
static char message[MAX_DESCRIPTION_LENGTH];
static int messageType;
static SDL_Color messageColor;
static char infoMessage[MAX_DESCRIPTION_LENGTH];
static Texture *atlasTexture;
static Atlas *healthIcon;
static Atlas *powerIcon;
static Atlas *oxygenIcon;

void initHud(void)
{
	messageTime = FPS * 2;
	messageType = MSG_STANDARD;
	strcpy(message, "");
	messageColor = colors.white;
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	healthIcon = getImageFromAtlas("gfx/hud/health.png");
	powerIcon = getImageFromAtlas("gfx/hud/power.png");
	oxygenIcon = getImageFromAtlas("gfx/hud/oxygen.png");
}

void doHud(void)
{
	if (--messageTime <= 0)
	{
		messageType = MSG_STANDARD;
		messageTime = 0;
		strcpy(message, "");
	}
	
	if (--infoMessageTime <= 0)
	{
		infoMessageTime = 0;
	}
}

void drawHud(void)
{
	int x, y, h;
	
	drawHealth();
	
	drawPower();
	
	drawOxygen();
	
	drawText(10, 82, 16, TA_LEFT, colors.white, app.strings[ST_WEAPON], getWeaponName(world.bob->weaponType));
	
	if (app.config.inventory)
	{
		drawInventory();
	}
	
	if (world.isBossActive)
	{
		drawBossHealth();
	}
	else if (messageTime > 0)
	{
		drawRect(0, SCREEN_HEIGHT - 32, SCREEN_WIDTH, 32, 0, 0, 0, 200);
		
		drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 26, 16, TA_CENTER, messageColor, message);
	}
	
	if (infoMessageTime > 0)
	{
		limitTextWidth(500);
		h = getWrappedTextHeight(infoMessage, 20) + 20;
		drawRect((SCREEN_WIDTH / 2) - 300, 40, 600, h, 0, 0, 0, 168);
		drawOutlineRect((SCREEN_WIDTH / 2) - 300, 40, 600, h, 192, 192, 192, 255);
		drawText(SCREEN_WIDTH / 2, 50, 20, TA_CENTER, colors.white, infoMessage);
		limitTextWidth(0);
	}
	
	if (dev.debug)
	{
		x = -camera.x + world.bob->x + (world.bob->w / 2);
		y = -camera.y + world.bob->y - world.bob->h;
		
		drawText(x, y, 14, TA_CENTER, colors.white, "[%.0f, %.0f]", world.bob->x / MAP_TILE_SIZE, world.bob->y / MAP_TILE_SIZE);
	}
}

static void drawHealth(void)
{
	int w, health, healthMax;
	
	health = MAX(0, world.bob->health);
	healthMax = MIN(world.bob->healthMax, 50);
	
	blitRect(atlasTexture->texture, 17, 17, &healthIcon->rect, 1);
	
	w = healthMax * 12;
	
	drawRect(35, 8, w, 18, 0, 64, 0, 255);
	
	w *= ((health * 1.0) / world.bob->healthMax);
	
	drawRect(35, 8, w, 18, 0, 225, 0, 255);
	if (world.frameCounter % 60 < 30 && getPercent(health, world.bob->healthMax) <= 33)
	{
		drawRect(35, 8, w, 18, 255, 225, 255, 255);
	}
	
	w = healthMax * 12;
	
	drawOutlineRect(35, 8, w, 18, 0, 0, 0, 255);
	
	drawText(35 + w + 5, 7, 14, TA_LEFT, colors.white, "%d", health);
}

static void drawPower(void)
{
	float w, powerMax;
	
	powerMax = MIN(world.bob->powerMax, 50);
	
	blitRect(atlasTexture->texture, 17, 42, &powerIcon->rect, 1);
	
	w = powerMax * 12;
	
	drawRect(35, 33, w, 18, 64, 32, 0, 255);
	
	w *= (world.bob->power / world.bob->powerMax);
	
	drawRect(35, 33, w, 18, 225, 112, 0, 255);
	
	w = powerMax * 12;
	
	drawOutlineRect(35, 33, w, 18, 0, 0, 0, 255);
	
	drawText(35 + w + 5, 32, 14, TA_LEFT, colors.white, "%.1f", world.bob->power);
}

static void drawOxygen(void)
{
	int w;
	
	blitRect(atlasTexture->texture, 17, 67, &oxygenIcon->rect, 1);
	
	w = MAX_OXYGEN / 5;
	
	drawRect(35, 58, w, 18, 0, 32, 64, 255);
	
	w = world.bob->oxygen / 5;
	
	drawRect(35, 58, w, 18, 0, 112, 225, 255);
	if (world.frameCounter % 60 < 30 && getPercent(world.bob->oxygen, MAX_OXYGEN) <= 33)
	{
		drawRect(35, 58, w, 18, 255, 225, 255, 255);
	}
	
	w = MAX_OXYGEN / 5;
	
	drawOutlineRect(35, 58, w, 18, 0, 0, 0, 255);
	
	drawText(35 + w + 5, 57, 14, TA_LEFT, colors.white, "%.1fs", (world.bob->oxygen * 1.0) / FPS);
}

static void drawInventory(void)
{
	int x, y, i, size, mid;
	float w, h, d;
	SDL_Rect *r;
	
	size = 45;
	mid = size / 2;
	
	x = 930;
	y = 5;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		if (i > 0 && i % (MAX_ITEMS / 2) == 0)
		{
			y += (size + 5);
			x = 930;
		}
		
		drawRect(x, y, size, size, 0, 0, 0, 128);
		
		drawOutlineRect(x, y, size, size, 255, 255, 255, 255);
		
		if (world.bob->items[i] != NULL)
		{
			r = getCurrentFrame(world.bob->items[i]->sprite[0]);
			
			w = r->w;
			h = r->h;
			
			if (w > 40 || h > 40)
			{
				d = 40;
				d /= (w > h) ? w : h;
				
				w *= d;
				h *= d;
			}
			
			blitRectScaled(atlasTexture->texture, x + mid, y + mid, w, h, r, 1);
		}
		
		x += (size + 5);
	}
}

static void drawBossHealth(void)
{
	float percent;
	int w;
	
	percent = world.boss->health;
	percent /= world.boss->healthMax;
	
	w = MAX(500 * percent, 0);
	
	drawRect(0, SCREEN_HEIGHT - 32, SCREEN_WIDTH, 32, 0, 0, 0, 200);
		
	drawText(440, SCREEN_HEIGHT - 28, 16, TA_RIGHT, colors.white, world.boss->name);
	
	drawRect(450, SCREEN_HEIGHT - 24, w, 16, 255, 0, 0, 255);
	drawOutlineRect(450, SCREEN_HEIGHT - 24, 500, 16, 192, 192, 192, 255);
}

void drawMissionStatus(void)
{
	Objective *o;
	int y, x, w, h, size, mid, i, textSize, lineSpacing;
	float rw, rh, d;
	SDL_Color c;
	SDL_Rect *r;
	char *status;
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 128);
	
	w = 800;
	h = 550;
	x = (SCREEN_WIDTH - w) / 2;
	
	drawRect(x, (SCREEN_HEIGHT - h) / 2, w, h, 0, 0, 0, 128);
	drawOutlineRect(x, (SCREEN_HEIGHT - h) / 2, w, h, 255, 255, 255, 200);
	
	drawText(SCREEN_WIDTH / 2, 100, 40, TA_CENTER, colors.white, app.strings[ST_OBJECTIVES]);
	
	y = 180;
	textSize = 24;
	lineSpacing = 55;
	
	if (world.numObjectives > 5)
	{
		textSize -= (world.numObjectives - 5) * 2;
		lineSpacing -= (world.numObjectives - 5) * 8;
	}
	
	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		c = o->required ? colors.red : colors.white;
		status = app.strings[ST_INCOMPLETE];
		
		if (o->currentValue >= o->targetValue)
		{
			c = colors.green;
			status = app.strings[ST_COMPLETE];
		}
		
		drawText(x + 20, y, textSize, TA_LEFT, c, o->description);
		drawText(SCREEN_WIDTH / 2 + 100, y, textSize, TA_LEFT, c, "%d / %d", MIN(o->currentValue, o->targetValue), o->targetValue);
		drawText(x + w - 20, y, textSize, TA_RIGHT, c, status);
		
		y += lineSpacing;
	}
	
	size = 60;
	mid = size / 2;
	
	y = (((SCREEN_HEIGHT - h) / 2) + h) - 165;
	
	x = ((SCREEN_WIDTH - w) / 2) + 90;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		if (i > 0 && i % (MAX_ITEMS / 2) == 0)
		{
			y += (size + 20);
			x = ((SCREEN_WIDTH - w) / 2) + 90;
		}
		
		drawRect(x, y, size, size, 0, 0, 0, 128);
		
		drawOutlineRect(x, y, size, size, 255, 255, 255, 255);
		
		if (world.bob->items[i] != NULL)
		{
			r = getCurrentFrame(world.bob->items[i]->sprite[0]);
			
			rw = r->w;
			rh = r->h;
			
			if (rw > 40 || rh > 40)
			{
				d = 40;
				d /= (rw > rh) ? rw : rh;
				
				rw *= d;
				rh *= d;
			}
			
			blitRectScaled(atlasTexture->texture, x + mid, y + mid, rw, rh, r, 1);
			
			if (world.bob->items[i]->value > 0)
			{
				drawText(x + size - 5, y, 14, TA_RIGHT, colors.white, "%d", world.bob->items[i]->value);
			}
		}
		
		x += (size + 30);
	}
}

void setGameplayMessage(int newMessageType, const char *format, ...)
{
	char newMessage[MAX_DESCRIPTION_LENGTH];
	va_list args;

	memset(&newMessage, '\0', sizeof(newMessage));

	va_start(args, format);
	vsprintf(newMessage, format, args);
	va_end(args);
	
	if (strlen(newMessage) > 0 && newMessageType >= messageType && strcmp(message, newMessage))
	{
		STRNCPY(message, newMessage, MAX_DESCRIPTION_LENGTH);
		messageType = newMessageType;
		messageTime = FPS * 3;
		
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%s", message);
		
		switch (messageType)
		{
			case MSG_STANDARD:
			case MSG_GAMEPLAY:
				messageColor = colors.white;
				break;
			
			case MSG_PROGRESS:
				messageColor = colors.cyan;
				break;
			
			case MSG_OBJECTIVE:
				messageColor = colors.green;
				break;
		}
	}
}

void showInfoMessage(char *newInfoMessage)
{
	STRNCPY(infoMessage, newInfoMessage, MAX_DESCRIPTION_LENGTH);
	infoMessageTime = FPS / 4;
}
