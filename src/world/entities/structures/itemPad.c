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

#include "itemPad.h"

static void tick(void);
static void touch(Entity *other);

void initItemPad(Entity *e)
{
	initEntity(e);
	
	e->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	e->plane = PLANE_FOREGROUND;

	e->isStatic = 1;
	
	if (!e->active)
	{
		e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("ItemPadActive");
	}
	else
	{
		e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("ItemPadInactive");
	}
	
	e->tick = tick;
	e->touch = touch;
}

static void tick(void)
{
	self->bobTouching = MAX(self->bobTouching - 1, 0);
}

static void touch(Entity *other)
{
	Entity *i;
	
	if (other->type == ET_BOB && !self->active)
	{
		i = getItem(self->requiredItem);

		if (i != NULL)
		{
			removeItem(i->name);
			
			i->flags &= ~EF_GONE;
			
			i->x = self->x + (self->w / 2) - (i->w / 2);
			i->y = self->y - i->h;
			
			i->canBeCarried = i->canBePickedUp = i->isMissionTarget = 0;
			
			self->active = 1;

			setGameplayMessage(MSG_GAMEPLAY, "%s removed", self->requiredItem);

			self->sprite[FACING_LEFT] = self->sprite[FACING_RIGHT] = self->sprite[FACING_DIE] = getSpriteIndex("ItemPadActive");

			self->spriteFrame = 0;

			updateObjective(self->name);
		}
		else if (!self->bobTouching)
		{
			setGameplayMessage(MSG_GAMEPLAY, "%s required", self->requiredItem);
		}

		self->bobTouching = 2;
	}
}
