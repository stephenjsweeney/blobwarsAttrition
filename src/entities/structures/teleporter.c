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

#include "teleporter.h"

static void init(void);
static void action(void);
static void touch(Entity *other);
static void activate(int active);
static void getCollisionBounds(SDL_Rect *r);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initTeleporter(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_TELEPORTER;
	
	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("TeleporterActive");
	
	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_IGNORE_BULLETS | EF_NO_TELEPORT;

	s->plane = PLANE_FOREGROUND;

	s->isStatic = 1;

	s->active = 1;
	
	s->init = init;
	s->action = action;
	s->touch = touch;
	s->activate = activate;
	s->getCollisionBounds = getCollisionBounds;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (s->active)
	{
		s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("TeleporterActive");
	}
	else
	{
		s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("TeleporterInactive");
	}
	
	s->spriteTime = 0;
	s->spriteFrame = 0;
}

static void action(void)
{
	if (self->active)
	{
		addTeleporterEffect(self->x + rand() % 85, self->y);
	}

	self->thinkTime = 1;
}

static void touch(Entity *other)
{
	float tx, ty;
	
	if (self->active && other != self && (other->flags & (EF_TELEPORTING | EF_NO_TELEPORT)) == 0)
	{
		tx = self->tx;
		other->tx += self->w / 2;
		other->tx -= other->w / 2;

		ty = self->ty;
		other->ty += self->h / 2;
		other->ty -= other->h / 2;

		teleportEntity(other, tx, ty);

		playBattleSound(SND_TELEPORT, self->uniqueId % MAX_SND_CHANNELS, self->x, self->y);
	}
}

static void activate(int active)
{
	self->active = active;

	self->init();

	if (self->active)
	{
		observeActivation(self);

		if (!isOnScreen(self))
		{
			setGameplayMessage(MSG_GAMEPLAY, app.strings[ST_TELEPORTER]);
		}
	}
}

static void getCollisionBounds(SDL_Rect *r)
{
	r->x = self->x + 16;
	r->y = self->y - 4;
	r->w = 64;
	r->h = self->h;
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->active = cJSON_GetObjectItem(root, "active")->valueint;
	s->tx = cJSON_GetObjectItem(root, "tx")->valueint;
	s->ty = cJSON_GetObjectItem(root, "ty")->valueint;
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "Teleporter");
	cJSON_AddNumberToObject(root, "active", s->active);
	cJSON_AddNumberToObject(root, "tx", s->tx);
	cJSON_AddNumberToObject(root, "ty", s->ty);
}
