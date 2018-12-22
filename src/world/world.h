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

extern void addKeysFromStash(void);
extern void addRandomWeapon(int x, int y);
extern void addTeleportStars(Entity *e);
extern void animateSprites(void);
extern void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center);
extern void blitScaled(SDL_Texture *texture, int x, int y, int w, int h, int center);
extern float cameraChase(Entity *e, int maxSpeed);
extern void cameraTrack(Entity *e);
extern void clearControl(int type);
extern void clearControls(void);
extern void clearScreen(void);
extern void clipCamera(void);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern Entity *createEntity(char *typeStr);
extern void destroyEntities(void);
extern void destroyObjectives(void);
extern void destroyParticles(void);
extern void destroyQuadtree(void);
extern void destroyTriggers(void);
extern void doBob(void);
extern void doEntities(void);
extern void doEntitiesStatic(void);
extern void doHud(void);
extern void doLocationTriggers(void);
extern void doParticles(void);
extern void doPlayer(void);
extern void doStats(void);
extern void doTrophies(void);
extern void doWidgets(void);
extern void drawEntities(int plane);
extern void drawHud(void);
extern void drawMap(void);
extern void drawMissionStatus(int showFirePrompt);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawParticles(int plane);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawStats(void);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void drawTrophies(void);
extern void drawWidgetFrame(void);
extern void drawWidgets(void);
extern void dropCarriedItems(void);
extern void dropRandomCherry(int x, int y);
extern void endSectionTransition(void);
extern int getDistance(int x1, int y1, int x2, int y2);
extern Atlas *getImageFromAtlas(char *filename);
extern int getMissionStatus(char *id);
extern Entity *getRandomObjectiveEntity(void);
extern Texture *getTexture(const char *filename);
extern Widget *getWidget(char *name, char *group);
extern void hideAllWidgets(void);
extern void initEffects(void);
extern void initEntities(void);
extern void initHud(void);
extern void initItems(void);
extern void initMap(void);
extern void initObjectives(void);
extern void initOptions(void (*callback)(void));
extern void initParticles(void);
extern void initPostMission(void);
extern void initQuadtree(Quadtree *root);
extern void initRadar(void);
extern void initStatsDisplay(void);
extern void initWeapons(void);
extern int isAcceptControl(void);
extern int isControl(int type);
extern int isLiquid(int x, int y);
extern int isOnScreen(Entity *e);
extern int isSolid(int x, int y);
extern int isWalkable(int x, int y);
extern float limit(float i, float a, float b);
extern void limitTextWidth(int width);
extern void loadMusic(char *filename);
extern void loadWorld(char *id);
extern void pauseSound(int pause);
extern void playBattleSound(int snd, int ch, int x, int y);
extern void playMusic(int loop);
extern void playSound(int snd, int ch);
extern double randF(void);
extern void resetAtCheckpoint(void);
extern void retryMission(void);
extern void returnToHub(void);
extern void returnToTitle(void);
extern int rrnd(int low, int high);
extern void showWidgetGroup(char *group);
extern void startSectionTransition(void);
extern void stopMusic(void);

extern App app;
extern Camera camera;
extern Colors colors;
extern Dev dev;
extern Entity *self;
extern Game game;
extern World world;
