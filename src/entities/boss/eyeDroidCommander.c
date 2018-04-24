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

#include "eyeDroidCommander.h"

static void activate(int activate);
static void tick(void);
static void walk(void);
static void moveTowardsPlayer(int dir);
static void die(void);
static void die2(void);
static void selectWeapon(void);
static void attackPistol(void);
static void attackPlasma(void);
static void attackMissile(void);
static void preFire(void);
static void attack(void);
static void applyDamage(int amount);
static SDL_Rect *getCurrentSprite(void);

static int brakingTimer;
static Sprite *aimedSprite;
static Sprite *missileSprite[2];
static Sprite *plasmaSprite[2];

Entity *initEyeDroidCommander(void)
{
	Boss *b;
	
	b = initBoss();
	
	STRNCPY(b->name, "EyeDroid Commander", MAX_NAME_LENGTH);

	b->sprite[FACING_LEFT] = getSprite("DroidCommanderLeft");
	b->sprite[FACING_RIGHT] = getSprite("DroidCommanderRight");
	b->sprite[FACING_DIE] = getSprite("DroidCommanderDie");
	
	b->flags |= EF_WEIGHTLESS | EF_EXPLODES;

	b->health = b->healthMax = 250;
	
	b->action = walk;
	b->walk = walk;
	b->tick = tick;
	b->activate = activate;
	b->applyDamage = applyDamage;
	b->die = die;
	b->getCurrentSprite = getCurrentSprite;

	brakingTimer = 0;
	
	aimedSprite = getSprite("AimedShot");
	
	missileSprite[0] = getSprite("MissileRight");
	missileSprite[1] = getSprite("MissileLeft");
	
	plasmaSprite[0] = getSprite("PlasmaRight");
	plasmaSprite[1] = getSprite("PlasmaLeft");
	
	return (Entity*)b;
}

static void activate(int activate)
{
	self->flags &= ~EF_GONE;

	world.isBossActive = 1;

	addTeleportStars(self);

	playMusic(1);
}

static void tick(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->health > 0)
	{
		b->facing = (world.bob->x < b->x) ? FACING_LEFT : FACING_RIGHT;

		b->reload = (int) limit(b->reload - 1, 0, FPS);

		brakingTimer = (int) limit(--brakingTimer, 0, FPS);

		if (brakingTimer > 0)
		{
			b->dx *= 0.95;
			b->dy *= 0.95;
		}
	}
	else
	{
		addSmokeParticles(b->x + (b->w / 2), b->y, 0);
	}
}

static void lookForPlayer(void)
{
	float distance;
	
	self->thinkTime = rrnd(0, FPS / 2);

	if (rand() % 100 < 5)
	{
		brakingTimer = rrnd(60, 120);
	}
	
	distance = getDistance(world.bob->x, world.bob->y, self->x, self->y);

	if (distance > SCREEN_HEIGHT)
	{
		moveTowardsPlayer(1);
		return;
	}
	
	if (!enemyCanSeePlayer(self))
	{
		moveTowardsPlayer(1);
		return;
	}

	if (rand() % 100 < 15)
	{
		selectWeapon();
	}
	
	if (distance < SCREEN_HEIGHT / 4)
	{
		moveTowardsPlayer(-6);
	}
	else
	{
		moveTowardsPlayer(1);
	}
}

static void selectWeapon(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (world.bob->isOnGround || fabs(self->y - world.bob->y) > 64)
	{
		b->weaponType = WPN_AIMED_PISTOL;
		b->shotsToFire = rrnd(1, 12);
		b->action = preFire;
	}
	else if (rand() % 4 == 0)
	{
		b->weaponType = WPN_MISSILE;
		b->shotsToFire = rrnd(1, 3);
		b->action = preFire;
	}
	else
	{
		b->weaponType = WPN_PLASMA;
		b->shotsToFire = rrnd(1, 12);
		b->action = preFire;
	}
}

static void walk(void)
{
	self->action = (self->health > 0) ? lookForPlayer : die;
}

static void moveTowardsPlayer(int dir)
{
	float vel;
	
	vel = 0.5 * dir;
	
	if (brakingTimer == 0)
	{
		if (world.bob->x < self->x)
		{
			self->dx -= vel;
		}

		if (world.bob->x > self->x)
		{
			self->dx += vel;
		}

		if (world.bob->y < self->y)
		{
			self->dy -= vel;
		}

		if (world.bob->y > self->y)
		{
			self->dy += vel;
		}

		self->dx = limit(self->dx, -7.5, 7.5);
		self->dy = limit(self->dy, -7.5, 7.5);
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
	Boss *b;
	
	if (self->facing != FACING_DIE)
	{
		b = (Boss*)self;
		
		switch (b->weaponType)
		{
			case WPN_AIMED_PISTOL:
				attackPistol();
				break;
			case WPN_MISSILE:
				attackMissile();
				break;
			case WPN_PLASMA:
				attackPlasma();
				break;
			default:
				break;
		}
	}
}

static void attackPistol(void)
{
	int bx, by;
	float dx, dy;
	Bullet *bullet;
	Boss *b;
	
	b = (Boss*)self;
	
	bx = world.bob->x + rrnd(-8, 24);
	by = world.bob->y + rrnd(-8, 24);

	getSlope(bx, by, self->x, self->y, &dx, &dy);

	bullet = createBaseBullet((Unit*)self, aimedSprite->w);
	bullet->facing = self->facing;
	bullet->damage = 1;
	bullet->owner = self;
	bullet->health = FPS * 3;
	bullet->weaponType = WPN_AIMED_PISTOL;
	bullet->dx = dx * 12;
	bullet->dy = dy * 12;
	bullet->sprite[0] = bullet->sprite[1] = aimedSprite;

	b->reload = 4;

	playBattleSound(SND_MACHINE_GUN, b->uniqueId % MAX_SND_CHANNELS, self->x, self->y);
}

static void attackPlasma(void)
{
	Boss *b;
	Bullet *bullet;
	
	b = (Boss*)self;
	
	bullet = createBaseBullet((Unit*)self, plasmaSprite[0]->w);
	bullet->facing = self->facing;
	bullet->damage = 2;
	bullet->owner = self;
	bullet->health = FPS * 3;
	bullet->weaponType = WPN_AIMED_PISTOL;
	bullet->dx = self->facing == FACING_RIGHT ? 15 : -15;
	bullet->dy = 0;
	bullet->sprite[0] = plasmaSprite[0];
	bullet->sprite[1] = plasmaSprite[1];

	b->reload = 4;

	playBattleSound(SND_PLASMA, b->uniqueId % MAX_SND_CHANNELS, self->x, self->y);
}

static void attackMissile(void)
{
	Boss *b;
	Bullet *missile;
	
	b = (Boss*)self;
	
	missile = createBaseBullet((Unit*)self, missileSprite[0]->w);
	missile->facing = b->facing;
	missile->dx = b->facing == FACING_RIGHT ? 15 : -15;
	missile->owner = self;
	missile->health = FPS * 3;
	missile->sprite[0] = missileSprite[0];
	missile->sprite[1] = missileSprite[1];
	
	initMissile(missile);

	b->reload = 15;

	playBattleSound(SND_MISSILE, b->uniqueId % MAX_SND_CHANNELS, self->x, self->y);
}

static void applyDamage(int amount)
{
	self->health -= amount;

	if (self->health <= 0)
	{
		self->health = 0;
		addExplosion(self->x + self->w / 2, self->y + self->h / 2, 50, self);
	}
}

static void die(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	b->dx = (randF() - randF()) * 3;

	b->spriteTime = 0;
	b->spriteFrame = 0;

	b->action = die2;
	b->thinkTime = 0;
	b->flags &= ~(EF_WEIGHTLESS | EF_HALT_AT_EDGE);

	b->dy = JUMP_POWER;

	if (rand() % 2)
	{
		playBattleSound(SND_DROID_DIE_1, b->uniqueId % MAX_SND_CHANNELS, self->x, self->y);
	}
	else
	{
		playBattleSound(SND_DROID_DIE_2, b->uniqueId % MAX_SND_CHANNELS, self->x, self->y);
	}
}

static void die2()
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->isOnGround)
	{
		addTeleportStars(self);

		playBattleSound(SND_APPEAR, -1, self->x, self->y);

		/* don't die! */
		b->flags |= EF_GONE;

		updateObjective(b->name);

		game.stats[STAT_TARGETS_DEFEATED]++;

		awardTrophy("EYEDROID_COMMANDER");

		game.stats[STAT_ENEMIES_KILLED]++;
		
		b->action = entityIdle;
	}
}

static SDL_Rect *getCurrentSprite(void)
{
	Sprite *s;
	
	s = (self->alive == ALIVE_ALIVE) ? self->sprite[self->facing] : self->sprite[FACING_DIE];
	
	if (self->spriteFrame >= s->numFrames)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "WARNING: %s (%d) bad sprite frames - %d > %d\n", self->name, self->type, self->spriteFrame, s->numFrames);
		self->spriteFrame = 0;
	}
	
	return &s->frames[self->spriteFrame]->rect;
}
