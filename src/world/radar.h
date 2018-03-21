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

#include "../common.h"

enum
{
	M_MIA,
	M_ITEM,
	M_ENEMY,
	M_MAX
};

#define MAX_BLIPS			32
#define MAX_MARKERS			12
#define OFFSET_X 			((SCREEN_WIDTH - (RADAR_TILE_SIZE * VIEW_SIZE_X)) / 2)
#define OFFSET_Y			((SCREEN_HEIGHT - (RADAR_TILE_SIZE * VIEW_SIZE_Y)) / 2)
#define RADAR_TILE_SIZE 	16
#define VIEW_SIZE_X			50
#define VIEW_SIZE_Y			30

extern void blitRectRotated(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, float angle);
extern void blitRectScaled(SDL_Texture *texture, int x, int y, int w, int h, SDL_Rect *srcRect, int center);
extern void clearControl(int type);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void endSectionTransition(void);
extern void exitRadar(void);
extern Atlas *getImageFromAtlas(char *filename);
extern Texture *getTexture(const char *filename);
extern int isControl(int type);
extern int isWithinMap(int x, int y);
extern float limit(float i, float low, float high);
extern void pauseSound(int pause);
extern void startSectionTransition(void);

extern App app;
extern Colors colors;
extern World world;
