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

#include "teeka.h"

static void tick(void);
static void lookForEnemies(void);
static void preFire(void);
static void attack(void);

static Entity *target;
static int aimedSprite;
static int exitMission;

void initTeeka(Entity *e)
{
	e->type = ET_TEEKA;

	aimedSprite = getSpriteIndex("AimedShot");
	
	e->flags |= EF_IMMUNE;

	e->action = lookForEnemies;

	e->weaponType = WPN_AIMED_PISTOL;

	e->sprite[FACING_LEFT] = getSpriteIndex("TeekaLeft");
	e->sprite[FACING_RIGHT] = getSpriteIndex("TeekaRight");
	e->sprite[FACING_DIE] = getSpriteIndex("TeekaLeft");

	e->health = e->healthMax = 9999;
	
	e->tick = tick;
}

static void tick(void)
{
	self->health = self->healthMax = 9999;

	if (target != NULL)
	{
		self->facing = (target->x < self->x) ? FACING_LEFT : FACING_RIGHT;

		if (target->alive == ALIVE_DEAD)
		{
			target = NULL;

			self->action = lookForEnemies;
		}
	}

	unitTick();
}

static void lookForEnemies(void)
{
	Entity *e;
	float distance, range;
	
	self->thinkTime = rrnd(FPS / 2, FPS);

	target = NULL;
	
	distance = 800;

	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_ENEMY)
		{
			range = getDistance(self->x, self->y, e->x, e->y);

			if (range < distance)
			{
				if (hasLineOfSight(self, e))
				{
					target = e;
					distance = range;
				}
			}
		}
	}

	if (target != NULL)
	{
		self->shotsToFire = rrnd(3, 5);

		self->action = preFire;
	}
	else if (exitMission)
	{
		addTeleportStars(self);
		self->alive = ALIVE_DEAD;
		playSound(SND_APPEAR, CH_ANY);
	}
	else
	{
		self->facing = rand() % 2 == 0 ? FACING_LEFT : FACING_RIGHT;
	}
}

static void preFire(void)
{
	if (self->reload > 0)
	{
		return;
	}

	if (target->y < self->y && self->isOnGround && rand() % 10 == 0)
	{
		self->dy = JUMP_POWER;
	}

	attack();

	self->shotsToFire--;

	if (self->shotsToFire == 0)
	{
		self->thinkTime = FPS;
	}
}

static void attack(void)
{
	Entity *bullet;
	float dx, dy;
	
	getSlope(target->x, target->y, self->x, self->y, &dx, &dy);

	bullet = createBaseBullet(self);
	bullet->x = self->x;
	bullet->y = (self->y + self->h / 2) - 3;
	bullet->facing = self->facing;
	bullet->damage = 1;
	bullet->owner = self;
	bullet->health = FPS * 3;
	bullet->weaponType = WPN_AIMED_PISTOL;
	bullet->dx = dx * 9;
	bullet->dy = dy * 9;
	bullet->sprite[0] = bullet->sprite[1] = aimedSprite;
	bullet->health *= 2;

	self->reload = 5;
}
