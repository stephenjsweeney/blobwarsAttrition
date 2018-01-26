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

#include "blobBoss.h"

static void activate(int activate);
static void walk(void);
static void tick(void);
static void changeEnvironment(void);
static int getCurrentSprite(void);
static void animate(void);
static void applyDamage(int amount);
static void moveTowardsPlayer(void);
static void preFire(void);
static void teleport(void);
static void attack(void);
static void die1(void);
static void die2(void);

static int aimedSprite;

void initBlobBoss(Entity *e)
{
	initBoss(e);
	
	aimedSprite = getSpriteIndex("AimedShot");
	
	e->flags |= EF_HALT_AT_EDGE;

	e->health = e->healthMax = 250;

	e->teleportTimer = FPS * rrnd(4, 6);
	
	e->activate = activate;
	e->walk = walk;
	e->tick = tick;
	e->changeEnvironment = changeEnvironment;
	e->getCurrentSprite = getCurrentSprite;
	e->animate = animate;
	e->applyDamage = applyDamage;
	e->die = die1;
}

static void activate(int activate)
{
	self->flags &= ~EF_GONE;

	world.isBossActive = 1;

	addTeleportStars(self);

	if (!isPlayingMusic())
	{
		playMusic("", 1);
	}
}

static void tick(void)
{
	if (self->health > 0)
	{
		self->stunTimer = MAX(self->stunTimer - 1, 0);

		if (self->environment == self->weakAgainst)
		{
			self->health -= 2;

			world.boss = self;
		}

		if (self->stunTimer == 0)
		{
			self->facing = (world.bob->x < self->x) ? FACING_LEFT : FACING_RIGHT;

			self->reload = limit(--self->reload, 0, FPS);

			self->teleportTimer = limit(self->teleportTimer - 1, 0, 9999);

			if (self->isOnGround)
			{
				self->flags |= EF_HALT_AT_EDGE;
			}
			else
			{
				self->flags &= ~EF_HALT_AT_EDGE;
			}
		}
	}
}

static void changeEnvironment()
{
	if (self->environment == self->weakAgainst)
	{
		self->teleportTimer = 0;
		self->stunTimer = 90;
		self->spriteFrame = self->spriteTime = 0;
	}
	else
	{
		self->teleportTimer = 0;
	}
}

static void die1(void)
{
	self->flags |= EF_BOUNCES;

	self->thinkTime = 0;

	self->spriteTime = 0;
	self->spriteFrame = 0;

	if (self->environment == ENV_AIR)
	{
		self->dy = -9;
	}

	self->dx = (randF() - randF()) * 5;

	self->flags &= ~EF_HALT_AT_EDGE;

	switch (rand() % 3)
	{
		case 0:
			playSound(SND_DEATH_1, CH_DEATH);
			break;

		case 1:
			playSound(SND_DEATH_2, CH_DEATH);
			break;

		case 2:
			playSound(SND_DEATH_3, CH_DEATH);
			break;
	}
	
	self->action = die2;
}

static int getCurrentSprite(void)
{
	if (self->stunTimer > 0 || self->health <= 0)
	{
		return self->sprite[FACING_DIE];
	}

	return self->sprite[self->facing];
}

static void animate(void)
{
	if (self->dx != 0 || self->health <= 0 || self->stunTimer > 0)
	{
		animateEntity(self);
	}
}

static void lookForPlayer(void)
{
	self->thinkTime = rrnd(0, FPS / 2);

	if (getDistance(world.bob->x, world.bob->y, self->x, self->y) > 650)
	{
		moveTowardsPlayer();
		return;
	}

	if (!enemyCanSeePlayer(self))
	{
		moveTowardsPlayer();
		return;
	}

	if (rand() % 100 < 15)
	{
		self->shotsToFire = rrnd(1, 12);
		self->action = preFire;
	}

	moveTowardsPlayer();
}

static void moveTowardsPlayer(void)
{
	self->dx = 0;

	if (rand() % 100 < 20)
	{
		if (world.bob->x < self->x)
		{
			self->dx = -3.5;
		}

		if (world.bob->x > self->x)
		{
			self->dx = 3.5;
		}

		if (world.bob->y <= self->y && rand() % 2 == 0 && self->isOnGround)
		{
			self->dy = JUMP_POWER;
		}
	}

	if (self->stunTimer == 0 && self->teleportTimer == 0)
	{
		teleport();

		self->teleportTimer = FPS * rrnd(4, 6);
	}
}

static void preFire(void)
{
	if (self->reload > 0)
	{
		return;
	}

	attack();

	self->shotsToFire--;

	if (self->shotsToFire == 0)
	{
		self->walk();
	}
}

static void attack(void)
{
	Entity *bullet;
	float dx, dy;
	int bx, by;
	
	bx = (int) (world.bob->x + rrnd(-8, 24));
	by = (int) (world.bob->y + rrnd(-8, 24));

	getSlope(bx, by, self->x, self->y, &dx, &dy);

	bullet = createBaseBullet(self);
	bullet->x = self->x;
	bullet->y = (self->y + self->h / 2) - 3;
	bullet->facing = self->facing;
	bullet->damage = 1;
	bullet->owner = self;
	bullet->health = FPS * 3;
	bullet->weaponType = WPN_AIMED_PISTOL;
	bullet->dx = dx * 12;
	bullet->dy = dy * 12;
	bullet->sprite[0] = bullet->sprite[1] = aimedSprite;

	self->reload = 4;

	playSound(SND_MACHINE_GUN, CH_WEAPON);
}

static void walk(void)
{
	self->action = (self->health > 0) ? lookForPlayer : die2;
}

void reappear(void)
{
	int valid, r;
	
	valid = 0;
	
	do
	{
		r = (int) (rand() %  MAX_CHECKPOINTS);
		self->x = world.checkpoints[r].x;
		self->y = world.checkpoints[r].y;
		valid = (self->x != 0 && self->y != 0);
	}
	while (!valid);
	
	self->y -= (self->h + 10);
	
	walk();
	
	self->flags &= ~EF_GONE;
	
	addTeleportStars(self);
	
	playSound(SND_APPEAR, CH_ANY);
}

static void applyDamage(int amount)
{
	self->health -= amount;

	self->thinkTime = 0;

	if (rand() % 10 == 0)
	{
		teleport();
	}

	world.boss = self;
}

static void teleport(void)
{
	self->action = reappear;
	self->flags |= EF_GONE;
	self->thinkTime = FPS * rrnd(3, 6);
	addTeleportStars(self);
	playSound(SND_APPEAR, CH_ANY);
}

static void die2(void)
{
	self->health--;

	if (self->health <= -FPS)
	{
		addTeleportStars(self);

		playSound(SND_APPEAR, CH_ANY);

		self->alive = ALIVE_DEAD;

		updateObjective(self->name);

		addDefeatedTarget(self->name);

		game.enemiesKilled++;
	}
}
