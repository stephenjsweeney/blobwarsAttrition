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
static Sprite *getCurrentSprite(void);
static void animate(void);
static void applyDamage(int amount);
static void moveTowardsPlayer(void);
static void preFire(void);
static void teleport(void);
static void attack(void);
static void die1(void);
static void die2(void);

static Sprite *aimedSprite;

void initBlobBoss(Entity *e)
{
	Boss *b;
	
	initBoss(e);
	
	b = (Boss*)e;
	
	b->flags |= EF_HALT_AT_EDGE;

	b->health = b->healthMax = 250;

	b->teleportTimer = FPS * rrnd(4, 6);
	
	b->activate = activate;
	b->walk = walk;
	b->tick = tick;
	b->changeEnvironment = changeEnvironment;
	b->getCurrentSprite = getCurrentSprite;
	b->animate = animate;
	b->applyDamage = applyDamage;
	b->die = die1;
	
	aimedSprite = getSprite("AimedShot");
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
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->health > 0)
	{
		b->stunTimer = MAX(b->stunTimer - 1, 0);

		if (b->environment == b->weakAgainst)
		{
			b->health -= 2;

			world.boss = b;
		}

		if (b->stunTimer == 0)
		{
			b->facing = (world.bob->x < b->x) ? FACING_LEFT : FACING_RIGHT;

			b->reload = limit(--b->reload, 0, FPS);

			b->teleportTimer = limit(b->teleportTimer - 1, 0, 9999);

			if (b->isOnGround)
			{
				b->flags |= EF_HALT_AT_EDGE;
			}
			else
			{
				b->flags &= ~EF_HALT_AT_EDGE;
			}
		}
	}
}

static void changeEnvironment()
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->environment == b->weakAgainst)
	{
		b->teleportTimer = 0;
		b->stunTimer = 90;
		b->spriteFrame = b->spriteTime = 0;
	}
	else
	{
		b->teleportTimer = 0;
	}
}

static void die1(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	b->flags |= EF_BOUNCES;

	b->thinkTime = 0;

	b->spriteTime = 0;
	b->spriteFrame = 0;

	if (b->environment == ENV_AIR)
	{
		b->dy = -9;
	}

	b->dx = (randF() - randF()) * 5;

	b->flags &= ~EF_HALT_AT_EDGE;

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
	
	b->action = die2;
}

static Sprite *getCurrentSprite(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->stunTimer > 0 || b->health <= 0)
	{
		return b->sprite[FACING_DIE];
	}

	return b->sprite[b->facing];
}

static void animate(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->dx != 0 || b->health <= 0 || b->stunTimer > 0)
	{
		animateEntity(self);
	}
}

static void lookForPlayer(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	b->thinkTime = rrnd(0, FPS / 2);

	if (getDistance(world.bob->x, world.bob->y, b->x, b->y) > 650)
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
		b->shotsToFire = rrnd(1, 12);
		b->action = preFire;
	}

	moveTowardsPlayer();
}

static void moveTowardsPlayer(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	b->dx = 0;

	if (rand() % 100 < 20)
	{
		if (world.bob->x < b->x)
		{
			b->dx = -3.5;
		}

		if (world.bob->x > b->x)
		{
			b->dx = 3.5;
		}

		if (world.bob->y <= b->y && rand() % 2 == 0 && b->isOnGround)
		{
			b->dy = JUMP_POWER;
		}
	}

	if (b->stunTimer == 0 && b->teleportTimer == 0)
	{
		teleport();

		b->teleportTimer = FPS * rrnd(4, 6);
	}
}

static void preFire(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->reload > 0)
	{
		return;
	}

	attack();

	b->shotsToFire--;

	if (b->shotsToFire == 0)
	{
		b->walk();
	}
}

static void attack(void)
{
	Bullet *bullet;
	float dx, dy;
	int bx, by;
	
	bx = (int) (world.bob->x + rrnd(-8, 24));
	by = (int) (world.bob->y + rrnd(-8, 24));

	getSlope(bx, by, self->x, self->y, &dx, &dy);

	bullet = createBaseBullet((Unit*)self);
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

	((Boss*)self)->reload = 4;

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

	world.boss = (Boss*)self;
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
	Boss *b;
	
	b = (Boss*)self;
	
	b->health--;

	if (b->health <= -FPS)
	{
		addTeleportStars(self);

		playSound(SND_APPEAR, CH_ANY);

		b->alive = ALIVE_DEAD;

		updateObjective(b->name);

		addDefeatedTarget(b->name);

		game.enemiesKilled++;
	}
}
