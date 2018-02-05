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

extern void initEntity(Entity *e);
extern int rrnd(int low, int high);
extern float limit(float i, float a, float b);
extern int getDistance(int x1, int y1, int x2, int y2);
extern void fireAimedShot(Entity *e);
extern void fireMachineGun(Entity *e);
extern void fireGrenade(Entity *e);
extern void firePlasma(Entity *e);
extern void fireSpread(Entity *e, int n);
extern void fireLaser(Entity *e);
extern void fireShotgun(Entity *e);
extern void fireMissile(Entity *e);
extern void addTeleportStars(Entity *e);
extern void playSound(int snd, int ch);

extern Entity *self;
extern World world;
