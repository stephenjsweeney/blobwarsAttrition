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

#include "evilBlob.h"

static void (*superAnimate)(void);
static void die(void);
static void die2(void);
static void returnToStart(void);
static void lookForPlayer(void);
static void animate(void);

Unit *createEvilBlob(void)
{
	Unit *u;
	
	u = createUnit();

	u->reload = 0;

	u->returnToStartTimer = 0;

	u->flags |= EF_HALT_AT_EDGE;
	
	superAnimate = u->animate;

	u->action = lookForPlayer;
	u->walk = lookForPlayer;
	u->animate = animate;
	u->die = die;

	return u;
}

static void die2(void)
{
	Unit *u;
	int mx, my;

	u = (Unit*)self;

	u->health--;

	/* burst on impact, due to speed */
	if (u->dy > 15)
	{
		u->health = -FPS;
	}

	if (u->health <= -FPS && (u->isOnGround || u->environment != ENV_AIR))
	{
		dropCarriedItem();

		u->alive = ALIVE_DEAD;

		throwFleshChunks(u->x + u->w / 2, u->y + u->h / 2, rrnd(3, 6));

		if (rand() % 100 < 35)
		{
			addRandomWeapon(u->x, u->y);
		}

		addRandomItems(u->x, u->y);

		updateObjective(u->name);
		updateObjective("ENEMY");
		fireTriggers(u->name);

		if (u->isMissionTarget)
		{
			game.stats[STAT_TARGETS_DEFEATED]++;
		}

		game.stats[STAT_ENEMIES_KILLED]++;

		mx = (int) ((u->x + (u->w / 2)) / MAP_TILE_SIZE);
		my = (int) (u->y / MAP_TILE_SIZE) + 1;
		addBloodDecal(mx, my);

		playBattleSound(SND_SPLAT, u->uniqueId % MAX_SND_CHANNELS, u->x, u->y);
	}
}

static void moveTowardsPlayer(void)
{
	Unit *u;

	u = (Unit*)self;

	if (world.bob->x < u->x)
	{
		u->dx = -2;
		u->facing = FACING_LEFT;
	}
	else if (world.bob->x > u->x)
	{
		u->dx = 2;
		u->facing = FACING_RIGHT;
	}

	if (u->flags & EF_SWIMS)
	{
		u->dx = limit(u->dx, -1, 1);

		if (world.bob->y < u->y)
		{
			u->dy = -1;
		}
		else
		{
			u->dy = 1;
		}
	}
}

static void patrol(void)
{
	Unit *u;

	u = (Unit*)self;

	if (u->returnToStartTimer > 0)
	{
		returnToStart();
		return;
	}

	if (!u->spawnedIn && rand() % 100 < 25)
	{
		u->returnToStartTimer = FPS * 3;
	}

	u->facing = rand() % 100 < 50 ? FACING_LEFT : FACING_RIGHT;
	u->dx = rrnd(-1, 1);
	u->thinkTime = rrnd(FPS / 2, FPS);

	if (u->dx == -1)
	{
		u->facing = FACING_LEFT;
	}
	else if (u->dx == 1)
	{
		u->facing = FACING_RIGHT;
	}

	if (u->flags & EF_SWIMS)
	{
		u->dy = rrnd(-1, 1);
	}
}

static void returnToStart(void)
{
	Unit *u;

	u = (Unit*)self;

	u->returnToStartTimer--;

	u->thinkTime = 10;

	/* jump up if your start is above you */
	if (u->startY - u->y < -64 && u->isOnGround && u->dx == 0)
	{
		u->dy = JUMP_POWER;
		u->isOnGround = 0;
	}

	u->facing = u->startX < u->x ? FACING_LEFT : FACING_RIGHT;
	u->dx = u->facing == FACING_LEFT ? -1 : 1;
	if (!u->isOnGround)
	{
		u->thinkTime = 0;
		u->returnToStartTimer = MAX(u->returnToStartTimer, FPS);
		u->dx *= 2;
	}

	if (u->flags & EF_SWIMS)
	{
		u->dy = u->startY < u->y ? -1 : -1;
	}

	if (abs(u->startX - u->x) <= MAP_TILE_SIZE)
	{
		u->returnToStartTimer = 0;
	}
}

static void lookForPlayer(void)
{
	Unit *u;
	float r;

	u = (Unit*)self;

	u->thinkTime = (int) rrnd(FPS / 2, FPS);

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
	
	r = randF();
	
	if (world.missionType == MT_OUTPOST)
	{
		r = randF() * 0.65;
	}
	
	if (u->isMissionTarget)
	{
		r = randF() * 0.3;
	}

	if (r < 0.065)
	{
		moveTowardsPlayer();
		u->shotsToFire = rrnd(1, u->maxShotsToFire);
		u->action = u->preFire;
	}
	else if (r < 0.12)
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
		moveTowardsPlayer();
	}
}

static void die(void)
{
	Unit *u;

	u = (Unit*)self;

	u->flags |= EF_BOUNCES | EF_ALWAYS_PROCESS;
	u->flags &= ~(EF_HALT_AT_EDGE | EF_GONE);

	u->action = die2;
	
	u->facing = FACING_DIE;
	u->thinkTime = 0;
	u->spriteTime = 0;
	u->spriteFrame = 0;

	if (u->environment == ENV_AIR)
	{
		u->dy = -9;
	}

	u->dx = (randF() - randF()) * 5;

	switch (rand() % 3)
	{
		case 0:
			playBattleSound(SND_DEATH_1, u->uniqueId % MAX_SND_CHANNELS, u->x, u->y);
			break;

		case 1:
			playBattleSound(SND_DEATH_2, u->uniqueId % MAX_SND_CHANNELS, u->x, u->y);
			break;

		case 2:
			playBattleSound(SND_DEATH_3, u->uniqueId % MAX_SND_CHANNELS, u->x, u->y);
			break;
	}
}

static void animate(void)
{
	if (self->alive != ALIVE_ALIVE)
	{
		self->facing = FACING_DIE;
		
		superAnimate();	
	}
	else if (self->dx != 0)
	{
		superAnimate();
	}
}
