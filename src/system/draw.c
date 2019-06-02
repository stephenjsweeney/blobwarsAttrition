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

#include "draw.h"

static void initColor(SDL_Color *c, int r, int g, int b);

static PointF backgroundPoint[4];

void initBackground(void)
{
	backgroundPoint[0].x = -app.config.winWidth / 2;
	backgroundPoint[0].y = -app.config.winHeight / 2;

	backgroundPoint[1].x = app.config.winWidth / 2;
	backgroundPoint[1].y = -app.config.winHeight / 2;

	backgroundPoint[2].x = -app.config.winWidth / 2;
	backgroundPoint[2].y = app.config.winHeight / 2;

	backgroundPoint[3].x = app.config.winWidth / 2;
	backgroundPoint[3].y = app.config.winHeight / 2;
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

	app.backBuffer = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.config.winWidth, app.config.winHeight);

	app.uiBuffer = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, UI_WIDTH, UI_HEIGHT);
	SDL_SetTextureBlendMode(app.uiBuffer, SDL_BLENDMODE_BLEND);

	app.uiOffset.x = (app.config.winWidth / 2) - (UI_WIDTH / 2);
	app.uiOffset.y = (app.config.winHeight / 2) - (UI_HEIGHT / 2);
}

void prepareScene(void)
{
	SDL_SetRenderTarget(app.renderer, app.uiBuffer);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0);
	SDL_RenderClear(app.renderer);

	SDL_SetRenderTarget(app.renderer, app.backBuffer);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
	SDL_Rect uiDest;

	uiDest.w = UI_WIDTH;
	uiDest.h = UI_HEIGHT;
	uiDest.x = (app.config.winWidth / 2) - (UI_WIDTH / 2);
	uiDest.y = (app.config.winHeight / 2) - (UI_HEIGHT / 2);

	if (dev.debug)
	{
		drawText(5, app.config.winHeight - 25, 14, TA_LEFT, colors.white, "DEBUG MODE");

		if (dev.showFPS)
		{
			drawText(app.config.winWidth - 5, app.config.winHeight - 25, 14, TA_RIGHT, colors.white, "FPS: %d", dev.fps);
		}
	}

	SDL_SetRenderTarget(app.renderer, NULL);
	SDL_RenderCopy(app.renderer, app.backBuffer, NULL, NULL);
	SDL_RenderCopy(app.renderer, app.uiBuffer, NULL, &uiDest);
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
			backgroundPoint[i].x += (app.config.winWidth * 2);
		}

		if (backgroundPoint[i].x >= app.config.winWidth)
		{
			backgroundPoint[i].x -= (app.config.winWidth * 2);
		}

		if (backgroundPoint[i].y < 0)
		{
			backgroundPoint[i].y += (app.config.winHeight * 2);
		}

		if (backgroundPoint[i].y >= app.config.winHeight)
		{
			backgroundPoint[i].y -= (app.config.winHeight * 2);
		}
	}
}

void drawBackground(SDL_Texture *texture)
{
	int i;
	SDL_Rect dstRect;

	for (i = 0 ; i < 4 ; i++)
	{
		dstRect.x = backgroundPoint[i].x;
		dstRect.y = backgroundPoint[i].y;
		dstRect.w = app.config.winWidth - 1;
		dstRect.h = app.config.winHeight - 1;

		SDL_RenderCopy(app.renderer, texture, NULL, &dstRect);
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

void saveScreenshot(char *name)
{
	char *filename;
	SDL_Surface *screenshot;

	if (name != NULL)
	{
		filename = buildFormattedString("%s/%d/%s.png", app.saveDir, game.saveSlot, name);
	}
	else
	{
		filename = buildFormattedString("%s/%d.png", dev.screenshotFolder, SDL_GetTicks());
	}

	screenshot = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(app.renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);
	SDL_SavePNG(screenshot, filename);
	SDL_FreeSurface(screenshot);

	free(filename);
}
