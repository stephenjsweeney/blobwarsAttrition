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

#include "../../common.h"

extern void addExplosion(float x, float y, int radius, Entity *owner);
extern void addRandomItems(int x, int y);
extern void addRandomWeapon(int x, int y);
extern void addScorchDecal(int x, int y);
extern void addSmokeParticles(float x, float y, int rising);
extern Unit *createUnit(void);
extern void dropCarriedItem(void);
extern int enemyCanSeePlayer(Entity *e);
extern void fireTriggers(char *name);
extern int getDistance(int x1, int y1, int x2, int y2);
extern float limit(float i, float a, float b);
extern void playBattleSound(int snd, int ch, int x, int y);
extern double randF(void);
extern int rrnd(int low, int high);
extern void throwDebris(float x, float y, int amount);
extern void updateObjective(char *targetName);

extern Dev dev;
extern Entity *self;
extern Game game;
extern World world;
