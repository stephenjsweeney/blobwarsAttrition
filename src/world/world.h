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
	SHOW_NONE,
	SHOW_WIDGETS,
	SHOW_STATS,
	SHOW_TROPHIES,
	SHOW_QUIT
};

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
extern void doEntitiesStatic(void);
extern void doParticles(void);
extern void doHud(void);
extern Entity *createEntity(char *typeStr);
extern void dropRandomCherry(int x, int y);
extern void addRandomWeapon(int x, int y);
extern void drawEntities(int plane);
extern void drawMap(void);
extern void blitScaled(SDL_Texture *texture, int x, int y, int w, int h, int center);
extern void clearScreen(void);
extern void drawHud(void);
extern void initHud(void);
extern void initWeapons(void);
extern void initQuadtree(Quadtree *root);
extern void initParticles(void);
extern void initEffects(void);
extern void drawParticles(int plane);
extern void initItems(void);
extern void doPlayer(void);
extern int isOnScreen(Entity *e);
extern int isControl(int type);
extern void clearControl(int type);
extern void drawMissionStatus(void);
extern int isAcceptControl(void);
extern void clearControls(void);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void loadMusic(char *filename);
extern void playMusic(int loop);
extern void initRadar(void);
extern void startSectionTransition(void);
extern void endSectionTransition(void);
extern void drawWidgets(void);
extern void showWidgetGroup(char *group);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void doWidgets(void);
extern Widget *getWidget(char *name, char *group);
extern void destroyTriggers(void);
extern void destroyObjectives(void);
extern void destroyEntities(void);
extern void destroyParticles(void);
extern void destroyQuadtree(void);
extern void loadWorld(char *id);
extern void initMap(void);
extern void initEntities(void);
extern void initPostMission(void);
extern void addKeysFromStash(void);
extern void drawStats(void);
extern void doStats(void);
extern void doTrophies(void);
extern void drawTrophies(void);
extern void limitTextWidth(int width);

extern App app;
extern Colors colors;
extern Dev dev;
extern Entity *self;
extern Game game;
extern World world;
