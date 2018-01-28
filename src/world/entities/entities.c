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

void animateEntity(void);
static void applyDamage(int damage);
static float bounce(float x);
static SDL_Rect *getBounds(void);

Entity *createEntity(int type)
{
	Entity *e;
	
	switch (type)
	{
		case ET_BOB:
			e = malloc(sizeof(Bob));
			memset(e, 0, sizeof(Bob));
			break;
		
		case ET_ENEMY:
		case ET_TEEKA:
			e = malloc(sizeof(Unit));
			memset(e, 0, sizeof(Unit));
			break;
			
		case ET_BOSS:
			e = malloc(sizeof(Boss));
			memset(e, 0, sizeof(Boss));
			break;
		
		case ET_HEART_CELL:
		case ET_KEY:
		case ET_ITEM:
		case ET_CONSUMABLE:
			e = malloc(sizeof(Item));
			memset(e, 0, sizeof(Item));
			break;
		
		case ET_MIA:
			e = malloc(sizeof(MIA));
			memset(e, 0, sizeof(MIA));
			break;
		
		case ET_DECORATION:
			e = malloc(sizeof(Decoration));
			memset(e, 0, sizeof(Decoration));
			break;
			
		case ET_DOOR:
		case ET_LIFT:
		case ET_PUSHBLOCK:
		case ET_DESTRUCTABLE:
		case ET_POWER_POINT:
		case ET_CARD_READER:
		case ET_PRESSURE_PLATE:
		case ET_TELEPORTER:
		case ET_ITEM_PAD:
		case ET_POOL:
		case ET_TRAP:
		case ET_EXIT:
		case ET_INFO_POINT:
			e = malloc(sizeof(Structure));
			memset(e, 0, sizeof(Structure));
			break;
	}
	
	world.entityTail->next = e;
	world.entityTail = e;
	
	e->type = type;
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

	e->health = 1;

	e->facing = FACING_LEFT;

	e->flags = 0;

	e->thinkTime = 0;
	
	e->animate = animateEntity;
	e->applyDamage = applyDamage;
	e->bounce = bounce;
	e->getBounds = getBounds;
}

static SDL_Rect *getBounds(void)
{
	self->bounds.x = self->x;
	self->bounds.y = self->y;
	self->bounds.w = self->w;
	self->bounds.h = self->h;

	return &self->bounds;
}

int getCurrentEntitySprite(EntityExt *e)
{
	if (e->alive == ALIVE_ALIVE)
	{
		return e->sprite[e->facing];
	}

	return e->sprite[FACING_DIE];
}

void animateEntity(void)
{
}

void setEntitySize(Entity *e)
{
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

void teleportEntity(Entity *e, float tx, float ty)
{
}

static void applyDamage(int damage)
{
	
}

void dropCarriedItem(void)
{
	EntityExt *e;
	Item *i;
	
	e = (EntityExt*)self;
	
	if (e->carriedItem != NULL)
	{
		i = e->carriedItem;
		
		i->x = (e->x + e->w / 2) - i->w / 2;
		i->y = e->y;

		i->dx = i->dy = 0;

		world.entityTail->next = (Entity*)i;
		world.entityTail = (Entity*)i;
		world.entityTail->next = NULL;

		e->carriedItem = NULL;
	}
}
