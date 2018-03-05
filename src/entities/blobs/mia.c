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

#include "mia.h"

static void init(void);
static void reset(void);
static void tick(void);
static void touch(Entity *other);
static void preTeleport(void);
static void teleport(void);
static void load(cJSON *root);
static void save(cJSON *root);

Entity *initMIA(void)
{
	MIA *m;
	
	m = malloc(sizeof(MIA));
	memset(m, 0, sizeof(MIA));
	
	initEntity((Entity*)m);
	
	m->type = ET_MIA;
	
	m->tx = m->ty = -1;

	m->sprite[FACING_LEFT] = getSprite("MIA");
	m->sprite[FACING_RIGHT] = getSprite("MIA");
	m->sprite[FACING_DIE] = getSprite("MIA");

	m->flags |= EF_IGNORE_BULLETS;

	/* blink at random intervals */
	m->spriteFrame = 0;
	m->spriteTime = rand() % 180;

	m->init = init;
	m->reset = reset;
	m->tick = tick;
	m->touch = touch;
	m->load = load;
	m->save = save;

	m->isMissionTarget = 1;
	
	return (Entity*)m;
}

static void init(void)
{
	if (self->tx == -1 && self->ty == -1)
	{
		self->tx = self->x;
		self->ty = self->y;
	}
}

static void reset(void)
{
	self->tx = self->ty = -1;
}

static void tick(void)
{
	MIA *m;
	
	m = (MIA*)self;
	
	m->shudderTimer--;
	if (m->shudderTimer <= 0)
	{
		m->x = (m->tx + rand() % 4);
		m->shudderTimer = 2;
	}

	if (!m->isMissionTarget)
	{
		m->starTimer--;
		if (m->starTimer <= 0)
		{
			addMIATeleportStars(m->x + rand() % m->w, m->y + rand() % m->h);
			m->starTimer = 1;
		}
	}
}

static void touch(Entity *other)
{
	MIA *m;
	
	m = (MIA*)self;
	
	if (m->isMissionTarget && other == (Entity*)world.bob)
	{
		m->action = preTeleport;
		m->teleportTimer = FPS * 3;
		setGameplayMessage(MSG_OBJECTIVE, _("Rescued %s"), m->name);
		m->isMissionTarget = 0;
		m->flags |= EF_ALWAYS_PROCESS;
		m->plane = PLANE_FOREGROUND;
		playSound(SND_MIA, m->uniqueId % MAX_SND_CHANNELS);
	}
}

static void preTeleport(void)
{
	MIA *m;
	
	m = (MIA*)self;
	
	if (--m->teleportTimer <= FPS)
	{
		m->action = teleport;
		m->flags |= (EF_NO_CLIP | EF_WEIGHTLESS);
		m->dy = -5;
	}
}

static void teleport(void)
{
	MIA *m;
	
	m = (MIA*)self;
	
	if (--m->teleportTimer <= 0)
	{
		addTeleportStars(self);
		game.stats[STAT_MIAS_RESCUED]++;
		updateObjective("MIA");
		m->alive = ALIVE_DEAD;
	}
}

static void load(cJSON *root)
{
	MIA *m;
	
	m = (MIA*)self;
	
	m->active = cJSON_GetObjectItem(root, "isMissionTarget")->valueint;
}

static void save(cJSON *root)
{
	MIA *m;
	
	m = (MIA*)self;
	
	cJSON_AddStringToObject(root, "type", "MIA");
	cJSON_AddNumberToObject(root, "isMissionTarget", m->isMissionTarget);
}
