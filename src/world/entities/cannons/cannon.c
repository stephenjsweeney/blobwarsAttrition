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
static SDL_Rect getBounds(void);
static int canFire(Entity *target);
static void preFire(void);

void initCannon(Entity *e)
{
	initUnit(e);

	e->sprite[FACING_LEFT] = getSpriteIndex("CannonLeft");
	e->sprite[FACING_RIGHT] = getSpriteIndex("CannonRight");
	e->sprite[FACING_DIE] = getSpriteIndex("CannonLeft");

	e->weaponType = WPN_MISSILE;

	e->maxShotsToFire = 4;

	e->reload = 0;

	e->canCarryItem = 1;

	e->health = e->healthMax = 50;

	e->flags |= EF_EXPLODES;

	e->animate = animate;
	e->applyDamage = applyDamage;
	e->walk = walk;
	e->die = die;
	e->getBounds= getBounds;
	e->canFire = canFire;
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
	int mx, my;

	self->health--;

	if (self->health % 3 == 0)
	{
		mx = (int) ((self->x + (self->w / 2)) / MAP_TILE_SIZE);
		my = (int) ((self->y + self->h) / MAP_TILE_SIZE);
		addScorchDecal(mx, my);

		addExplosion(self->x, self->y, 50, self);
	}

	if (self->health <= -50)
	{
		updateObjective(self->name);
		updateObjective("ENEMY");
		fireTriggers(self->name);

		dropCarriedItem();

		self->alive = ALIVE_DEAD;
	}
}

static void patrol(void)
{
	self->facing = rand() % 2 ? FACING_LEFT : FACING_RIGHT;

	self->thinkTime = rrnd(FPS / 2, FPS);
}

static void lookForPlayer(void)
{
	int r;

	self->thinkTime = rrnd(FPS / 2, FPS);

	if (world.state != WS_IN_PROGRESS || dev.cheatBlind)
	{
		patrol();
		return;
	}

	if ((self->facing == FACING_LEFT && world.bob->x > self->x) || (self->facing == FACING_RIGHT && world.bob->x < self->x))
	{
		patrol();
		return;
	}

	if (getDistance(world.bob->x, world.bob->y, self->x, self->y) > 650)
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

	if (self->isMissionTarget)
	{
		r = rand() % 20;
	}

	if (r < 15)
	{
		self->shotsToFire = rrnd(1, self->maxShotsToFire);
		self->action = preFire;
	}
}

static void preFire(void)
{
	self->facing = (world.bob->x < self->x) ? FACING_LEFT : FACING_RIGHT;

	if (self->reload > 0)
	{
		return;
	}

	self->attack();

	self->shotsToFire--;

	if (self->shotsToFire == 0)
	{
		self->walk();
	}
}

static void walk(void)
{
	self->action = lookForPlayer;
}

static void animate(void)
{

}

static SDL_Rect getBounds(void)
{
	self->bounds.x = self->x + 36;
	self->bounds.y = self->y;
	self->bounds.w = 36;
	self->bounds.h = self->h;

	return self->bounds;
}

static int canFire(Entity *target)
{
	return abs(target->y - self->y) <= MAP_TILE_SIZE;
}
