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

#include "entities.h"

Entity *createEntity(void)
{
	Entity *e;
	
	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	world.entityTail->next = e;
	world.entityTail = e;
	
	e->uniqueId = game.entityCounter++;
	
	return e;
}

void initEntity(Entity *e)
{
	e->sprite[0] = e->sprite[1] = e->sprite[2] = -1;
	
	e->environment = ENV_AIR;
	e->alive = ALIVE_ALIVE;

	e->isOnGround = 0;
	e->isSolid = 0;
	e->isStatic = 0;
	e->isMissionTarget = 0;

	e->health = e->healthMax = 1;

	e->facing = FACING_LEFT;

	e->spriteFrame = -1;
	e->spriteTime = 0;

	e->dx = e->dy = 0;

	e->flags = 0;

	e->owner = NULL;

	e->thinkTime = 0;

	e->plane = PLANE_BACKGROUND;
}

SDL_Rect *getEntityBounds(Entity *e)
{
	e->bounds.x = e->x;
	e->bounds.y = e->y;
	e->bounds.w = e->w;
	e->bounds.h = e->h;

	return &e->bounds;
}

int getCurrentEntitySprite(Entity *e)
{
	if (e->alive == ALIVE_ALIVE)
	{
		return e->sprite[e->facing];
	}

	return e->sprite[FACING_DIE];
}

void animateEntity(Entity *e)
{
	Sprite *spr;
	
	if (e->spriteTime != -1)
	{
		e->spriteTime--;

		if (e->spriteTime <= 0)
		{
			spr = getSpriteByIndex(getCurrentEntitySprite(e));
			e->spriteFrame = wrap(e->spriteFrame + 1, 0, 1);
			e->spriteTime = 0;
			e->w = spr->w;
			e->h = spr->h;
		}
	}
}

void setEntitySize(Entity *e)
{
	Sprite *spr;
	
	spr = getSpriteByIndex(getCurrentEntitySprite(e));
	e->w = spr->w;
	e->h = spr->h;
}

float bounce(Entity *e, float x)
{
	if (!(e->flags & EF_BOUNCES))
	{
		return 0;
	}
	else if (e->flags & EF_FRICTIONLESS)
	{
		return -x;
	}

	x = -x * FRICTION;

	if (x > -1 && x < 1)
	{
		e->flags &= ~EF_BOUNCES;
		x = 0;
	}

	return x;
}

void teleport(Entity *e, float tx, float ty)
{
	e->tx = tx;
	e->ty = ty;

	e->flags |= EF_TELEPORTING;

	addTeleportStars(e);
}

void activateEntities(char *names, int activate)
{
}

void applyEntityDamage(Entity *e, int damage)
{
	if (e->health < 0)
	{
		e->health = 0;
		e->alive = ALIVE_ALIVE;
	}

	e->health -= damage;

	if (e->health > 0)
	{
		e->thinkTime = 0;

		e->facing = e->x < world.bob->x ? FACING_RIGHT : FACING_LEFT;

		if (e->isMissionTarget && rand() % 10 == 0)
		{
			e->currentAction = unitReappear;
			e->flags |= EF_GONE;
			e->thinkTime = rand() % FPS;
			addTeleportStars(e);
			playSound(SND_APPEAR, CH_ANY);
		}
	}
}
