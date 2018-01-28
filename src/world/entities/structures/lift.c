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
	Structure *s;
	
	initEntity(e);
	
	s = (Structure*)e;
	
	s->type = ET_LIFT;
	
	s->state = LIFT_GOTO_FINISH;

	s->flags |= EF_WEIGHTLESS | EF_NO_ENVIRONMENT | EF_EXPLODES | EF_NO_CLIP | EF_ALWAYS_PROCESS | EF_NO_TELEPORT;

	s->speed = 2;

	s->waitTime = 2;

	s->startX = s->startY = -1;

	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSpriteIndex("Lift");

	s->active = 1;
	
	s->action = action;
	s->activate = activate;
}

static void action(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->dx = s->dy = 0;

	if (s->active)
	{
		if (s->state == LIFT_GOTO_START)
		{
			getSlope(s->startX, s->startY, s->x, s->y, &s->dx, &s->dy);

			s->dx *= s->speed;
			s->dy *= s->speed;

			if (abs(s->x - s->startX) < s->speed && abs(s->y - s->startY) < s->speed)
			{
				s->x = s->startX;
				s->y = s->startY;
				s->state = LIFT_GOTO_FINISH;
				s->thinkTime = s->waitTime * FPS;
				s->dx = s->dy = 0;
			}
		}
		else if (s->state == LIFT_GOTO_FINISH)
		{
			getSlope(s->tx, s->ty, s->x, s->y, &s->dx, &s->dy);

			s->dx *= s->speed;
			s->dy *= s->speed;

			if (abs(s->x - s->tx) < s->speed && abs(s->y - s->ty) < s->speed)
			{
				s->x = s->tx;
				s->y = s->ty;
				s->state = LIFT_GOTO_START;
				s->thinkTime = s->waitTime * FPS;
				s->dx = s->dy = 0;
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
