/*
Copyright (C) 2018-2019 Parallel Realities

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

static void (*superTick)(void);
static void tick(void);
static void lookForEnemies(void);
static void preFire(void);
static void attack(void);

static Entity *target;
static Sprite *aimedSprite;
static int exitMission;

Entity *initTeeka(void)
{
	Unit *u;

	u = createUnit();

	u->unitType = "Teeka";

	u->type = ET_TEEKA;

	u->flags |= EF_IMMUNE;

	u->action = lookForEnemies;

	u->weaponType = WPN_AIMED_PISTOL;

	u->sprite[FACING_LEFT] = getSprite("TeekaLeft");
	u->sprite[FACING_RIGHT] = getSprite("TeekaRight");
	u->sprite[FACING_DIE] = getSprite("TeekaLeft");

	u->health = u->healthMax = 9999;

	superTick = u->tick;

	u->tick = tick;

	exitMission = 0;

	aimedSprite = getSprite("AimedShot");

	return (Entity*)u;
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

	superTick();
}

static void lookForEnemies(void)
{
	Entity *e;
	float distance, range;
	Unit *u;

	u = (Unit*)self;

	u->thinkTime = rrnd(FPS / 2, FPS);

	target = NULL;

	distance = 800;

	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_ENEMY)
		{
			range = getDistance(u->x, u->y, e->x, e->y);

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
		u->shotsToFire = rrnd(3, 5);

		u->action = preFire;
	}
	else if (exitMission)
	{
		addTeleportStars(self);
		u->alive = ALIVE_DEAD;
		playBattleSound(SND_APPEAR, u->uniqueId % MAX_SND_CHANNELS, u->x, u->y);
	}
	else
	{
		u->facing = rand() % 2 == 0 ? FACING_LEFT : FACING_RIGHT;
	}
}

static void preFire(void)
{
	Unit *u;

	u = (Unit*)self;

	if (u->reload > 0)
	{
		return;
	}

	if (target->y < u->y && u->isOnGround && rand() % 10 == 0)
	{
		u->dy = JUMP_POWER;
	}

	attack();

	if (--u->shotsToFire <= 0)
	{
		u->thinkTime = FPS;
		u->action = lookForEnemies;
	}
}

static void attack(void)
{
	Bullet *bullet;
	float dx, dy;

	getSlope(target->x, target->y, self->x, self->y, &dx, &dy);

	bullet = createBaseBullet((Unit*)self, aimedSprite->w);
	bullet->facing = self->facing;
	bullet->damage = 1;
	bullet->owner = self;
	bullet->health = FPS * 3;
	bullet->weaponType = WPN_AIMED_PISTOL;
	bullet->dx = dx * 9;
	bullet->dy = dy * 9;
	bullet->sprite[0] = bullet->sprite[1] = aimedSprite;
	bullet->health *= 2;

	((Unit*)self)->reload = 8;

	playBattleSound(SND_PISTOL, self->uniqueId % MAX_SND_CHANNELS, self->x, self->y);
}

void teekaExitMission(void)
{
	exitMission = 1;
}
