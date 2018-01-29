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

extern void dropCarriedItem(void);
extern int getDistance(int x1, int y1, int x2, int y2);
extern double randF(void);
extern void throwFleshChunks(float x, float y, int amount);
extern void addRandomWeapon(float x, float y);
extern float limit(float i, float a, float b);
extern void playSound(int snd, int ch);
extern void addBloodDecal(int x, int y);
extern void updateObjective(char *targetName);
extern int enemyCanSeePlayer(Entity *e);
extern void addDefeatedTarget(char *name);
extern void fireTriggers(char *name);
extern void addRandomItems(float x, float y);
extern int rrnd(int low, int high);

extern Dev dev;
extern Entity *self;
extern Game game;
extern World world;
