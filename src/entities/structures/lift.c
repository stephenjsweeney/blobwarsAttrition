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
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initLift(Entity *e)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_LIFT;
	
	s->state = LIFT_GOTO_FINISH;

	s->flags |= EF_WEIGHTLESS | EF_NO_ENVIRONMENT | EF_EXPLODES | EF_NO_CLIP | EF_ALWAYS_PROCESS | EF_NO_TELEPORT;

	s->speed = 2;

	s->waitTime = 2;

	s->startX = s->startY = -1;

	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("Lift");

	s->active = 1;
	
	s->action = action;
	s->activate = activate;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
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
			setGameplayMessage(MSG_GAMEPLAY, _("Lift activated ..."));
		}
	}
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->active = cJSON_GetObjectItem(root, "active")->valueint;
	s->tx = cJSON_GetObjectItem(root, "tx")->valueint;
	s->ty = cJSON_GetObjectItem(root, "ty")->valueint;
	s->speed = cJSON_GetObjectItem(root, "speed")->valueint;
	s->state = cJSON_GetObjectItem(root, "state")->valueint;
	s->startX = cJSON_GetObjectItem(root, "startX")->valueint;
	s->startY = cJSON_GetObjectItem(root, "startY")->valueint;
	s->waitTime = cJSON_GetObjectItem(root, "waitTime")->valueint;
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "Lift");
	cJSON_AddNumberToObject(root, "active", s->active);
	cJSON_AddNumberToObject(root, "tx", s->tx);
	cJSON_AddNumberToObject(root, "ty", s->ty);
	cJSON_AddNumberToObject(root, "speed", s->speed);
	cJSON_AddNumberToObject(root, "state", s->state);
	cJSON_AddNumberToObject(root, "startX", s->startX);
	cJSON_AddNumberToObject(root, "startY", s->startY);
	cJSON_AddNumberToObject(root, "waitTime", s->waitTime);
}
