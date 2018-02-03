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

extern Texture *getTexture(const char *filename);
extern void initObjectives(void);
extern Entity *getRandomObjectiveEntity(void);
extern void doBob(void);
extern void doLocationTriggers(void);
extern void dropCarriedItems(void);
extern void playSound(int snd, int ch);
extern void initEnding(void);
extern void initTitle(void);
extern float cameraChase(Entity *e, int maxSpeed);
extern int rrnd(int low, int high);
extern void hideAllWidgets(void);
extern void resetAtCheckpoint(void);
extern void cameraTrack(Entity *e);
extern void stopMusic(void);
extern void animateSprites(void);
extern void addTeleportStars(Entity *e);
extern double randF(void);
extern int isSolid(int x, int y);
extern int isLiquid(int x, int y);
extern int isWalkable(int x, int y);
extern void doEntities(void);
extern void doParticles(void);
extern void doHud(void);
extern Entity *createEntity(char *typeStr);
extern void dropRandomCherry(int x, int y);
extern void addRandomWeapon(int x, int y);
extern void drawEntities(int plane);
extern void drawMap(void);
extern void blitScaled(SDL_Texture *texture, int x, int y, int w, int h, int center);
extern void clearScreen(void);

extern App app;
extern Dev dev;
extern Game game;
extern World world;
