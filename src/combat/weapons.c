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
	
	if (world.bob->facing != FACING_DIE)
	{
		bullet = createBaseBullet((Unit*)world.bob, bulletSprite[0]->frames[0]->rect.w);
		bullet->weaponType = WPN_PISTOL;
		bullet->facing = world.bob->facing;
		bullet->sprite[0] = bulletSprite[0];
		bullet->sprite[1] = bulletSprite[1];

		world.bob->reload = 20;

		playSound(SND_PISTOL, world.bob->uniqueId % MAX_SND_CHANNELS);
	}
}

void fireAimedShot(Unit *owner)
{
	int x, y;
	float dx, dy;
	Bullet *bullet;
	
	if (owner->facing != FACING_DIE)
	{
		x = (int) (world.bob->x + rrnd(-8, 24));
		y = (int) (world.bob->y + rrnd(-8, 24));

		getSlope(x, y, owner->x, owner->y, &dx, &dy);

		bullet = createBaseBullet(owner, aimedSprite->w);
		bullet->weaponType = WPN_AIMED_PISTOL;
		bullet->dx = dx * 6;
		bullet->dy = dy * 6;
		bullet->sprite[0] = bullet->sprite[1] = aimedSprite;
		bullet->health *= 2;

		owner->reload = 15;

		playBattleSound(SND_PISTOL, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
	}
}

void fireMachineGun(Unit *owner)
{
	Bullet *bullet;
	
	if (owner->facing != FACING_DIE)
	{
		bullet = createBaseBullet(owner, bulletSprite[0]->w);
		bullet->weaponType = WPN_MACHINE_GUN;
		bullet->sprite[0] = bulletSprite[0];
		bullet->sprite[1] = bulletSprite[1];
		owner->reload = 8;

		playBattleSound(SND_MACHINE_GUN, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
	}
}

void firePlasma(Unit *owner)
{
	Bullet *bullet;
	
	if (owner->facing != FACING_DIE)
	{
		bullet = createBaseBullet(owner, plasmaSprite[0]->w);
		bullet->weaponType = WPN_PLASMA;
		bullet->sprite[0] = plasmaSprite[0];
		bullet->sprite[1] = plasmaSprite[1];
		bullet->damage = 2;

		owner->reload = owner->type == ET_BOB ? 4 : 8;

		playBattleSound(SND_PLASMA, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
	}
}

void fireSpread(Unit *owner, int numberOfShots)
{
	Bullet *bullet;
	int i;
	float dy;

	if (owner->facing != FACING_DIE)
	{
		dy = -(numberOfShots / 2) * 3;

		for (i = 0 ; i < numberOfShots ; i++)
		{
			bullet = createBaseBullet(owner, spreadShotSprite->w);
			bullet->weaponType = WPN_SPREAD;
			bullet->sprite[0] = bullet->sprite[1] = owner->type == ET_BOB ? spreadShotSprite : alienSpreadShotSprite;
			bullet->dx = owner->facing == FACING_RIGHT ? 15 : -15;
			bullet->dy = dy;

			dy += 3;

			owner->reload = 16;
		}

		playBattleSound(SND_SPREAD, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
	}
}

void fireLaser(Unit *owner)
{
	Bullet *laser;
	
	if (owner->facing != FACING_DIE)
	{
		laser = createBaseBullet(owner, laserSprite[0]->w);
		laser->facing = owner->facing;
		laser->dx = owner->facing == FACING_RIGHT ? 20 : -20;
		laser->health = FPS * 3;
		laser->sprite[0] = laser->sprite[1] = (owner->type == ET_BOB) ? laserSprite[0] : laserSprite[1];

		initLaser(laser);

		owner->reload = owner->type == ET_BOB ? FPS / 2 : FPS;

		playBattleSound(SND_LASER, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
	}
}

void fireGrenade(Unit *owner)
{
	Bullet *grenade;
	
	if (owner->facing != FACING_DIE)
	{
		grenade = createBaseBullet(owner, grenadeSprite->w);
		grenade->y = owner->y;
		grenade->facing = owner->facing;
		grenade->health = FPS * 3;
		grenade->dx = owner->facing == FACING_RIGHT ? 8 : -8;
		grenade->sprite[0] = grenade->sprite[1] = (owner->type == ET_BOB) ? grenadeSprite : alienGrenadeSprite;
		grenade->dy = -6;

		initGrenade(grenade);

		owner->reload = FPS / 2;

		playBattleSound(SND_THROW, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
	}
}

void fireShotgun(Unit *owner)
{
	int i;
	float dx, dy;
	Bullet *bullet;
	
	if (owner->facing != FACING_DIE)
	{
		for (i = 0 ; i < 8 ; i++)
		{
			getSlope(world.bob->x + rrnd(0, 40), world.bob->y + rrnd(0, 40), owner->x, owner->y, &dx, &dy);

			bullet = createBaseBullet(owner, 0);
			bullet->weaponType = WPN_SHOTGUN;
			bullet->x = owner->x + (owner->w / 2) + rrnd(-8, 8);
			bullet->y = owner->y + (owner->h / 2) + rrnd(-8, 8);
			bullet->dx = dx * 12;
			bullet->dy = dy * 12;
			bullet->damage = 2;
			bullet->sprite[0] = bullet->sprite[1] = shotgunPelletSprite;

			owner->reload = 15;
		}

		playBattleSound(SND_SHOTGUN, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
	}
}

void fireMissile(Unit *owner)
{
	Bullet *missile;
	
	if (owner->facing != FACING_DIE)
	{
		missile = createBaseBullet(owner, missileSprite[0]->w);
		missile->y = owner->y + 10;
		missile->facing = owner->facing;
		missile->dx = owner->facing == FACING_RIGHT ? 10 : -10;
		missile->health = FPS * 3;
		missile->sprite[0] = missileSprite[0];
		missile->sprite[1] = missileSprite[1];

		initMissile(missile);

		owner->reload = FPS / 2;

		playBattleSound(SND_MISSILE, owner->uniqueId % MAX_SND_CHANNELS, owner->x, owner->y);
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
