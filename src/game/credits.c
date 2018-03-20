/*
Copyright (C) 2015-2017 Parallel Realities

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

#include "credits.h"

static void loadCredits(void);
static void logic(void);
static void draw(void);
static void handleKeyboard(void);

static Texture *atlasTexture;
static Atlas *background;
static Credit head, *tail;
static float creditSpeed;
static int timeout;
static int isFromEnding;

void initCredits(int fromEnding)
{
	startSectionTransition();

	isFromEnding = fromEnding;
	
	memset(&head, 0, sizeof(Credit));
	tail = &head;
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	memset(&app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	background = getImageFromAtlas("gfx/main/credits.png");
	
	loadCredits();
	
	if (isFromEnding)
	{
		loadMusic("music/Eternal Wishes.ogg");
		
		playMusic(0);
	}
	
	endSectionTransition();
}

static void logic(void)
{
	Credit *c;
	
	handleKeyboard();
	
	for (c = head.next ; c != NULL ; c = c->next)
	{
		c->y -= creditSpeed;
	}
	
	if (--timeout <= 0)
	{
		if (isFromEnding)
		{
			initTitle();
		}
		else
		{
			startSectionTransition();
			returnToTitle();
			endSectionTransition();
		}
	}
}

static void draw(void)
{
	float scale;
	int w, h;
	Credit *c;
	
	scale = (SCREEN_WIDTH * 1.0) / background->rect.w;
	
	w = background->rect.w * scale;
	h = background->rect.h * scale;
	
	blitRectScaled(atlasTexture->texture, 0, SCREEN_HEIGHT - h, w, h, &background->rect, 0);
	
	limitTextWidth(CREDIT_LINE_LIMIT);
	
	for (c = head.next ; c != NULL ; c = c->next)
	{
		if (c->y > -c->h && c->y < SCREEN_HEIGHT)
		{
			drawText(SCREEN_WIDTH / 2, (int)c->y, c->size, TA_CENTER, colors.white, c->text);
		}
	}
	
	limitTextWidth(0);
}

static void loadCredits(void)
{
	int y, dist;
	char *text, *line, *saveptr;
	Credit *c;
	
	y = SCREEN_HEIGHT + 50;

	text = readFile("data/misc/credits.txt");
	
	limitTextWidth(CREDIT_LINE_LIMIT);
	
	line = strtok_r(text, "\n", &saveptr);
	
	while (line)
	{
		c = malloc(sizeof(Credit));
		memset(c, 0, sizeof(Credit));
		tail->next = c;
		tail = c;
		
		c->y = y;
		
		c->text = malloc(sizeof(char) * strlen(line));
		memset(c->text, '\0', sizeof(char) * strlen(line));
		
		sscanf(line, "%d %d %[^\n]", &dist, &c->size, c->text);
		
		c->y += dist;
		c->h = getWrappedTextHeight(c->text, c->size);
		
		y += c->h + dist;
		
		line = strtok_r(NULL, "\n", &saveptr);
	}
	
	limitTextWidth(0);

	/* the music that plays over the credits is 1m 55s, so scroll credits roughly inline with that (plus 2 seconds) */
	if (isFromEnding)
	{
		timeout = (60 + 57) * FPS;
	}
	else
	{
		timeout = 60 * FPS;
	}
	
	creditSpeed = y;
	creditSpeed /= timeout;
	
	free(text);
}

static void handleKeyboard(void)
{
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		timeout = 0;
	}
}

void destroyCredits(void)
{
	Credit *c;
	
	while (head.next)
	{
		c = head.next;
		head.next = c->next;
		if (c->text)
		{
			free(c->text);
		}
		free(c);
	}
}
