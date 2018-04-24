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

extern void blitRectScaled(SDL_Texture *texture, int x, int y, int w, int h, SDL_Rect *srcRect, int center);
extern void clearControls(void);
extern void destroyWorld(void);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void endSectionTransition(void);
extern Atlas *getImageFromAtlas(char *filename);
extern Texture *getTexture(const char *filename);
extern void initEnding(void);
extern void initHub(void);
extern void initWorld(void);
extern int isAcceptControl(void);
extern float limit(float i, float low, float high);
extern void playSound(int snd, int ch);
extern void restoreGameState(void);
extern void saveGame(int isTempFile);
extern void saveWorld(void);
extern void startSectionTransition(void);
extern char *buildFormattedString(const char *format, ...);
extern int renameFile(char *src, char *dest);

extern App app;
extern Colors colors;
extern Game game;
extern World world;
