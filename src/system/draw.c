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

#include "draw.h"

static void initColor(SDL_Color *c, int r, int g, int b);

static PointF backgroundPoint[4];

void initBackground(void)
{
	backgroundPoint[0].x = -SCREEN_WIDTH / 2;
	backgroundPoint[0].y = -SCREEN_HEIGHT / 2;
	
	backgroundPoint[1].x = SCREEN_WIDTH / 2;
	backgroundPoint[1].y = -SCREEN_HEIGHT / 2;
	
	backgroundPoint[2].x = -SCREEN_WIDTH / 2;
	backgroundPoint[2].y = SCREEN_HEIGHT / 2;
	
	backgroundPoint[3].x = SCREEN_WIDTH / 2;
	backgroundPoint[3].y = SCREEN_HEIGHT / 2;
}

void initGraphics(void)
{
	initColor(&colors.red, 255, 0, 0);
	initColor(&colors.orange, 255, 128, 0);
	initColor(&colors.yellow, 255, 255, 0);
	initColor(&colors.green, 0, 255, 0);
	initColor(&colors.blue, 0, 0, 255);
	initColor(&colors.cyan, 0, 255, 255);
	initColor(&colors.purple, 255, 0, 255);
	initColor(&colors.white, 255, 255, 255);
	initColor(&colors.black, 0, 0, 0);
	initColor(&colors.lightGrey, 192, 192, 192);
	initColor(&colors.darkGrey, 128, 128, 128);
	
	app.backBuffer = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void prepareScene(void)
{
	SDL_SetRenderTarget(app.renderer, app.backBuffer);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
	if (dev.debug)
	{
		drawText(5, SCREEN_HEIGHT - 25, 14, TA_LEFT, colors.white, "DEBUG MODE");
		if (dev.showFPS)
		{
			drawText(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 25, 14, TA_RIGHT, colors.white, "FPS: %d", dev.fps);
		}
	}
	
	SDL_SetRenderTarget(app.renderer, NULL);
	SDL_RenderCopy(app.renderer, app.backBuffer, NULL, NULL);
	SDL_RenderPresent(app.renderer);
}

void clearScreen(void)
{
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(app.renderer, NULL);
}

void blit(SDL_Texture *texture, int x, int y, int center)
{
	SDL_Rect dstRect;
	
	dstRect.x = x;
	dstRect.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);

	if (center)
	{
		dstRect.x -= (dstRect.w / 2);
		dstRect.y -= (dstRect.h / 2);
	}

	SDL_RenderCopy(app.renderer, texture, NULL, &dstRect);
}

void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center)
{
	SDL_Rect dstRect;
	
	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = srcRect->w;
	dstRect.h = srcRect->h;

	if (center)
	{
		dstRect.x -= (dstRect.w / 2);
		dstRect.y -= (dstRect.h / 2);
	}

	SDL_RenderCopy(app.renderer, texture, srcRect, &dstRect);
}

void blitRectRotated(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, float angle)
{
	SDL_Rect dstRect;
	
	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = srcRect->w;
	dstRect.h = srcRect->h;
	
	dstRect.x -= (srcRect->w / 2);
	dstRect.y -= (srcRect->h / 2);

	SDL_RenderCopyEx(app.renderer, texture, srcRect, &dstRect, angle, NULL, SDL_FLIP_NONE);
}

void blitScaled(SDL_Texture *texture, int x, int y, int w, int h, int center)
{
	SDL_Rect dstRect;
	
	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = w;
	dstRect.h = h;
	
	if (center)
	{
		dstRect.x -= (dstRect.w / 2);
		dstRect.y -= (dstRect.h / 2);
	}

	SDL_RenderCopy(app.renderer, texture, NULL, &dstRect);
}

void blitRectScaled(SDL_Texture *texture, int x, int y, int w, int h, SDL_Rect *srcRect, int center)
{
	SDL_Rect dstRect;
	
	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = w;
	dstRect.h = h;

	if (center)
	{
		dstRect.x -= (dstRect.w / 2);
		dstRect.y -= (dstRect.h / 2);
	}

	SDL_RenderCopy(app.renderer, texture, srcRect, &dstRect);
}

void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);
}

void drawRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	
	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderFillRect(app.renderer, &rect);
}

void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	
	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawRect(app.renderer, &rect);
}

void scrollBackground(float x, float y)
{
	int i;
	
	for (i = 0 ; i < 4 ; i++)
	{
		backgroundPoint[i].x += x;
		backgroundPoint[i].y += y;
		
		if (backgroundPoint[i].x < 0)
		{
			backgroundPoint[i].x += (SCREEN_WIDTH * 2);
		}
		
		if (backgroundPoint[i].x >= SCREEN_WIDTH)
		{
			backgroundPoint[i].x -= (SCREEN_WIDTH * 2);
		}
		
		if (backgroundPoint[i].y < 0)
		{
			backgroundPoint[i].y += (SCREEN_HEIGHT * 2);
		}
		
		if (backgroundPoint[i].y >= SCREEN_HEIGHT)
		{
			backgroundPoint[i].y -= (SCREEN_HEIGHT * 2);
		}
	}
}

void drawBackground(SDL_Texture *texture, SDL_Rect *srcRect)
{
	int i;
	
	for (i = 0 ; i < 4 ; i++)
	{
		blitRectScaled(texture, backgroundPoint[i].x, backgroundPoint[i].y, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, srcRect, 0);
	}
}

static void initColor(SDL_Color *c, int r, int g, int b)
{
	memset(c, 0, sizeof(SDL_Color));
	c->r = r;
	c->g = g;
	c->b = b;
	c->a = 255;
}
