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

static void init(void);
static void tick(void);
static void action(void);
static void touch(Entity *other);
static void getCollisionBounds(SDL_Rect *r);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initExit(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_EXIT;
	
	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("Exit");

	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	s->isStatic = 1;

	s->active = 0;

	s->spriteTime = -1;
	
	s->init = init;
	s->tick = tick;
	s->action = action;
	s->touch = touch;
	s->getCollisionBounds = getCollisionBounds;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	if (!self->active)
	{
		self->spriteFrame = 0;
	}
	else
	{
		self->spriteFrame = 1;
	}

	self->spriteTime = -1;
}

static void tick(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (!s->active)
	{
		s->bobTouching = MAX(s->bobTouching - 1, 0);
	}
}

static void action(void)
{
	Objective *o;
	Structure *s;
	
	s = (Structure*)self;
	
	if (!s->active)
	{
		s->active = 1;

		for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
		{
			if (o->required && strcmp(o->targetName, "EXIT") != 0 && o->currentValue < o->targetValue)
			{
				s->active = 0;
			}
		}

		if (s->active)
		{
			s->spriteFrame = 1;
		}
	}

	s->thinkTime = FPS;
}

static void touch(Entity *other)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (other->type == ET_BOB && !world.isReturnVisit)
	{
		if (s->bobTouching == 0)
		{
			if (s->active)
			{
				updateObjective("EXIT");
				world.missionCompleteTimer = (int) (FPS * 1.5);
				world.bob->flags |= (EF_IMMUNE | EF_GONE);
				world.state = WS_COMPLETE;
				stopMusic();
			}
			else
			{
				setGameplayMessage(MSG_GAMEPLAY, _("Can't exit yet - required objectives not met"));
			}
		}

		s->bobTouching = 2;
	}
}

static void getCollisionBounds(SDL_Rect *r)
{
	r->x = self->x + 64;
	r->y = self->y;
	r->w = 2;
	r->h = self->h;
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->active = cJSON_GetObjectItem(root, "active")->valueint;
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddNumberToObject(root, "active", s->active);
}
