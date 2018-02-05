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

#include "eyeDroid.h"

static void die(void);
static void tick(void);
static void lookForPlayer(void);
static void touch(Entity *other);
static void (*superTick)(void);
static void (*superTouch)(Entity *other);

void initEyeDroid(Unit *u)
{
	u->flags |= EF_WEIGHTLESS | EF_HALT_AT_EDGE | EF_EXPLODES;

	superTick = u->tick;
	superTouch = u->touch;

	u->walk = lookForPlayer;
	u->action = lookForPlayer;
	u->tick = tick;
	u->touch = touch;
	u->die = die;
}

static void tick(void)
{
	Unit *u;

	u = (Unit*)self;

	superTick();

	if (u->alive == ALIVE_DYING)
	{
		addSmokeParticles(u->x + (u->w / 2), u->y);
	}
}

static void touch(Entity *other)
{
	Unit *u;
	int mx, my;

	u = (Unit*)self;

	superTouch(other);

	if (u->alive == ALIVE_DYING && (other == NULL || other->isSolid))
	{
		dropCarriedItem();

		u->alive = ALIVE_DEAD;

		addExplosion(u->x, u->y, 80, self);

		throwDebris(u->x + u->w / 2, u->y + u->h / 2, rrnd(3, 5));

		if (rand() % 100 < 30)
		{
			addRandomWeapon(u->x, u->y);
		}

		addRandomItems((int) u->x, (int) u->y);

		updateObjective(u->name);
		updateObjective("ENEMY");
		fireTriggers(u->name);

		if (u->isMissionTarget)
		{
			addDefeatedTarget(u->name);
		}

		game.enemiesKilled++;

		mx = (int) ((u->x + (u->w / 2)) / MAP_TILE_SIZE);
		my = (int) ((u->y + u->h) / MAP_TILE_SIZE) + 1;
		addScorchDecal(mx, my);
	}
}


static void unitDie(void)
{
	if (self->environment != ENV_AIR)
	{
		touch(NULL);
	}
}

static void die(void)
{
	Unit *u;

	u = (Unit*)self;

	u->dx = (randF() - randF()) * 3;

	u->facing = FACING_DIE;
	u->spriteTime = 0;
	u->spriteFrame = 0;

	u->action = unitDie;
	u->thinkTime = 0;
	u->flags |= EF_ALWAYS_PROCESS;
	u->flags &= ~(EF_WEIGHTLESS | EF_HALT_AT_EDGE);

	u->dy = JUMP_POWER;

	if (rand() % 2)
	{
		playSound(SND_DROID_DIE_1, CH_DEATH);
	}
	else
	{
		playSound(SND_DROID_DIE_2, CH_DEATH);
	}
}

static void chase(void)
{
	Unit *u;

	u = (Unit*)self;

	u->dx = world.bob->x - u->x;
	u->dy = world.bob->y - u->y;

	u->dx = limit(u->dx, -1, 1);
	u->dy = limit(u->dy, -1, 1);

	u->thinkTime = (int) rrnd(FPS / 2, FPS);

	u->facing = (u->dx >= 0) ? FACING_RIGHT : FACING_LEFT;
}

static void patrol(void)
{
	Unit *u;

	u = (Unit*)self;

	switch (rand() % 3)
	{
		case 0:
			u->dx = 0;
			u->facing = rand() % 2 ? FACING_LEFT : FACING_RIGHT;
			u->thinkTime = rrnd(FPS / 2, FPS);
			break;

		case 1:
			u->dx = rrnd(-100, 100);
			u->dy = rrnd(-100, 100);
			u->dx *= 0.01;
			u->dy *= 0.01;
			u->thinkTime = rrnd(FPS / 2, FPS);
			u->facing = (u->dx >= 0) ? FACING_RIGHT : FACING_LEFT;
			break;

		case 2:
			u->dx = 0;
			u->thinkTime = rrnd(FPS / 2, FPS);
			break;
	}
}

static void lookForPlayer(void)
{
	Unit *u;
	double distance, r;

	u = (Unit*)self;

	u->dx *= 0.5;
	u->dy *= 0.5;

	u->thinkTime = rrnd(FPS / 2, FPS);

	if (world.state != WS_IN_PROGRESS || dev.cheatBlind)
	{
		patrol();
		return;
	}

	distance = getDistance(world.bob->x, world.bob->y, u->x, u->y);

	if (distance > 650)
	{
		patrol();
		return;
	}

	if (!enemyCanSeePlayer(self))
	{
		patrol();
		return;
	}

	r = randF();
	if (u->isMissionTarget)
	{
		r = randF() * 0.3;
	}

	if (r < 0.125)
	{
		chase();
		u->shotsToFire = rrnd(1, u->maxShotsToFire);
		u->action = u->preFire;
	}
	else if (r < 0.25)
	{
		u->dx = 0;
		u->shotsToFire = rrnd(1, u->maxShotsToFire);
		u->action = u->preFire;
	}
	else if (r < 0.5)
	{
		u->dx = 0;
	}
	else
	{
		chase();
	}

	u->thinkTime = rrnd(FPS / 2, FPS);
}
