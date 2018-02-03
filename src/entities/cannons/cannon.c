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

#include "cannon.h"

static void applyDamage(int damage);
static void walk(void);
static void die(void);
static void animate(void);
static int canFire(Entity *target);
static void preFire(void);

void initCannon(void)
{
	Unit *u;
	
	u = createUnit();
	
	u->type = ET_ENEMY;

	u->sprite[FACING_LEFT] = getSprite("CannonLeft");
	u->sprite[FACING_RIGHT] = getSprite("CannonRight");
	u->sprite[FACING_DIE] = getSprite("CannonLeft");

	u->weaponType = WPN_MISSILE;

	u->maxShotsToFire = 4;

	u->reload = 0;

	u->canCarryItem = 1;

	u->health = u->healthMax = 50;

	u->flags |= EF_EXPLODES;

	u->animate = animate;
	u->applyDamage = applyDamage;
	u->walk = walk;
	u->die = die;
	u->canFire = canFire;
}

static void applyDamage(int damage)
{
	if (self->health > 0)
	{
		self->health -= damage;

		self->facing = self->x < world.bob->x ? FACING_RIGHT : FACING_LEFT;

		self->thinkTime = 0;
	}
}

static void die(void)
{
	Unit *u;
	int mx, my;
	
	u = (Unit*)self;

	u->health--;

	if (u->health % 3 == 0)
	{
		mx = (int) ((u->x + (u->w / 2)) / MAP_TILE_SIZE);
		my = (int) ((u->y + u->h) / MAP_TILE_SIZE);
		addScorchDecal(mx, my);

		addExplosion(u->x, u->y, 50, self);
	}

	if (u->health <= -50)
	{
		updateObjective(u->name);
		updateObjective("ENEMY");
		fireTriggers(u->name);

		dropCarriedItem();

		u->alive = ALIVE_DEAD;
	}
}

static void patrol(void)
{
	self->facing = rand() % 2 ? FACING_LEFT : FACING_RIGHT;

	self->thinkTime = rrnd(FPS / 2, FPS);
}

static void lookForPlayer(void)
{
	Unit *u;
	int r;
	
	u = (Unit*)self;

	u->thinkTime = rrnd(FPS / 2, FPS);

	if (world.state != WS_IN_PROGRESS || dev.cheatBlind)
	{
		patrol();
		return;
	}

	if ((u->facing == FACING_LEFT && world.bob->x > u->x) || (u->facing == FACING_RIGHT && world.bob->x < u->x))
	{
		patrol();
		return;
	}

	if (getDistance(world.bob->x, world.bob->y, u->x, u->y) > 650)
	{
		patrol();
		return;
	}

	if (!enemyCanSeePlayer(self))
	{
		patrol();
		return;
	}

	r = rand() % 100;

	if (u->isMissionTarget)
	{
		r = rand() % 20;
	}

	if (r < 15)
	{
		u->shotsToFire = rrnd(1, u->maxShotsToFire);
		u->action = preFire;
	}
}

static void preFire(void)
{
	Unit *u;
	
	u = (Unit*)self;
	
	u->facing = (world.bob->x < u->x) ? FACING_LEFT : FACING_RIGHT;

	if (u->reload > 0)
	{
		return;
	}

	u->attack();

	u->shotsToFire--;

	if (u->shotsToFire == 0)
	{
		u->walk();
	}
}

static void walk(void)
{
	self->action = lookForPlayer;
}

static void animate(void)
{

}

static int canFire(Entity *target)
{
	return abs(target->y - self->y) <= MAP_TILE_SIZE;
}
