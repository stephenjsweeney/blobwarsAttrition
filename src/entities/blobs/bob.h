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
#include "../../json/cJSON.h"

#define FLY_ACCEL	0.1
#define FLY_SPEED	8
#define SWIM_SPEED	3
#define WALK_SPEED	5

extern void addFlameParticles(float x, float y);
extern void addTeleportStars(Entity *e);
extern void clearControl(int type);
extern void fireGrenade(Entity *e);
extern void fireLaser(Entity *e);
extern void firePistol(void);
extern void firePlasma(Entity *e);
extern void fireSpread(Entity *e, int n);
extern char *getLookupName(const char *prefix, long num);
extern Sprite *getSprite(char *name);
extern void initEntity(Entity *e);
extern int isControl(int type);
extern int isSolid(int x, int y);
extern float limit(float i, float a, float b);
extern long lookup(const char *name);
extern void playSound(int snd, int ch);
extern double randF(void);
extern int rrnd(int low, int high);
extern void setGameplayMessage(int type, char *format, ...);
extern void throwFleshChunks(float x, float y, int amount);

extern App app;
extern Dev dev;
extern Game game;
extern World world;

