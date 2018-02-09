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

#include "unit.h"

static void tick(void);
static void init(void);
static void attack(void);
static void applyDamage(int damage);
static int canFire(Entity *target);
static SDL_Rect *getCurrentSprite(void);
static void preFire(void);
static void load(cJSON *root);
static void save(cJSON *root);

Unit *createUnit(void)
{
	Unit *u;
	
	u = malloc(sizeof(Unit));
	memset(u, 0, sizeof(Unit));
	
	initEntity((Entity*)u);
	
	u->type = ET_ENEMY;
	
	u->oxygen = MAX_OXYGEN;

	u->canCarryItem = rand() % 100 < 85;

	if (world.isOutpostMission)
	{
		u->canCarryItem = 1;
		u->health = u->healthMax = rrnd(1, 4);
	}

	u->spriteTime = 0;
	u->spriteFrame = 0;

	u->startX = u->startY = -1;

	u->init = init;
	u->tick = tick;
	u->preFire = preFire;
	u->attack = attack;
	u->canFire = canFire;
	u->getCurrentSprite = getCurrentSprite;
	u->applyDamage = applyDamage;
	u->load = load;
	u->save = save;
	
	return u;
}

static void init(void)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (u->startX == -1 && u->startY == -1)
	{
		u->startX = (int) u->x;
		u->startY = (int) u->y;
	}

	if (u->isMissionTarget)
	{
		u->flags |= EF_BOMB_SHIELD;
	}
}

static void tick(void)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (u->alive == ALIVE_ALIVE)
	{
		u->reload = limit(u->reload - 1, 0, FPS);
	}

	switch (u->environment)
	{
		case ENV_AIR:
			u->oxygen = limit(u->oxygen + 4, 0, MAX_OXYGEN);
			break;

		case ENV_WATER:
			u->oxygen = limit(u->oxygen - 1, 0, MAX_OXYGEN);
			if (u->oxygen == 0 && world.frameCounter % 30 == 0)
			{
				u->health--;
			}
			break;

		case ENV_SLIME:
		case ENV_LAVA:
			if (u->alive == ALIVE_ALIVE)
			{
				u->health = 0;
			}
			break;
	}

	if (u->flags & EF_WATER_BREATHING)
	{
		u->oxygen = MAX_OXYGEN;
	}

	if (u->spawnedIn)
	{
		if (getDistance(u->x, u->y, world.bob->x, world.bob->y) < 1000)
		{
			u->spawnedInTimer = FPS * 5;
		}

		if (--u->spawnedInTimer <= 0)
		{
			u->alive = ALIVE_DEAD;
		}
	}
}

static void reappear(void)
{
	int valid, r;

	valid = 0;

	do
	{
		r = rand() % MAX_CHECKPOINTS;
		self->x = world.bob->checkpoints[r].x;
		self->y = world.bob->checkpoints[r].y;
		valid = (self->x != 0 && self->y != 0);
	}
	while (!valid);

	self->y -= (self->h + 10);
	
	self->action = self->walk;
	
	self->flags &= ~EF_GONE;

	addTeleportStars(self);

	playSound(SND_APPEAR, CH_ANY);
}

static void applyDamage(int damage)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (u->health < 0)
	{
		u->health = 0;
		u->alive = ALIVE_ALIVE;
	}
	
	u->health -= damage;

	if (u->health > 0)
	{
		u->thinkTime = 0;

		u->facing = u->x < world.bob->x ? FACING_RIGHT : FACING_LEFT;

		if (u->isMissionTarget && rand() % 100 < 10)
		{
			u->action = reappear;
			u->flags |= EF_GONE;
			u->thinkTime = rrnd(FPS, FPS * 2);
			addTeleportStars(self);
			playSound(SND_APPEAR, CH_ANY);
		}
	}
}

static void attack(void)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (u->canFire((Entity*)world.bob))
	{
		switch (u->weaponType)
		{
			case WPN_AIMED_PISTOL:
				fireAimedShot(self);
				break;

			case WPN_MACHINE_GUN:
				fireMachineGun(self);
				break;

			case WPN_GRENADES:
				fireGrenade(self);
				break;

			case WPN_PLASMA:
				firePlasma(self);
				break;

			case WPN_SPREAD:
				fireSpread(self, 3);
				break;

			case WPN_LASER:
				fireLaser(self);
				break;

			case WPN_SHOTGUN:
				fireShotgun(self);
				break;

			case WPN_MISSILE:
				fireMissile(self);
				break;

			default:
				printf("Can't fire weapon: %d\n", u->weaponType);
				break;
		}
	}
}

static void preFire(void)
{
	Unit *u;
	
	u = (Unit*)self;
	
	if (!(u->flags & EF_WEIGHTLESS))
	{
		if (world.bob->y < u->y && u->isOnGround && rand() % 4 == 0)
		{
			u->dy = JUMP_POWER;
		}
	}

	u->facing = (world.bob->x < u->x) ? FACING_LEFT : FACING_RIGHT;

	if (u->reload > 0)
	{
		return;
	}

	u->attack();
	
	u->shotsToFire--;

	if (u->shotsToFire == 0)
	{
		u->action = u->walk;
	}
}

static int canFire(Entity *target)
{
	return 0;
}

static SDL_Rect *getCurrentSprite(void)
{
	if (self->alive == ALIVE_ALIVE)
	{
		return &self->sprite[self->facing]->frames[self->spriteFrame]->rect;
	}

	return &self->sprite[FACING_DIE]->frames[self->spriteFrame]->rect;
}

static void load(cJSON *root)
{
	Unit *u;
	
	u = (Unit*)self;
	
	u->canCarryItem = cJSON_GetObjectItem(root, "canCarryItem")->valueint;
	u->startX = cJSON_GetObjectItem(root, "startX")->valueint;
	u->startY = cJSON_GetObjectItem(root, "startY")->valueint;
	u->isMissionTarget = cJSON_GetObjectItem(root, "isMissionTarget")->valueint;
	u->health = cJSON_GetObjectItem(root, "health")->valueint;
	u->healthMax = cJSON_GetObjectItem(root, "healthMax")->valueint;
}

static void save(cJSON *root)
{
	Unit *u;
	
	u = (Unit*)self;
	
	cJSON_AddStringToObject(root, "type", u->unitType);
	cJSON_AddNumberToObject(root, "canCarryItem", u->canCarryItem);
	cJSON_AddNumberToObject(root, "startX", u->startX);
	cJSON_AddNumberToObject(root, "startY", u->startY);
	cJSON_AddNumberToObject(root, "isMissionTarget", u->isMissionTarget);
	cJSON_AddNumberToObject(root, "health", u->health);
	cJSON_AddNumberToObject(root, "healthMax", u->healthMax);
}
