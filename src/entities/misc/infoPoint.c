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

#include "infoPoint.h"

static void init(void);
static void tick(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initInfoPoint(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_INFO_POINT;
	
	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("InfoPoint");

	s->flags |= EF_WEIGHTLESS | EF_IGNORE_BULLETS | EF_NO_CLIP | EF_NO_ENVIRONMENT;
	
	s->ty = s->y;
	
	s->init = init;
	s->tick = tick;
	s->touch = touch;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	self->ty = self->y;
}

static void tick(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->sinVal -= 0.05;
	s->y += (float) sin(s->sinVal) * 0.1;
}

static void touch(Entity *other)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (other == (Entity*)world.bob)
	{
		showInfoMessage(s->message);
	}
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	STRNCPY(s->message, cJSON_GetObjectItem(root, "message")->valuestring, MAX_DESCRIPTION_LENGTH);
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "InfoPoint");
	cJSON_AddStringToObject(root, "message", s->name);
}
