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

#include "item.h"

static void reset(void);
static void tick(void);
static void touch(Entity *other);
static void changeEnvironment(void);
static void die(void);
static void destructablePickupItem(Structure *s);
static void enemyPickupItem(Unit *u);
static void bobPickupItem(void);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *createItem(void)
{
	Item *i;
	
	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));
	world.entityTail->next = (Entity*)i;
	world.entityTail = (Entity*)i;
	
	initEntity((Entity*)i);
	
	i->type = ET_ITEM;

	STRNCPY(i->spriteName, "Weapon", MAX_NAME_LENGTH);

	i->flags |= EF_IGNORE_BULLETS;

	i->isMissionTarget = 1;
	i->canBePickedUp = 1;
	i->canBeCarried = 0;
	i->collected = 0;

	i->sprite[FACING_LEFT] = i->sprite[FACING_RIGHT] = i->sprite[FACING_DIE] = getSprite(i->spriteName);

	i->tick = tick;
	i->touch = touch;
	i->changeEnvironment = changeEnvironment;
	i->reset = reset;
	i->die = die;
	i->load = load;
	i->save = save;
	
	return (Entity*)i;
}

Entity *initItem(void)
{
	return createItem();
}

static void reset(void)
{
	Item *i;
	
	i = (Item*)self;
	
	i->startX = (int) self->x;
	i->startY = (int) self->y;
}

static void tick(void)
{
	if (self->isOnGround)
	{
		self->dx *= 0.95;
	}

	/* hack, to ensure the size doesn't exceed the maxmium of something carrying it */
	self->w = self->w > 32 ? 32 : self->w;
}

static void touch(Entity *other)
{
	Item *i;
	
	i = (Item*)self;
	
	if (i->alive == ALIVE_ALIVE && other != NULL && i->canBePickedUp)
	{
		if (other->type == ET_BOB && !world.bob->stunTimer)
		{
			bobPickupItem();
		}
		else if (other->type == ET_ENEMY)
		{
			enemyPickupItem((Unit*)other);
		}
		else if (other->type == ET_DESTRUCTABLE)
		{
			destructablePickupItem((Structure*)other);
		}
	}
}

static void bobPickupItem(void)
{
	Item *i;
	
	i = (Item*)self;
	
	if (!i->isMissionTarget)
	{
		if (i->thinkTime == 0)
		{
			i->alive = ALIVE_DEAD;
			addKey(i->name);
			game.keysFound++;
			updateObjective("KEY");

			setGameplayMessage(MSG_STANDARD, _("Picked up a %s"), i->name);

			playSound(SND_KEY, CH_ITEM);
		}
		else
		{
			setGameplayMessage(MSG_GAMEPLAY, _("Can't carry any more keys"));
		}
	}
	else if (i->canBeCarried)
	{
		if (numCarriedItems() < MAX_ITEMS)
		{
			i->flags |= EF_GONE;

			if (!i->collected)
			{
				updateObjective(i->name);
				i->collected = 1;
			}

			addBobItem(i);
			
			setGameplayMessage(MSG_STANDARD, _("Picked up a %s"), i->name);

			playSound(SND_ITEM, CH_ITEM);
		}
		else
		{
			setGameplayMessage(MSG_GAMEPLAY, _("Can't carry any more items"));
		}
	}
	else
	{
		i->alive = ALIVE_DEAD;
		updateObjective(i->name);

		if (strcmp(world.id, "teeka") != 0)
		{
			setGameplayMessage(MSG_STANDARD, _("Picked up a %s"), i->name);
		}

		playSound(SND_ITEM, CH_ITEM);
	}
}

static void enemyPickupItem(Unit *u)
{
	Item *i;
	
	i = (Item*)self;
	
	if (u->canCarryItem && u->carriedItem == NULL && u->alive == ALIVE_ALIVE)
	{
		u->carriedItem = i;

		i->flags |= EF_GONE;
	}
}

static void destructablePickupItem(Structure *s)
{
	Item *i;
	
	i = (Item*)self;
	
	if (s->carriedItem == NULL && s->alive == ALIVE_ALIVE)
	{
		s->carriedItem = i;

		i->flags |= EF_GONE;
	}
}

static void changeEnvironment(void)
{
	Item *i;
	
	i = (Item*)self;
	
	if (i->environment == ENV_SLIME || i->environment == ENV_LAVA)
	{
		addTeleportStars(self);
		i->x = i->startX;
		i->y = i->startY;
		addTeleportStars(self);
		playSound(SND_APPEAR, CH_ANY);
	}
}

static void die(void)
{
	/* we will handle this ourselves! */
}

static void load(cJSON *root)
{
	Item *i;
	
	i = (Item*)self;
	
	i->canBeCarried = cJSON_GetObjectItem(root, "canBeCarried")->valueint;
	i->canBePickedUp = cJSON_GetObjectItem(root, "canBePickedUp")->valueint;
	i->isMissionTarget = cJSON_GetObjectItem(root, "isMissionTarget")->valueint;
	STRNCPY(i->spriteName, cJSON_GetObjectItem(root, "spriteName")->valuestring, MAX_NAME_LENGTH);
	i->startX = cJSON_GetObjectItem(root, "startX")->valueint;
	i->startY = cJSON_GetObjectItem(root, "startY")->valueint;
	if (cJSON_GetObjectItem(root, "collected"))
	{
		i->collected = cJSON_GetObjectItem(root, "collected")->valueint;
	}
}

static void save(cJSON *root)
{
	Item *i;
	
	i = (Item*)self;
	
	cJSON_AddStringToObject(root, "type", "Item");
	cJSON_AddNumberToObject(root, "canBeCarried", i->canBeCarried);
	cJSON_AddNumberToObject(root, "canBePickedUp", i->canBePickedUp);
	cJSON_AddNumberToObject(root, "isMissionTarget", i->isMissionTarget);
	cJSON_AddStringToObject(root, "spriteName", i->spriteName);
	cJSON_AddNumberToObject(root, "startX", i->startX);
	cJSON_AddNumberToObject(root, "startY", i->startY);
	cJSON_AddNumberToObject(root, "collected", i->collected);
}
