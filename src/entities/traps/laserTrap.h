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

extern void initEntity(Entity *e);
extern void observeActivation(Entity *e);
extern int isOnScreen(Entity *e);
extern void setGameplayMessage(int type, char *format, ...);
extern Sprite *getSprite(char *name);
extern int rrnd(int low, int high);
extern void stunBob(void);
extern void addSparkParticles(float x, float y);
extern void addSmallFleshChunk(float x, float y);
extern void swapSelf(Entity *e);
extern void playSound(int snd, int ch);
extern void playBattleSound(int snd, int ch, int x, int y);

extern Entity *self;
extern World world;
