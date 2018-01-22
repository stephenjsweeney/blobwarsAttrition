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

static Entity *createBaseBullet(Entity *owner);

static int bulletSprite[2];
static int plasmaSprite[2];
static int aimedSprite;
static int spreadShotSprite;
static int alienSpreadShotSprite;
static int laserSprite[2];
static int grenadeSprite;
static int alienGrenadeSprite;
static int shotgunPelletSprite;
static int missileSprite[2];

void initWeapons(void)
{
	bulletSprite[0] = getSpriteIndex("BulletRight");
	bulletSprite[1] = getSpriteIndex("BulletLeft");

	plasmaSprite[0] = getSpriteIndex("PlasmaRight");
	plasmaSprite[1] = getSpriteIndex("PlasmaLeft");

	aimedSprite = getSpriteIndex("AimedShot");

	spreadShotSprite = getSpriteIndex("SpreadShot");
	alienSpreadShotSprite = getSpriteIndex("AlienSpreadShot");

	laserSprite[0] = getSpriteIndex("Laser");
	laserSprite[1] = getSpriteIndex("AlienLaser");

	grenadeSprite = getSpriteIndex("Grenade");
	alienGrenadeSprite = getSpriteIndex("AlienGrenade");

	shotgunPelletSprite = getSpriteIndex("ShotgunPellet");

	missileSprite[0] = getSpriteIndex("MissileRight");
	missileSprite[1] = getSpriteIndex("MissileLeft");
}

void firePistol(Entity *owner)
{
	Entity *bullet;
	
	bullet = createBaseBullet(owner);
	bullet->weaponType = WPN_PISTOL;
	bullet->facing = owner->facing;
	bullet->sprite[0] = bulletSprite[0];
	bullet->sprite[1] = bulletSprite[1];

	owner->reload = 20;

	playSound(SND_PISTOL, CH_PLAYER);
}

void fireAimedShot(Entity *owner)
{
	int x, y;
	float dx, dy;
	Entity *bullet;
	
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

void fireMachineGun(Entity *owner)
{
	Entity *bullet;
	
	bullet = createBaseBullet(owner);
	bullet->weaponType = WPN_MACHINE_GUN;
	bullet->sprite[0] = bulletSprite[0];
	bullet->sprite[1] = bulletSprite[1];
	owner->reload = 8;

	playSound(SND_MACHINE_GUN, CH_WEAPON);
}

void firePlasma(Entity *owner)
{
	Entity *bullet;
	
	bullet = createBaseBullet(owner);
	bullet->weaponType = WPN_PLASMA;
	bullet->sprite[0] = plasmaSprite[0];
	bullet->sprite[1] = plasmaSprite[1];
	bullet->damage = 2;

	owner->reload = owner->type == ET_BOB ? 4 : 8;

	playSound(SND_PLASMA, owner->type == ET_BOB ? CH_PLAYER : CH_WEAPON);
}

void fireSpread(Entity *owner, int numberOfShots)
{
	Entity *bullet;
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

	playSound(SND_SPREAD, owner->type == ET_BOB ? CH_PLAYER : CH_WEAPON);
}

void fireLaser(Entity *owner)
{
	Entity *laser;
	
	laser = createBaseBullet(owner);
	laser->x = owner->x + owner->w / 2;
	laser->y = owner->y + owner->h / 2;
	laser->facing = owner->facing;
	laser->dx = owner->facing == FACING_RIGHT ? 20 : -20;
	laser->owner = owner;
	laser->health = FPS * 3;
	laser->sprite[0] = laser->sprite[1] = (owner->type == ET_BOB) ? laserSprite[0] : laserSprite[1];

	owner->reload = owner->type == ET_BOB ? FPS / 2 : FPS;

	playSound(SND_LASER, owner->type == ET_BOB ? CH_PLAYER : CH_WEAPON);
}

void fireGrenade(Entity *owner)
{
	Entity *grenade;
	
	grenade = createBaseBullet(owner);
	grenade->x = owner->x + owner->w / 2;
	grenade->y = owner->y;
	grenade->facing = owner->facing;
	grenade->owner = owner;
	grenade->health = FPS * 3;
	grenade->dx = owner->facing == FACING_RIGHT ? 8 : -8;
	grenade->sprite[0] = grenade->sprite[1] = (owner->type == ET_BOB) ? grenadeSprite : alienGrenadeSprite;

	grenade->dy = -6;

	owner->reload = FPS / 2;

	playSound(SND_THROW, owner->type == ET_BOB ? CH_PLAYER : CH_WEAPON);
}

void fireShotgun(Entity *owner)
{
	int i;
	float dx, dy;
	Entity *bullet;
	
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

void fireMissile(Entity *owner)
{
	Entity *missile;
	
	missile = createBaseBullet(owner);
	missile->x = owner->x + owner->w / 2;
	missile->y = owner->y + 10;
	missile->facing = owner->facing;
	missile->dx = owner->facing == FACING_RIGHT ? 10 : -10;
	missile->owner = owner;
	missile->health = FPS * 3;
	missile->sprite[0] = missileSprite[0];
	missile->sprite[1] = missileSprite[1];

	owner->reload = FPS / 2;

	playSound(SND_MISSILE, CH_WEAPON);
}

static Entity *createBaseBullet(Entity *owner)
{
	Entity *bullet;
	
	bullet = createEntity();
	bullet->x = (owner->x + owner->w / 2);
	bullet->y = (owner->y + owner->h / 2) - 3;
	bullet->dx = owner->facing == FACING_RIGHT ? 15 : -15;
	bullet->facing = owner->facing;
	bullet->damage = 1;
	bullet->owner = owner;
	bullet->health = FPS * 3;

	return bullet;
}

int getRandomPlayerWeapon(int excludeGrenades)
{
	if (excludeGrenades)
	{
		return rand() % WPN_GRENADES;
	}

	return rand() % WPN_ANY;
}
