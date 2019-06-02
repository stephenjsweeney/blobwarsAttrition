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

#include "../common.h"

#define STATS_PER_PAGE	7

extern void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center);
extern void clearControl(int type);
extern void doWidgets(void);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void drawWidgets(void);
extern Atlas *getImageFromAtlas(char *filename);
extern int getPercent(float current, float total);
extern Texture *getTexture(const char *filename);
extern int isControl(int type);
extern float limit(float i, float low, float high);
extern void playSound(int snd, int ch);
extern char *timeToString(int seconds, int showHours);

extern App app;
extern Colors colors;
extern Game game;
