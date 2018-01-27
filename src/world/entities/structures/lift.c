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

#include "lift.h"

static void action(void);
static void activate(int active);

void initLift(Entity *e)
{
	initEntity(e);
	
	e->state = LIFT_GOTO_FINISH;

	e->flags |= EF_WEIGHTLESS | EF_NO_ENVIRONMENT | EF_EXPLODES | EF_NO_CLIP | EF_ALWAYS_PROCESS | EF_NO_TELEPORT;

	e->speed = 2;

	e->waitTime = 2;

	e->startX = e->startY = -1;

	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("Lift");

	e->active = 1;
	
	e->action = action;
	e->activate = activate;
}

static void action(void)
{
	self->dx = self->dy = 0;

	if (self->active)
	{
		if (self->state == LIFT_GOTO_START)
		{
			getSlope(self->startX, self->startY, self->x, self->y, &self->dx, &self->dy);

			self->dx *= self->speed;
			self->dy *= self->speed;

			if (abs(self->x - self->startX) < self->speed && abs(self->y - self->startY) < self->speed)
			{
				self->x = self->startX;
				self->y = self->startY;
				self->state = LIFT_GOTO_FINISH;
				self->thinkTime = self->waitTime * FPS;
				self->dx = self->dy = 0;
			}
		}
		else if (self->state == LIFT_GOTO_FINISH)
		{
			getSlope(self->tx, self->ty, self->x, self->y, &self->dx, &self->dy);

			self->dx *= self->speed;
			self->dy *= self->speed;

			if (abs(self->x - self->tx) < self->speed && abs(self->y - self->ty) < self->speed)
			{
				self->x = self->tx;
				self->y = self->ty;
				self->state = LIFT_GOTO_START;
				self->thinkTime = self->waitTime * FPS;
				self->dx = self->dy = 0;
			}
		}
	}
}

static void activate(int active)
{
	self->active = active;

	if (self->active)
	{
		observeActivation(self);

		if (!isOnScreen(self))
		{
			setGameplayMessage(MSG_GAMEPLAY, "Lift activated ...");
		}
	}
}
