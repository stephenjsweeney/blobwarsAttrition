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

#include "../../../common.h"

extern void initBoss(Entity *e);
extern int rrnd(int low, int high);
extern void addTeleportStars(Entity *e);
extern void playMusic(char *filename, int loop);
extern int isPlayingMusic(void);
extern float limit(float i, float a, float b);
extern double randF(void);
extern void playSound(int snd, int ch);
extern void animateEntity(Entity *e);
extern Entity *createBaseBullet(Entity *owner);
extern int getSpriteIndex(char *name);
extern int getDistance(int x1, int y1, int x2, int y2);
extern void getSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
extern int enemyCanSeePlayer(Entity *e);
extern void updateObjective(char *targetName);
extern void addDefeatedTarget(char *name);

extern Entity *self;
extern Game game;
extern World world;
