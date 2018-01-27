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

static void tick(void);
static void touch(Entity *other);

void initInfoPoint(Entity *e)
{
	initEntity(e);
	
	e->sprite[0] = e->sprite[1] = e->sprite[2] = getSpriteIndex("InfoPoint");

	e->flags |= EF_WEIGHTLESS | EF_IGNORE_BULLETS | EF_NO_CLIP | EF_NO_ENVIRONMENT;
	
	e->ty = e->y;

	e->firstTouch = 1;
	
	e->tick = tick;
	e->touch = touch;
}

static void tick(void)
{
	self->sinVal -= 0.05;
	self->y += (float) sin(self->sinVal) * 0.1;
}

static void touch(Entity *other)
{
	int showMessage;
	
	if (other == world.bob)
	{
		showMessage = 0;

		if (self->firstTouch)
		{
			self->firstTouch = 0;
			showMessage = 1;
			self->messageTimer = FPS;
		}
		else if (world.bob->dx == 0 && world.bob->dy == 0 && world.bob->isOnGround)
		{
			self->messageTimer++;

			if (self->messageTimer == FPS)
			{
				showMessage = 1;
			}
		}
		else
		{
			self->messageTimer = 0;
		}

		if (showMessage)
		{
			showInfoMessage(self->message);
		}
	}
}
