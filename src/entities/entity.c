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

#include "entity.h"

static void init(void);
static void reset(void);
static void action(void);
static void applyDamage(int damage);
static float bounce(float x);
static void tick(void);
static void activate(int active);
static void touch(Entity *other);
static void animate(void);
static void load(cJSON *root);
static void save(cJSON *root);
static SDL_Rect *getCurrentSprite(void);

void initEntity(Entity *e)
{
	e->uniqueId = world.entityCounter++;
	
	e->environment = ENV_AIR;
	e->alive = ALIVE_ALIVE;

	e->isOnGround = 0;
	e->isSolid = 0;
	e->isStatic = 0;
	e->isMissionTarget = 0;

	e->health = 1;

	e->facing = FACING_LEFT;

	e->flags = 0;

	e->thinkTime = 0;
	
	e->spriteFrame = -1;
	e->spriteTime = 0;
	
	e->init = init;
	e->reset = reset;
	e->action = action;
	e->animate = animate;
	e->tick = tick;
	e->touch = touch;
	e->activate = activate;
	e->applyDamage = applyDamage;
	e->bounce = bounce;
	e->getCurrentSprite = getCurrentSprite;
	
	e->load = load;
	e->save = save;
	
	world.entityTail->next = e;
	world.entityTail = e;
}

static void init(void)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Cannot init() entity [name=%s, type=%d, x=%d, y=%d]", self->name, self->type, (int)self->x, (int)self->y);
}

static void reset(void)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Cannot reset() entity [name=%s, type=%d, x=%d, y=%d]", self->name, self->type, (int)self->x, (int)self->y);
}

static void action(void)
{
}

static void animate(void)
{
	Sprite *s;
	
	s = self->sprite[self->facing];
	
	if (self->spriteTime != -1)
	{
		if (--self->spriteTime <= 0)
		{
			if (++self->spriteFrame >= s->numFrames)
			{
				self->spriteFrame = 0;
			}
			
			self->spriteTime = self->sprite[self->facing]->times[self->spriteFrame];
		}
	}
}

static float bounce(float x)
{
	if (!(self->flags & EF_BOUNCES))
	{
		return 0;
	}
	else if (self->flags & EF_FRICTIONLESS)
	{
		return -x;
	}

	x = -x * FRICTION;

	if (x > -1 && x < 1)
	{
		self->flags &= ~EF_BOUNCES;
		x = 0;
	}

	return x;
}

static void applyDamage(int damage)
{
	
}

static void tick(void)
{
}

static void touch(Entity *other)
{
}

static void activate(int active)
{
}

static SDL_Rect *getCurrentSprite(void)
{
	return &self->sprite[self->facing]->frames[self->spriteFrame]->rect;
}

static void load(cJSON *root)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Entity [name=%s, type=%d, x=%d, y=%d] cannot be loaded", self->name, self->type, (int)self->x, (int)self->y);
	exit(1);
}

static void save(cJSON *root)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Entity [name=%s, type=%d, x=%d, y=%d] cannot be saved", self->name, self->type, (int)self->x, (int)self->y);
	exit(1);
}
