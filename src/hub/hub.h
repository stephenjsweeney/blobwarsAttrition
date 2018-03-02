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

enum
{
	SHOW_NONE,
	SHOW_WIDGETS,
	SHOW_STATS,
	SHOW_TROPHIES
};

#define CURSOR_SPEED	8

extern void animateSprites(void);
extern void awardTrophy(char *id);
extern void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center);
extern void blitRectScaled(SDL_Texture *texture, int x, int y, int w, int h, SDL_Rect *srcRect, int center);
extern int clearControl(int type);
extern void doStats(void);
extern void doTrophies(void);
extern void doWidgets(void);
extern void drawBackground(SDL_Texture *texture, SDL_Rect *srcRect);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawStats(void);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void drawTrophies(void);
extern void drawWidgetFrame(void);
extern void drawWidgets(void);
extern void endSectionTransition(void);
extern SDL_Rect *getCurrentFrame(Sprite *s);
extern int getDistance(int x1, int y1, int x2, int y2);
extern Atlas *getImageFromAtlas(char *filename);
extern int getMissionStatus(char *id);
extern Sprite *getSprite(char *name);
extern Texture *getTexture(const char *filename);
extern Widget *getWidget(char *name, char *group);
extern void hideAllWidgets(void);
extern void initOptions(void (*callback)(void));
extern void initWorld(void);
extern int isControl(int type);
extern void limitTextWidth(int width);
extern void loadMusic(char *filename);
extern void playMusic(int loop);
extern void playSound(int snd, int ch);
extern double randF(void);
extern char *readFile(const char *filename);
extern void saveGame(void);
extern void scrollBackground(float x, float y);
extern Widget *selectWidgetAt(int x, int y);
extern void showWidgetGroup(char *group);
extern void startSectionTransition(void);
extern void stopMusic(void);
extern void initStatsDisplay(void);

extern App app;
extern Colors colors;
extern Dev dev;
extern Game game;

