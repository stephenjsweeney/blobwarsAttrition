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

#include "cardReader.h"

static void tick(void);
static void touch(Entity *other);

void initCardReader(Entity *e)
{
	initEntity(e);
	
	e->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS | EF_NO_TELEPORT;

	STRNCPY(e->requiredCard, "Black Keycard", MAX_NAME_LENGTH);

	e->isStatic = 1;
	
	if (!e->active)
	{
		e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("CardReaderIdle");
	}
	else
	{
		e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("CardReader");
	}
	
	e->tick = tick;
	e->touch = touch;
}

static void tick(void)
{
	if (!self->active)
	{
		self->bobTouching = MAX(self->bobTouching - 1, 0);
	}
}

static void touch(Entity *other)
{
	if (!self->active && other->type == ET_BOB)
	{
		if (hasItem(self->requiredCard) || dev.cheatKeys)
		{
			activateEntities(self->targetNames, 1);

			setGameplayMessage(MSG_GAMEPLAY, "%s removed", self->requiredCard);

			removeItem(self->requiredCard);

			self->active = 1;
			self->sprite[FACING_LEFT] = self->sprite[FACING_RIGHT] = self->sprite[FACING_DIE] = getSpriteIndex("CardReader");
			self->spriteTime = 0;
			self->spriteFrame = 0;

			playSound(SND_CONFIRMED, CH_TOUCH);
		}
		else if (self->bobTouching == 0)
		{
			setGameplayMessage(MSG_GAMEPLAY, "%s required", self->requiredCard);
			playSound(SND_DENIED, CH_TOUCH);
		}

		self->bobTouching = 2;
	}
}
