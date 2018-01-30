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

static void action(void);
static void touch(Entity *other);
static void activate(int active);

Entity *initTeleporter(void)
{
	Structure *s;
	
	s = createStructure();
	
	s->type = ET_TELEPORTER;
	
	s->flags |= EF_WEIGHTLESS | EF_NO_CLIP | EF_IGNORE_BULLETS | EF_NO_TELEPORT;

	s->plane = PLANE_FOREGROUND;

	s->isStatic = 1;

	s->active = 1;
	
	s->action = action;
	s->touch = touch;
	s->activate = activate;
	
	return (Entity*)s;
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

		playSound(SND_TELEPORT, CH_EFFECTS);
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
			setGameplayMessage(MSG_GAMEPLAY, _("Teleporter activated ..."));
		}
	}
}
