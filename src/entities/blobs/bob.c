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

#include "bob.h"

static SDL_Rect *getCurrentSprite(void);
static void (*superAnimate)(void);
static void animate(void);
static void init(void);
static void tick(void);
static void doAlive(void);
static void doDying(void);
static void doBobInWater(void);
static void doBobInAir(void);
static void handeImmunity(void);
static void changeSprite(Sprite **sprite);
static void load(cJSON *root);
static void save(cJSON *root);
static int completelyOnGround(void);
void resetAtCheckpoint(void);
void terminateJetpack(void);
static void bobFly(void);
static void bobWalk(void);
static void bobSwim(void);
static void fireWeapon(void);
static void activate(int activate);
static void changeEnvironment(void);

static Sprite *walkSprite[3];
static Sprite *swimSprite[3];
static Sprite *flySprite[3];
static int checkpointTimer;

Entity *initBob(void)
{
	Bob *b;
	
	b = malloc(sizeof(Bob));
	memset(b, 0, sizeof(Bob));
	
	initEntity((Entity*)b);
	
	b->type = ET_BOB;

	walkSprite[FACING_LEFT] = getSprite("BobLeft");
	walkSprite[FACING_RIGHT] = getSprite("BobRight");
	walkSprite[FACING_DIE] = getSprite("BobSpin");

	swimSprite[FACING_LEFT] = getSprite("BobAquaLeft");
	swimSprite[FACING_RIGHT] = getSprite("BobAquaRight");
	swimSprite[FACING_DIE] = getSprite("BobAquaSpin");

	flySprite[FACING_LEFT] = getSprite("BobJPLeft");
	flySprite[FACING_RIGHT] = getSprite("BobJPRight");
	flySprite[FACING_DIE] = getSprite("BobSpin");

	b->health = b->healthMax = game.hearts;
	b->power = b->powerMax = game.cells;

	b->weaponType = WPN_PISTOL;
	b->reload = 0;

	b->flags |= EF_SWIMS | EF_BOMB_SHIELD;
	
	superAnimate = b->animate;
	
	b->tick = tick;
	b->init = init;
	b->getCurrentSprite = getCurrentSprite;
	b->animate = animate;
	b->activate = activate;
	b->changeEnvironment = changeEnvironment;
	b->load = load;
	b->save = save;
	
	return (Entity*)b;
}

static void init(void)
{
	changeSprite(walkSprite);
	
	world.bob->checkpoints[0].x = world.bob->x;
	world.bob->checkpoints[0].y = world.bob->y;
	
	superAnimate();
}

static void tick(void)
{
	if (world.isTrainingMission || dev.cheatHealth)
	{
		world.bob->alive = ALIVE_ALIVE;
		world.bob->health = world.bob->healthMax;
	}

	if (world.isTrainingMission)
	{
		world.bob->power = MIN(world.bob->power + 0.01, world.bob->powerMax);
	}
	else
	{
		world.bob->power = MIN(world.bob->power + 0.00065, world.bob->powerMax);
	}

	if (dev.cheatPower)
	{
		world.bob->power = world.bob->powerMax;
	}

	if (dev.cheatReload && world.bob->reload > 4)
	{
		world.bob->reload = 4;
	}

	switch (world.bob->alive)
	{
		case ALIVE_ALIVE:
			doAlive();
			break;
		case ALIVE_DYING:
			doDying();
			break;
		default:
			break;
	}
}

static void doAlive(void)
{
	handeImmunity();

	world.bob->checkpointTimer = MAX(world.bob->checkpointTimer - 1, 0);

	world.bob->reload = limit(world.bob->reload - 1, 0, FPS);

	switch (world.bob->environment)
	{
		case ENV_AIR:
			doBobInAir();
			break;

		case ENV_WATER:
			doBobInWater();
			break;

		case ENV_SLIME:
			if (world.bob->alive == ALIVE_ALIVE && world.bob->outTimer == 0)
			{
				world.bob->outTimer = FPS * 3;
				world.bob->stunTimer = 1;
				if (world.bob->flags & EF_IMMUNE)
				{
					world.bob->health--;
				}
			}
			break;

		case ENV_LAVA:
			if (world.bob->alive == ALIVE_ALIVE && world.bob->outTimer == 0)
			{
				world.bob->outTimer = FPS * 3;
				world.bob->stunTimer = 1;
				if (world.bob->flags & EF_IMMUNE)
				{
					world.bob->health -= 2;
				}
			}
			break;
	}
}

static void handeImmunity(void)
{
	int i;

	if (world.bob->outTimer > 0)
	{
		world.bob->outTimer--;
		if (world.bob->outTimer <= 0)
		{
			world.betweenTimer = FPS / 2;
			resetAtCheckpoint();
		}
	}
	else if (world.bob->stunTimer > 0)
	{
		world.bob->stunTimer--;
		if (world.bob->stunTimer <= 0)
		{
			if (!world.bob->isOnGround && world.bob->environment != ENV_WATER)
			{
				world.bob->stunTimer = 1;
			}
			else
			{
				world.bob->spriteFrame = 0;
				world.bob->spriteTime = 0;
				world.bob->dy = 0;
				world.bob->flags &= ~EF_BOUNCES;
				world.bob->flags |= EF_FLICKER;
			}
		}
	}
	else if (world.bob->immuneTimer > 0)
	{
		world.bob->immuneTimer--;
		if (world.bob->immuneTimer <= 0)
		{
			world.bob->flags &= ~(EF_FLICKER | EF_IMMUNE);
		}
	}
	else if (world.bob->checkpointTimer == 0 && world.bob->isOnGround && completelyOnGround() && world.bob->environment == ENV_AIR && world.bob->riding == NULL)
	{
		for (i = MAX_CHECKPOINTS - 1 ; i > 0 ; i--)
		{
			world.bob->checkpoints[i].x = world.bob->checkpoints[i - 1].x;
			world.bob->checkpoints[i].y = world.bob->checkpoints[i - 1].y;
		}

		world.bob->checkpoints[0].x = world.bob->x;
		world.bob->checkpoints[0].y = world.bob->y;
		checkpointTimer = FPS;
	}
}

static int completelyOnGround(void)
{
	int x1, x2, y;

	x1 = (world.bob->x / MAP_TILE_SIZE);
	x2 = ((world.bob->x + world.bob->w) / MAP_TILE_SIZE);
	y = ((world.bob->y + world.bob->h + 8) / MAP_TILE_SIZE);

	return isSolid(x1, y) && isSolid(x2, y);
}


static void activate(int activate)
{
	if (world.bob->stunTimer <= 0)
	{
		if (world.bob->flags & (EF_WATER_BREATHING | EF_WEIGHTLESS))
		{
			world.bob->flags &= ~(EF_WATER_BREATHING | EF_WEIGHTLESS);
			changeSprite(walkSprite);
		}
		else if (world.bob->power > 0)
		{
			if (world.bob->environment == ENV_AIR)
			{
				world.bob->flags |= EF_WEIGHTLESS;
				changeSprite(flySprite);
			}
			else if (world.bob->environment == ENV_WATER)
			{
				world.bob->flags |= EF_WATER_BREATHING;
				changeSprite(swimSprite);
			}

			world.bob->dy = 0;
		}
		else
		{
			if (world.bob->environment == ENV_AIR)
			{
				setGameplayMessage(MSG_STANDARD, _("Not enough power for jetpack"));
			}
			else
			{
				setGameplayMessage(MSG_STANDARD, _("Not enough power for aqualung"));
			}
		}
	}
}

static void doBobInAir(void)
{
	world.bob->oxygen = limit(world.bob->oxygen + 4, 0, MAX_OXYGEN);

	if (world.bob->flags & EF_WEIGHTLESS)
	{
		if (!dev.cheatPower)
		{
			world.bob->power = limit(world.bob->power - (1.0 / FPS), 0, world.bob->powerMax);
		}

		if (--world.bob->jpEffectTimer <= 0)
		{
			addFlameParticles(world.bob->x + (world.bob->facing * 25) + rrnd(-1, 1), world.bob->y + 25 + rrnd(-1, 1));
			world.bob->jpEffectTimer = 1;
		}

		if (world.bob->power == 0)
		{
			terminateJetpack();
		}
	}
}

void terminateJetpack(void)
{
	changeSprite(walkSprite);

	world.bob->flags &= ~EF_WEIGHTLESS;
}

static void doBobInWater(void)
{
	if (world.bob->flags & EF_WATER_BREATHING)
	{
		if (!dev.cheatPower)
		{
			world.bob->power = limit(world.bob->power - (1.0 / FPS), 0, world.bob->powerMax);
		}

		if (world.bob->power == 0)
		{
			world.bob->flags &= ~EF_WATER_BREATHING;
			changeSprite(walkSprite);
		}
	}
	else
	{
		world.bob->oxygen = limit(--world.bob->oxygen, 0, MAX_OXYGEN);

		if (world.bob->oxygen == 0 && (world.frameCounter % 30) == 0)
		{
			world.bob->health--;
			playSound(SND_DROWN, CH_BOB);
		}
	}
}

static void doDying(void)
{
	world.bob->health--;

	if (world.bob->health <= -(FPS * 2))
	{
		world.bob->alive = ALIVE_DEAD;
		
		throwFleshChunks(world.bob->x + world.bob->w / 2, world.bob->y + world.bob->h / 2, rrnd(3, 6));

		world.state = WS_GAME_OVER;

		playSound(SND_SPLAT, CH_BOB);

		game.deaths++;
	}
}

void stunBob(void)
{
	terminateJetpack();

	world.bob->stunTimer = FPS;
	world.bob->immuneTimer = FPS * 2;
	world.bob->spriteFrame = 0;
	world.bob->spriteTime = 0;
	world.bob->flags &= ~EF_FLICKER;
	world.bob->flags |= (EF_BOUNCES | EF_IMMUNE);
}


void applyDamage(int damage)
{
	if (!(world.bob->flags & EF_IMMUNE) && !world.isTrainingMission && !dev.cheatHealth && world.bob->alive != ALIVE_DEAD)
	{
		if (world.bob->health < 0)
		{
			world.bob->health = 0;
			world.bob->alive = ALIVE_ALIVE;
		}

		world.bob->health -= damage;

		if (world.bob->health > 0)
		{
			world.bob->immuneTimer = FPS * 2;

			world.bob->flags |= EF_FLICKER | EF_IMMUNE;
		}
	}
}

void doBob(void)
{
	if (world.bob->stunTimer <= 0 && world.bob->health > 0)
	{
		if (world.bob->flags & EF_WEIGHTLESS)
		{
			bobFly();
		}
		else if (world.bob->environment == ENV_AIR)
		{
			bobWalk();
		}
		else if (world.bob->environment == ENV_WATER)
		{
			bobSwim();
		}
	}
}

static void bobWalk(void)
{
	world.bob->dx = 0;

	if (game.config.control[CONTROL_LEFT])
	{
		world.bob->dx = -WALK_SPEED;
		world.bob->facing = FACING_LEFT;
	}

	if (game.config.control[CONTROL_RIGHT])
	{
		world.bob->dx = WALK_SPEED;
		world.bob->facing = FACING_RIGHT;
	}

	if (game.config.control[CONTROL_JUMP] && world.bob->isOnGround)
	{
		world.bob->dy = JUMP_POWER;
	}

	if (game.config.control[CONTROL_FIRE] && world.bob->reload == 0)
	{
		fireWeapon();
	}
}

static void bobSwim(void)
{
	world.bob->dx = 0;
	world.bob->dy = 0.5f;

	if (game.config.control[CONTROL_LEFT])
	{
		world.bob->dx = -SWIM_SPEED;
		world.bob->facing = FACING_LEFT;
	}

	if (game.config.control[CONTROL_RIGHT])
	{
		world.bob->dx = SWIM_SPEED;
		world.bob->facing = FACING_RIGHT;
	}

	if (game.config.control[CONTROL_JUMP] || game.config.control[CONTROL_UP])
	{
		world.bob->dy = -SWIM_SPEED;
	}

	if (game.config.control[CONTROL_DOWN])
	{
		world.bob->dy = SWIM_SPEED;
	}

	if (game.config.control[CONTROL_FIRE] && world.bob->reload == 0 && world.bob->weaponType == WPN_PISTOL)
	{
		firePistol();
	}
}

static void bobFly(void)
{
	if (game.config.control[CONTROL_LEFT])
	{
		world.bob->dx -= FLY_ACCEL;
		world.bob->facing = FACING_LEFT;
	}

	if (game.config.control[CONTROL_RIGHT])
	{
		world.bob->dx += FLY_ACCEL;
		world.bob->facing = FACING_RIGHT;
	}

	if (game.config.control[CONTROL_UP])
	{
		world.bob->dy -= FLY_ACCEL;
	}

	if (game.config.control[CONTROL_DOWN])
	{
		world.bob->dy += FLY_ACCEL;
	}

	if (game.config.control[CONTROL_FIRE] && world.bob->reload == 0)
	{
		fireWeapon();
	}

	world.bob->dx = MIN(FLY_SPEED, MAX(world.bob->dx, -FLY_SPEED));
	world.bob->dy = MIN(FLY_SPEED, MAX(world.bob->dy, -FLY_SPEED));
}

static void fireWeapon(void)
{
	switch (world.bob->weaponType)
	{
		case WPN_PISTOL:
			firePistol();
			break;
		case WPN_PLASMA:
			firePlasma((Entity*)world.bob);
			break;
		case WPN_SPREAD:
			fireSpread((Entity*)world.bob, 3);
			game.statShotsFired[WPN_SPREAD] += 2;
			break;
		case WPN_LASER:
			fireLaser((Entity*)world.bob);
			break;
		case WPN_GRENADES:
			fireGrenade((Entity*)world.bob);
			break;
		default:
			break;
	}

	game.statShotsFired[world.bob->weaponType]++;
}


static void changeEnvironment(void)
{
	switch (world.bob->environment)
	{
		case ENV_AIR:
		case ENV_LAVA:
		case ENV_SLIME:
			world.bob->flags &= ~EF_WATER_BREATHING;
			changeSprite(walkSprite);
			break;
		case ENV_WATER:
			terminateJetpack();
			break;
	}
}

static void changeSprite(Sprite **sprite)
{
	world.bob->sprite[0] = sprite[0];
	world.bob->sprite[1] = sprite[1];
	world.bob->sprite[2] = sprite[2];
	world.bob->spriteFrame = 0;
	world.bob->spriteTime = 0;
}

void setCheckpoint(void)
{
	world.bob->checkpoints[0].x = world.bob->x;
	world.bob->checkpoints[0].y = world.bob->y;
}

void resetAtCheckpoint(void)
{
	world.bob->x = world.bob->checkpoints[0].x;
	world.bob->y = world.bob->checkpoints[0].y;

	world.bob->outTimer = 0;
	world.bob->flags |= EF_FLICKER;
	world.bob->flags &= ~EF_WEIGHTLESS;
	changeSprite(walkSprite);
	world.bob->environment = ENV_AIR;

	if (!game.isResumingMission)
	{
		world.bob->immuneTimer = FPS * 2;
		addTeleportStars((Entity*)world.bob);
		if (world.state == WS_IN_PROGRESS)
		{
			playSound(SND_APPEAR, CH_ANY);
		}
	}
}


void die(void)
{
	world.bob->flags &= ~EF_WEIGHTLESS;

	world.bob->flags |= EF_BOUNCES;

	if (world.bob->environment == ENV_AIR)
	{
		world.bob->dy = -9;
	}

	world.bob->dx = (randF() - randF()) * 5;

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
}

static SDL_Rect *getCurrentSprite(void)
{
	if (world.bob->alive == ALIVE_ALIVE && world.bob->stunTimer <= 0)
	{
		return &world.bob->sprite[world.bob->facing]->frames[world.bob->spriteFrame]->rect;
	}

	return &world.bob->sprite[FACING_DIE]->frames[world.bob->spriteFrame]->rect;
}

static void animate(void)
{
	if (world.bob->dx != 0 || world.bob->stunTimer > 0 || world.bob->flags & EF_WEIGHTLESS || world.bob->health <= 0)
	{
		superAnimate();
	}
}

static void load(cJSON *root)
{
	world.bob->x = cJSON_GetObjectItem(root, "x")->valueint;
	world.bob->y = cJSON_GetObjectItem(root, "y")->valueint;
	world.bob->facing = lookup(cJSON_GetObjectItem(root, "facing")->valuestring);
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "type", "Bob");
	cJSON_AddNumberToObject(root, "x", world.bob->x);
	cJSON_AddNumberToObject(root, "y", world.bob->y);
	cJSON_AddStringToObject(root, "facing", getLookupName("FACING_", world.bob->facing));
}
