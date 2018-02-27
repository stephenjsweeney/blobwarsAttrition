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

#include "destructable.h"

static void init(void);
static void applyDamage(int amount);
static void action(void);
static void die(void);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initDestructable(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_DESTRUCTABLE;
	
	s->isMissionTarget = 1;

	STRNCPY(s->spriteName, "Crate4", MAX_NAME_LENGTH);

	s->flags |= EF_EXPLODES;

	s->health = s->healthMax = 10;
	
	s->init = init;
	s->applyDamage = applyDamage;
	s->action = action;
	s->die = die;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite(s->spriteName);
}

static void applyDamage(int amount)
{
	if (self->health > 0)
	{
		self->health -= amount;
	}
}

static void action(void)
{
	Structure *s;
	int mx, my;
	
	s = (Structure*)self;
	
	if (s->health <= 0)
	{
		s->health--;

		if (s->health % 3 == 0)
		{
			mx = (int) ((s->x + (s->w / 2)) / MAP_TILE_SIZE);
			my = (int) ((s->y + s->h) / MAP_TILE_SIZE);
			addScorchDecal(mx, my);

			addExplosion(s->x, s->y, 50, self);
			s->dx = rrnd(-10, 10);
			s->dy = rrnd(-10, 10);
		}

		if (s->health <= -50)
		{
			dropCarriedItem();

			updateObjective(s->name);

			if (strlen(s->targetNames) > 0)
			{
				activateEntities(s->targetNames, 1);
			}

			s->alive = ALIVE_DEAD;
		}
	}
}

static void die(void)
{
	
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->health = cJSON_GetObjectItem(root, "health")->valueint;
	s->healthMax = cJSON_GetObjectItem(root, "healthMax")->valueint;
	STRNCPY(s->spriteName, cJSON_GetObjectItem(root, "spriteName")->valuestring, MAX_NAME_LENGTH);
	
	if (cJSON_GetObjectItem(root, "targetNames"))
	{
		STRNCPY(s->targetNames, cJSON_GetObjectItem(root, "targetNames")->valuestring, MAX_DESCRIPTION_LENGTH);
	}
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddNumberToObject(root, "health", s->health);
	cJSON_AddNumberToObject(root, "healthMax", s->healthMax);
	cJSON_AddStringToObject(root, "spriteName", s->spriteName);
	cJSON_AddStringToObject(root, "targetNames", s->targetNames);
}
