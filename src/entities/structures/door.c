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

#include "door.h"

static void init(void);
static void tick(void);
static void touch(Entity *other);
static void openWithKey(void);
static int isClosed(void);
static int isOpening(void);
static int isClosing(void);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initDoor(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_DOOR;
	
	s->isSolid = 1;

	s->flags |= EF_WEIGHTLESS | EF_NO_ENVIRONMENT | EF_NO_CLIP | EF_EXPLODES | EF_NO_TELEPORT;

	s->closedX = s->closedY = -1;

	s->state = DOOR_CLOSED;

	s->speed = 8;

	s->isLocked = 1;

	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("Door");
	
	if (s->closedX == -1 && s->closedY == -1)
	{
		s->closedX = (int) s->x;
		s->closedY = (int) s->y;
	}
	
	s->init = init;
	s->tick = tick;
	s->touch = touch;
	s->load = load;
	s->save = save;
	
	return (Entity*)s;
}

static void init(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (s->closedX == -1 && s->closedY == -1)
	{
		s->closedX = s->x;
		s->closedY = s->y;
	}
}

Entity *initBronzeDoor(void)
{
	Structure *s;
	
	s = (Structure*)initDoor();
	
	STRNCPY(s->requiredItem, "Bronze Key", MAX_NAME_LENGTH);

	s->speed = 2;

	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("BronzeDoor");
	
	return (Entity*)s;
}

Entity *initSilverDoor(void)
{
	Structure *s;
	
	s = (Structure*)initDoor();
	
	STRNCPY(s->requiredItem, "Silver Key", MAX_NAME_LENGTH);

	s->speed = 2;

	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("SilverDoor");
	
	return (Entity*)s;
}

Entity *initGoldDoor(void)
{
	Structure *s;
	
	s = (Structure*)initDoor();
	
	STRNCPY(s->requiredItem, "Gold Key", MAX_NAME_LENGTH);

	s->speed = 2;

	s->sprite[0] = s->sprite[1] = s->sprite[2] = getSprite("GoldDoor");
	
	return (Entity*)s;
}

static void tick(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->dx = s->dy = 0;

	if (isOpening())
	{
		getSlope(s->tx, s->ty, s->x, s->y, &s->dx, &s->dy);

		s->dx *= s->speed;
		s->dy *= s->speed;

		if (abs(s->x - s->tx) < s->speed && abs(s->y - s->ty) < s->speed)
		{
			s->x = s->tx;
			s->y = s->ty;
		}

		s->isStatic = 0;
	}
	else if (isClosing())
	{
		getSlope(s->closedX, s->closedY, s->x, s->y, &s->dx, &s->dy);

		s->dx *= s->speed;
		s->dy *= s->speed;

		if (abs(s->x - s->closedX) < s->speed && abs(self->y - s->closedY) < s->speed)
		{
			s->x = s->closedX;
			s->y = s->closedY;
		}

		s->isStatic = 0;
	}

	if (s->dx == 0 && s->dy == 0 && !s->isStatic)
	{
		s->isStatic = 1;

		playSound(SND_DOOR_FINISH, CH_MECHANICAL);
	}
}

static void touch(Entity *other)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if ((other->type != ET_BOB && s->isLocked) || (other->type != ET_BOB && other->type != ET_ENEMY))
	{
		return;
	}

	if (s->isLocked && !dev.cheatKeys)
	{
		if (isClosed())
		{
			if (strlen(s->requiredItem) != 0)
			{
				openWithKey();
			}
			else if (s->thinkTime == 0)
			{
				setGameplayMessage(MSG_GAMEPLAY, _("Door is locked"));

				playSound(SND_DENIED, CH_MECHANICAL);
			}

			s->thinkTime = 2;

			return;
		}
		else if (isClosing())
		{
			return;
		}
	}

	if (s->state != DOOR_OPEN)
	{
		playSound(SND_DOOR_START, CH_MECHANICAL);
	}

	s->state = DOOR_OPEN;
}

static void openWithKey(void)
{
	Structure *s;
	
	s = (Structure*)self;
	
	if (hasItem(s->requiredItem))
	{
		if (s->thinkTime <= 0)
		{
			setGameplayMessage(MSG_GAMEPLAY, _("%s required"), s->requiredItem);

			playSound(SND_DENIED, CH_MECHANICAL);
		}

		s->thinkTime = 2;

		return;
	}

	removeItem(s->requiredItem);

	setGameplayMessage(MSG_GAMEPLAY, _("%s removed"), s->requiredItem);

	STRNCPY(s->requiredItem, "", MAX_NAME_LENGTH);
	s->isLocked = 0;

	if (s->state != DOOR_OPEN)
	{
		playSound(SND_DOOR_START, CH_MECHANICAL);
	}

	s->state = DOOR_OPEN;
}

static int isOpening(void)
{
	Structure *s = (Structure*)self;
	
	return (s->state == DOOR_OPEN && ((int) s->x != (int) s->tx || (int) s->y != (int) s->ty));
}

static int isClosing(void)
{
	Structure *s = (Structure*)self;
	
	return (s->state == DOOR_CLOSED && ((int) s->x != s->closedX || (int) s->y != s->closedY));
}

static int isClosed(void)
{
	Structure *s = (Structure*)self;
	
	return (s->state == DOOR_CLOSED && ((int) s->x == s->closedX && (int) s->y == s->closedY));
}

static void load(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	s->active = cJSON_GetObjectItem(root, "isLocked")->valueint;
	if (cJSON_GetObjectItem(root, "requiredKey"))
	{
		STRNCPY(s->requiredItem, cJSON_GetObjectItem(root, "requiredKey")->valuestring, MAX_NAME_LENGTH);
	}
	s->tx = cJSON_GetObjectItem(root, "tx")->valueint;
	s->ty = cJSON_GetObjectItem(root, "ty")->valueint;
	s->speed = cJSON_GetObjectItem(root, "speed")->valueint;
	s->state = cJSON_GetObjectItem(root, "state")->valueint;
}

static void save(cJSON *root)
{
	Structure *s;
	
	s = (Structure*)self;
	
	cJSON_AddStringToObject(root, "type", "Door");
	cJSON_AddNumberToObject(root, "isLocked", s->isLocked);
	cJSON_AddNumberToObject(root, "tx", s->tx);
	cJSON_AddNumberToObject(root, "ty", s->ty);
	cJSON_AddNumberToObject(root, "speed", s->speed);
	cJSON_AddNumberToObject(root, "state", s->state);
	cJSON_AddStringToObject(root, "requiredKey", s->requiredItem);
	
	if (strcmp(s->sprite[0]->name, "Door") != 0)
	{
		cJSON_AddStringToObject(root, "type", s->sprite[0]->name);
	}
}
