/*
Copyright (C) 2018-2019 Parallel Realities

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

#include "laserTrap.h"

static void init(void);
static void tick(void);
static void action(void);
static void touch(Entity *other);
static void activate(int active);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initLaserTrap(void)
{
	Trap *t;

	t = malloc(sizeof(Trap));
	memset(t, 0, sizeof(Trap));

	initEntity((Entity*)t);

	t->type = ET_TRAP;

	t->flags |= EF_WEIGHTLESS | EF_IGNORE_BULLETS | EF_NO_ENVIRONMENT | EF_NO_CLIP | EF_ALWAYS_PROCESS;

	t->onTime = FPS * 2;
	t->offTime = FPS * 2;

	t->sprite[0] = t->sprite[1] = t->sprite[2] = getSprite("LaserTrap");

	t->active = 1;

	t->init = init;
	t->tick = tick;
	t->action = action;
	t->touch = touch;
	t->activate = activate;
	t->load = load;
	t->save = save;

	return (Entity*)t;
}

static void init(void)
{
	if (!self->active)
	{
		self->flags |= EF_GONE;
	}
}

static void tick(void)
{
	Trap *t;

	t = (Trap*)self;

	if (!t->active && t->spriteTime == -1)
	{
		t->flags |= EF_GONE;
	}
}

static void action(void)
{
	Trap *t;

	t = (Trap*)self;

	if (t->offTime != 0)
	{
		if (!t->active)
		{
			t->thinkTime = t->onTime;
			t->spriteFrame = 0;
			t->spriteTime = 0;
			t->flags &= ~EF_GONE;
		}
		else if (t->offTime > 0)
		{
			t->thinkTime = t->offTime;
			t->spriteTime = 0;
		}

		t->active = !t->active;
	}
	else
	{
		t->spriteTime = -1;
		t->spriteFrame = 4;
	}
}

static void touch(Entity *other)
{
	Trap *t;

	t = (Trap*)self;

	if (other != NULL && (other->type == ET_BOB || other->type == ET_ENEMY))
	{
		if (!(other->flags & EF_IMMUNE))
		{
			if (other->health > 0 || rand() % 100 < 5)
			{
				other->dx = rrnd(-12, 12);
				other->dy = rrnd(-8, 0);

				if (t->offTime != 0)
				{
					swapSelf(other);
					other->applyDamage((int) (other->healthMax / 4) + 1);
					swapSelf(other);
				}
				else
				{
					/* instant kill */
					swapSelf(other);
					other->applyDamage((int) other->healthMax + 1);
					swapSelf(other);
				}

				playBattleSound(SND_FLESH_HIT, other->uniqueId % MAX_SND_CHANNELS, other->x, other->y);
				playBattleSound(SND_ELECTRIC_HIT, self->uniqueId % MAX_SND_CHANNELS, self->x, self->y);

				if (other->flags & EF_EXPLODES)
				{
					addSparkParticles(t->x, t->y);
				}
				else
				{
					addSmallFleshChunk(t->x, t->y);
				}

				if (other == (Entity*)world.bob && world.bob->stunTimer == 0)
				{
					stunBob();
				}
			}
		}
	}
}

static void activate(int active)
{
	self->active = !active;

	if (!self->active)
	{
		self->flags |= EF_GONE;
	}
	else
	{
		self->flags &= ~EF_GONE;
	}

	if (!self->active)
	{
		observeActivation(self);

		if (!isOnScreen(self))
		{
			setGameplayMessage(MSG_GAMEPLAY, app.strings[ST_LASERS]);
		}
	}
}

static void load(cJSON *root)
{
	Trap *t;

	t = (Trap*)self;

	if (cJSON_GetObjectItem(root, "active"))
	{
		t->active = cJSON_GetObjectItem(root, "active")->valueint;
	}
	t->onTime = cJSON_GetObjectItem(root, "onTime")->valueint;
	t->offTime = cJSON_GetObjectItem(root, "offTime")->valueint;
}

static void save(cJSON *root)
{
	Trap *t;

	t = (Trap*)self;

	cJSON_AddStringToObject(root, "type", t->sprite[0]->name);
	cJSON_AddNumberToObject(root, "active", t->active);
	cJSON_AddNumberToObject(root, "onTime", t->onTime);
	cJSON_AddNumberToObject(root, "offTime", t->offTime);
}
