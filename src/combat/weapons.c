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

#include "weapons.h"

Bullet *createBaseBullet(Unit *owner);
static void tick(void);
static void touch(Entity *other);

static Sprite *bulletSprite[2];
static Sprite *plasmaSprite[2];
static Sprite *aimedSprite;
static Sprite *spreadShotSprite;
static Sprite *alienSpreadShotSprite;
static Sprite *laserSprite[2];
static Sprite *grenadeSprite;
static Sprite *alienGrenadeSprite;
static Sprite *shotgunPelletSprite;
static Sprite *missileSprite[2];

static char *weaponName[WPN_ANY];

void initWeapons(void)
{
	bulletSprite[0] = getSprite("BulletRight");
	bulletSprite[1] = getSprite("BulletLeft");

	plasmaSprite[0] = getSprite("PlasmaRight");
	plasmaSprite[1] = getSprite("PlasmaLeft");

	aimedSprite = getSprite("AimedShot");

	spreadShotSprite = getSprite("SpreadShot");
	alienSpreadShotSprite = getSprite("AlienSpreadShot");

	laserSprite[0] = getSprite("Laser");
	laserSprite[1] = getSprite("AlienLaser");

	grenadeSprite = getSprite("Grenade");
	alienGrenadeSprite = getSprite("AlienGrenade");

	shotgunPelletSprite = getSprite("ShotgunPellet");

	missileSprite[0] = getSprite("MissileRight");
	missileSprite[1] = getSprite("MissileLeft");
	
	weaponName[WPN_PISTOL] = _("Pistol");
	weaponName[WPN_PLASMA] = _("Plasma Rifle");
	weaponName[WPN_SPREAD] = _("Spread Gun");
	weaponName[WPN_LASER] = _("Laser Cannon");
	weaponName[WPN_GRENADES] = _("Grenades");
}

/* only used by Bob */
void firePistol(void)
{
	Bullet *bullet;
	
	bullet = createBaseBullet((Unit*)world.bob);
	bullet->weaponType = WPN_PISTOL;
	bullet->facing = world.bob->facing;
	bullet->sprite[0] = bulletSprite[0];
	bullet->sprite[1] = bulletSprite[1];

	world.bob->reload = 20;

	playSound(SND_PISTOL, CH_BOB);
}

void fireAimedShot(Unit *owner)
{
	int x, y;
	float dx, dy;
	Bullet *bullet;
	
	x = (int) (world.bob->x + rrnd(-8, 24));
	y = (int) (world.bob->y + rrnd(-8, 24));

	getSlope(x, y, owner->x, owner->y, &dx, &dy);

	bullet = createBaseBullet(owner);
	bullet->weaponType = WPN_AIMED_PISTOL;
	bullet->dx = dx * 6;
	bullet->dy = dy * 6;
	bullet->sprite[0] = bullet->sprite[1] = aimedSprite;
	bullet->health *= 2;

	owner->reload = 15;

	playSound(SND_PISTOL, CH_WEAPON);
}

void fireMachineGun(Unit *owner)
{
	Bullet *bullet;
	
	bullet = createBaseBullet(owner);
	bullet->weaponType = WPN_MACHINE_GUN;
	bullet->sprite[0] = bulletSprite[0];
	bullet->sprite[1] = bulletSprite[1];
	owner->reload = 8;

	playSound(SND_MACHINE_GUN, CH_WEAPON);
}

void firePlasma(Unit *owner)
{
	Bullet *bullet;
	
	bullet = createBaseBullet(owner);
	bullet->weaponType = WPN_PLASMA;
	bullet->sprite[0] = plasmaSprite[0];
	bullet->sprite[1] = plasmaSprite[1];
	bullet->damage = 2;

	owner->reload = owner->type == ET_BOB ? 4 : 8;

	playSound(SND_PLASMA, owner->type == ET_BOB ? CH_BOB : CH_WEAPON);
}

void fireSpread(Unit *owner, int numberOfShots)
{
	Bullet *bullet;
	int i;
	float dy;

	dy = -(numberOfShots / 2) * 3;

	for (i = 0 ; i < numberOfShots ; i++)
	{
		bullet = createBaseBullet(owner);
		bullet->weaponType = WPN_SPREAD;
		bullet->sprite[0] = bullet->sprite[1] = owner->type == ET_BOB ? spreadShotSprite : alienSpreadShotSprite;
		bullet->dx = owner->facing == FACING_RIGHT ? 15 : -15;
		bullet->dy = dy;

		dy += 3;

		owner->reload = 16;
	}

	playSound(SND_SPREAD, owner->type == ET_BOB ? CH_BOB : CH_WEAPON);
}

void fireLaser(Unit *owner)
{
	Bullet *laser;
	
	laser = createBaseBullet(owner);
	laser->x = owner->x + owner->w / 2;
	laser->y = owner->y + owner->h / 2;
	laser->facing = owner->facing;
	laser->dx = owner->facing == FACING_RIGHT ? 20 : -20;
	laser->health = FPS * 3;
	laser->sprite[0] = laser->sprite[1] = (owner->type == ET_BOB) ? laserSprite[0] : laserSprite[1];

	initLaser(laser);

	owner->reload = owner->type == ET_BOB ? FPS / 2 : FPS;

	playSound(SND_LASER, owner->type == ET_BOB ? CH_BOB : CH_WEAPON);
}

void fireGrenade(Unit *owner)
{
	Bullet *grenade;
	
	grenade = createBaseBullet(owner);
	grenade->x = owner->x + owner->w / 2;
	grenade->y = owner->y;
	grenade->facing = owner->facing;
	grenade->health = FPS * 3;
	grenade->dx = owner->facing == FACING_RIGHT ? 8 : -8;
	grenade->sprite[0] = grenade->sprite[1] = (owner->type == ET_BOB) ? grenadeSprite : alienGrenadeSprite;
	grenade->dy = -6;

	initGrenade(grenade);

	owner->reload = FPS / 2;

	playSound(SND_THROW, owner->type == ET_BOB ? CH_BOB : CH_WEAPON);
}

void fireShotgun(Unit *owner)
{
	int i;
	float dx, dy;
	Bullet *bullet;
	
	for (i = 0 ; i < 8 ; i++)
	{
		getSlope(world.bob->x + rrnd(0, 40), world.bob->y + rrnd(0, 40), owner->x, owner->y, &dx, &dy);

		bullet = createBaseBullet(owner);
		bullet->weaponType = WPN_SHOTGUN;
		bullet->x = owner->x + (owner->w / 2) + rrnd(-8, 8);
		bullet->y = owner->y + (owner->h / 2) + rrnd(-8, 8);
		bullet->dx = dx * 12;
		bullet->dy = dy * 12;
		bullet->damage = 2;
		bullet->sprite[0] = bullet->sprite[1] = shotgunPelletSprite;

		owner->reload = 15;
	}

	playSound(SND_SHOTGUN, CH_WEAPON);
}

void fireMissile(Unit *owner)
{
	Bullet *missile;
	
	missile = createBaseBullet(owner);
	missile->x = owner->x + owner->w / 2;
	missile->y = owner->y + 10;
	missile->facing = owner->facing;
	missile->dx = owner->facing == FACING_RIGHT ? 10 : -10;
	missile->health = FPS * 3;
	missile->sprite[0] = missileSprite[0];
	missile->sprite[1] = missileSprite[1];

	initMissile(missile);

	owner->reload = FPS / 2;

	playSound(SND_MISSILE, CH_WEAPON);
}

Bullet *createBaseBullet(Unit *owner)
{
	Bullet *bullet;
	
	bullet = malloc(sizeof(Bullet));
	memset(bullet, 0, sizeof(Bullet));

	initEntity((Entity*)bullet);
	
	bullet->x = (owner->x + owner->w / 2);
	bullet->y = (owner->y + owner->h / 2) - 3;
	bullet->dx = owner->facing == FACING_RIGHT ? 15 : -15;
	bullet->facing = owner->facing;
	bullet->damage = 1;
	bullet->owner = (Entity*)owner;
	bullet->health = FPS * 3;
	bullet->flags |= EF_WEIGHTLESS | EF_IGNORE_BULLETS | EF_NO_ENVIRONMENT | EF_KILL_OFFSCREEN | EF_NO_TELEPORT;

	bullet->tick = tick;
	bullet->touch = touch;

	return bullet;
}

static void tick(void)
{
	Bullet *b;

	b = (Bullet*)self;

	b->health--;

	if (b->x <= 0 || b->y <= 0 || b->x >= (MAP_WIDTH * MAP_TILE_SIZE) - b->w || b->y >= (MAP_HEIGHT * MAP_TILE_SIZE) - b->h)
	{
		b->alive = ALIVE_DEAD;
	}

	// don't allow the player to kill everything on the map by firing
	// constantly
	if (b->owner->type == ET_BOB)
	{
		if (b->x < camera.x || b->y < camera.y || b->x > camera.x + SCREEN_WIDTH || b->y > camera.y + SCREEN_HEIGHT)
		{
			b->alive = ALIVE_DEAD;
		}
	}
}

static void touch(Entity *other)
{
	Bullet *b;

	b = (Bullet*)self;

	if (b->alive == ALIVE_ALIVE)
	{
		if (other == NULL)
		{
			addSparkParticles(b->x, b->y);

			b->alive = ALIVE_DEAD;

			if (rand() % 2)
			{
				playSound(SND_RICO_1, CH_ANY);
			}
			else
			{
				playSound(SND_RICO_2, CH_ANY);
			}
		}
		else if (other != b->owner && (!(other->flags & EF_IGNORE_BULLETS)) && b->owner->type != other->type)
		{
			other->applyDamage(b->damage);

			if (other->flags & EF_EXPLODES)
			{
				playSound(SND_METAL_HIT, CH_ANY);

				addSparkParticles(b->x, b->y);
			}
			else
			{
				playSound(SND_FLESH_HIT, CH_ANY);

				addSmallFleshChunk(b->x, b->y);
			}

			b->alive = ALIVE_DEAD;

			if (b->owner->type == world.bob->type)
			{
				game.statShotsHit[b->weaponType]++;
			}
		}
	}
}

int getRandomPlayerWeapon(int excludeGrenades)
{
	if (excludeGrenades)
	{
		return rand() % WPN_GRENADES;
	}

	return rand() % WPN_ANY;
}

const char *getWeaponName(int i)
{
	return weaponName[i];
}
