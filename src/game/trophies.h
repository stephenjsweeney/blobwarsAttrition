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
#include "../json/cJSON.h"
#include <time.h>

#define TROPHIES_PER_PAGE	4

extern void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center);
extern void blitRectRotated(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, float angle);
extern void blitRectScaled(SDL_Texture *texture, int x, int y, int w, int h, SDL_Rect *srcRect, int center);
extern void clearControl(int type);
extern void doWidgets(void);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void drawWidgets(void);
extern Atlas *getImageFromAtlas(char *filename);
extern Texture *getTexture(const char *filename);
extern int isControl(int type);
extern float limit(float i, float low, float high);
extern long lookup(const char *name);
extern float mod(float n, float x);
extern void playSound(int snd, int ch);
extern char *readFile(const char *filename);
extern void saveScreenshot(char *name);
extern void textSize(char *text, int size, int *w, int *h);
extern char *timeToDate(long millis);

extern App app;
extern Colors colors;
extern Game game;
