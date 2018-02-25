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

static void init(void);
static void tick(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initCardReader(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_CARD_READER;
	
	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_NO_ENVIRONMENT | EF_IGNORE_BULLETS | EF_NO_TELEPORT;

	STRNCPY(s->requiredItem, "Black Keycard", MAX_NAME_LENGTH);

	s->isStatic = 1;
	
	s->tick = tick;
	s->init = init;
	s->touch = touch;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (!s->active)
	{
		s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("CardReaderIdle");
	}
	else
	{
		s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("CardReader");
	}
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

static void touch(Entity *other)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (!s->active && other->type == ET_BOB)
	{
		if (hasItem(s->requiredItem) || dev.cheatKeys)
		{
			activateEntities(s->targetNames, 1);

			setGameplayMessage(MSG_GAMEPLAY, _("%s removed"), s->requiredItem);

			removeItem(s->requiredItem);

			s->active = 1;
			s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSprite("CardReader");
			s->spriteTime = 0;
			s->spriteFrame = 0;

			playSound(SND_CONFIRMED, s->uniqueId % MAX_SND_CHANNELS);
		}
		else if (s->bobTouching == 0)
		{
			setGameplayMessage(MSG_GAMEPLAY, _("%s required"), s->requiredItem);
			playSound(SND_DENIED, s->uniqueId % MAX_SND_CHANNELS);
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
	STRNCPY(s->requiredItem, cJSON_GetObjectItem(root, "requiredCard")->valuestring, MAX_NAME_LENGTH);
	STRNCPY(s->targetNames, cJSON_GetObjectItem(root, "targetNames")->valuestring, MAX_DESCRIPTION_LENGTH);
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "CardReader");
	cJSON_AddNumberToObject(root, "active", s->active);
	cJSON_AddStringToObject(root, "requiredCard", s->requiredItem);
	cJSON_AddStringToObject(root, "targetNames", s->targetNames);
}
