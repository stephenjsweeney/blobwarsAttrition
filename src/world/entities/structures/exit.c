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

#include "exit.h"

static void tick(void);
static void action(void);
static void touch(Entity *other);
static SDL_Rect getBounds(void);

void initExit(Entity *e)
{
	e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex("Exit");

	e->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	e->isStatic = 1;

	e->active = 0;
	
	if (!e->active)
	{
		e->spriteFrame = 0;
	}
	else
	{
		e->spriteFrame = 1;
	}

	e->spriteTime = -1;
	
	e->tick = tick;
	e->action = action;
	e->touch = touch;
	e->getBounds = getBounds;
}

static void tick(void)
{
	if (!self->active)
	{
		self->bobTouching = MAX(self->bobTouching - 1, 0);
	}
}

static void action(void)
{
	Objective *o;
	
	if (!self->active)
	{
		self->active = 1;

		for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
		{
			if (o->required && strcmp(o->targetName, "EXIT") != 0 && o->currentValue < o->targetValue)
			{
				self->active = 0;
			}
		}

		if (self->active)
		{
			self->spriteFrame = 1;
		}
	}

	self->thinkTime = FPS;
}

static void touch(Entity *other)
{
	if (other->type == ET_BOB && !world.isReturnVisit)
	{
		if (self->bobTouching == 0)
		{
			if (self->active)
			{
				updateObjective("EXIT");
				world.missionCompleteTimer = (int) (FPS * 1.5);
				world.bob->flags |= (EF_IMMUNE | EF_GONE);
				world.state = WS_COMPLETE;
				stopMusic();
			}
			else
			{
				setGameplayMessage(MSG_GAMEPLAY, "Can't exit yet - required objectives not met");
			}
		}

		self->bobTouching = 2;
	}
}

static SDL_Rect getBounds(void)
{
	self->bounds.x = self->x + 64;
	self->bounds.y = self->y;
	self->bounds.w = 2;
	self->bounds.h = self->h;

	return self->bounds;
}
