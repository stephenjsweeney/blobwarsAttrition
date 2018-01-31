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

#include "bob.h"

static SDL_Rect *getCurrentSprite(void);
static void (*superAnimate)(void);
static void animate(void);
static void load(cJSON *root);
static void save(cJSON *root);

void initBob(void)
{
	Unit *u;
	
	u = createUnit();
	
	u->type = ET_BOB;
	
	u->sprite[FACING_LEFT] = getSprite("BobLeft");
	u->sprite[FACING_RIGHT] = getSprite("BobRight");
	u->sprite[FACING_DIE] = getSprite("BobSpin");
	
	superAnimate = u->animate;
	
	u->getCurrentSprite = getCurrentSprite;
	u->animate = animate;
	u->load = load;
	u->save = save;
}

void addBobItem(Item *i)
{
	
}

int numCarriedItems(void)
{
	return 0;
}

static SDL_Rect *getCurrentSprite(void)
{
	if (world.bob->alive == ALIVE_ALIVE && world.bob->stunTimer <= 0)
	{
		return &world.bob->sprite[world.bob->facing]->frames[0]->rect;
	}

	return &world.bob->sprite[FACING_DIE]->frames[0]->rect;
}

static void animate(void)
{
	if (world.bob->dx != 0 || world.bob->stunTimer > 0 || world.bob->flags & EF_WEIGHTLESS || world.bob->health <= 0)
	{
		superAnimate();
	}
}

static void load(cJSON *root)
{
	world.bob->x = cJSON_GetObjectItem(root, "x")->valueint;
	world.bob->y = cJSON_GetObjectItem(root, "y")->valueint;
	world.bob->facing = lookup(cJSON_GetObjectItem(root, "facing")->valuestring);
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "type", "Bob");
	cJSON_AddNumberToObject(root, "x", world.bob->x);
	cJSON_AddNumberToObject(root, "y", world.bob->y);
	cJSON_AddStringToObject(root, "facing", getLookupName("FACING_", world.bob->facing));
}
