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

void blitFlip(SDL_Texture *texture, int x, int y, int center, SDL_RendererFlip flip)
{
	SDL_Rect dstRect;
	
	dstRect.x = x;
	dstRect.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);

	SDL_RenderCopyEx(app.renderer, texture, NULL, &dstRect, 0, NULL, flip);
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

void blitRotated(SDL_Texture *texture, int x, int y, float angle)
{
	SDL_Rect dstRect;
	
	dstRect.x = x;
	dstRect.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);
	dstRect.x -= (dstRect.w / 2);
	dstRect.y -= (dstRect.h / 2);

	SDL_RenderCopyEx(app.renderer, texture, NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
}

void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);
}

void drawCircle(int cx, int cy, int radius, int r, int g, int b, int a)
{
	int x = radius;
	int y = 0;
	int radiusError = 1 - x;
	SDL_Point p[8];

	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);

	while (x >= y)
	{
		p[0].x =  x + cx; p[0].y = y + cy;
		p[1].x =  y + cx; p[1].y =  x + cy;
		p[2].x = -x + cx; p[2].y =  y + cy;
		p[3].x = -y + cx; p[3].y =  x + cy;
		p[4].x = -x + cx; p[4].y = -y + cy;
		p[5].x = -y + cx; p[5].y = -x + cy;
		p[6].x =  x + cx; p[6].y = -y + cy;
		p[7].x =  y + cx; p[7].y = -x + cy;
		SDL_RenderDrawPoints(app.renderer, p, 8);

		y++;

		if (radiusError < 0)
		{
			radiusError += 2 * y + 1;
		}
		else
		{
			x--;
			radiusError += 2 * (y - x) + 1;
		}
	}

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
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

static void initColor(SDL_Color *c, int r, int g, int b)
{
	memset(c, 0, sizeof(SDL_Color));
	c->r = r;
	c->g = g;
	c->b = b;
	c->a = 255;
}
