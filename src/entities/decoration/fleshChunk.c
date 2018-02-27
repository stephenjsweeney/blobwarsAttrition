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

#include "fleshChunk.h"

static void tick(void);
static void action(void);
static void touch(Entity *other);

void initFleshChunk(Decoration *d)
{
	initEntity((Entity*)d);
	
	d->type = ET_DECORATION;

	d->flags |= EF_BOUNCES | EF_IGNORE_BULLETS | EF_KILL_OFFSCREEN | EF_NO_TELEPORT;

	d->bleedTime = FPS * 3;

	d->spriteFrame = 0;

	d->tick = tick;
	d->action = action;
	d->touch = touch;
}

static void tick(void)
{
	Decoration *d;
	
	d = (Decoration*)self;
	
	d->health--;
	d->bleedTime--;
}

static void action(void)
{
	Decoration *d;
	
	d = (Decoration*)self;
	
	if (d->bleedTime > 0 || app.config.blood == 2)
	{
		addBlood(d->x + (rand() % d->w), d->y + (rand() % d->h));
	}

	d->thinkTime = 1;
}

static void touch(Entity *other)
{
	if (other == NULL)
	{
		self->dx *= 0.9;
	}
}
