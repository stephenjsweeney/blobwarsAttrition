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

#include "boss.h"

static void load(cJSON *root);
static void save(cJSON *root);
static void init(void);

Boss *initBoss(void)
{
	Boss *b;
	
	b = malloc(sizeof(Boss));
	memset(b, 0, sizeof(Boss));
	
	initEntity((Entity*)b);
	
	b->type = ET_BOSS;
	
	b->sprite[FACING_LEFT] = b->sprite[FACING_RIGHT] = b->sprite[FACING_DIE] = getSprite("Boss");
	
	b->isMissionTarget = 1;

	b->spriteFrame = 0;
	b->spriteTime = 0;

	world.boss = b;

	b->flags |= EF_ALWAYS_PROCESS | EF_BOMB_SHIELD | EF_GONE;
	
	b->init = init;
	b->load = load;
	b->save = save;
	
	return b;
}

static void init(void)
{
}

static void load(cJSON *root)
{
}

static void save(cJSON *root)
{
}
