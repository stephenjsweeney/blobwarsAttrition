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

static void init(void);
static void tick(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initItemPad(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_ITEM_PAD;
	
	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS;

	s->plane = PLANE_FOREGROUND;

	s->isStatic = 1;
	
	s->init = init;
	s->tick = tick;
	s->touch = touch;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	if (self->active)
	{
		self->sprite[FACING_LEFT] = self->sprite[FACING_RIGHT] = self->sprite[FACING_DIE] = getSprite("ItemPadActive");
	}
	else
	{
		self->sprite[FACING_LEFT] = self->sprite[FACING_RIGHT] = self->sprite[FACING_DIE] = getSprite("ItemPadInactive");
	}
}

static void tick(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->bobTouching = MAX(s->bobTouching - 1, 0);
}

static void touch(Entity *other)
{
	Structure *s;
	Item *i;
	
	s = (Structure*)self;
	
	if (other->type == ET_BOB && !s->active)
	{
		i = getItem(s->requiredItem);

		if (i != NULL)
		{
			removeItem(i->name);
			
			i->flags &= ~EF_GONE;
			
			i->x = s->x + (s->w / 2) - (i->w / 2);
			i->y = s->y - i->h;
			
			i->canBeCarried = i->canBePickedUp = i->isMissionTarget = 0;
			
			s->active = 1;

			setGameplayMessage(MSG_GAMEPLAY, _("%s removed"), s->requiredItem);

			s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("ItemPadActive");

			s->spriteFrame = 0;

			updateObjective(s->name);
		}
		else if (!s->bobTouching)
		{
			setGameplayMessage(MSG_GAMEPLAY, _("%s required"), s->requiredItem);
		}

		s->bobTouching = 2;
	}
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (cJSON_GetObjectItem(root, "active"))
	{
		s->active = cJSON_GetObjectItem(root, "active")->valueint;
	}
	STRNCPY(s->requiredItem, cJSON_GetObjectItem(root, "requiredItem")->valuestring, MAX_NAME_LENGTH);
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "ItemPad");
	cJSON_AddNumberToObject(root, "active", s->active);
	cJSON_AddStringToObject(root, "requiredItem", s->requiredItem);
}
