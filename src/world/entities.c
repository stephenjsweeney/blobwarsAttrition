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

static Texture *atlasTexture;

void initEntities(void)
{
	atlasTexture = getTexture("gfx/atlas/atlas.png");
}

void doEntities(void)
{
	for (self = world.entityHead.next ; self != NULL ; self = self->next)
	{
		if (--self->thinkTime <= 0)
		{
			self->action();
		}
		
		self->tick();
		
		self->animate();
	}
}

void drawEntities(int plane)
{
	int x, y, draw;
	
	for (self = world.entityHead.next ; self != NULL ; self = self->next)
	{
		self->isOnScreen = 1;
		
		draw = self->isOnScreen && !(self->flags & EF_GONE) && self->plane == plane;
		
		if (draw)
		{
			x = (-camera.x + self->x);
			y = (-camera.y + self->y);
			
			blitRect(atlasTexture->texture, x, y, self->getCurrentSprite(), 0);
		}
	}
}

void activateEntities(char *names, int activate)
{
}

void teleportEntity(Entity *e, float tx, float ty)
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

void teleport(Entity *e, float tx, float ty)
{
	e->tx = tx;
	e->ty = ty;

	e->flags |= EF_TELEPORTING;

	addTeleportStars(e);
}

Entity *findEntity(char *name)
{
	Entity *e;
	
	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (strcmp(e->name, name) == 0)
		{
			return e;
		}
	}

	return NULL;
}

Entity *getRandomObjectiveEntity(void)
{
	Entity *rtn, *e;
	
	rtn = (Entity*)world.bob;

	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->isMissionTarget)
		{
			if (rand() % 4 == 0)
			{
				rtn = e;
			}
		}
	}

	return rtn;
}
