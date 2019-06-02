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

#define MAX_RIDERS		256
#define MAX_TOUCHED		256

extern void addTeleportStar(float x, float y);
extern void addTeleportStars(Entity *e);
extern void addToQuadtree(Entity *e, Quadtree *root);
extern void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern Entity **getAllEntsWithin(int x, int y, int w, int h, Entity *ignore);
extern int getDistance(int x1, int y1, int x2, int y2);
extern Sprite *getSprite(char *name);
extern Texture *getTexture(const char *filename);
extern int isLiquid(int x, int y);
extern int isSolid(int x, int y);
extern int isWalkable(int x, int y);
extern int isWithinMap(int x, int y);
extern float limit(float i, float a, float b);
extern void playBattleSound(int snd, int ch, int x, int y);
extern void removeFromQuadtree(Entity *e, Quadtree *root);
extern void terminateJetpack(void);

extern App app;
extern Camera camera;
extern Entity *self;
extern World world;
