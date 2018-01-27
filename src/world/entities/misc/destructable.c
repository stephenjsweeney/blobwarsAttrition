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

#include "destructable.h"

static void applyDamage(int amount);
static void action(void);

void initDestructable(Entity *e)
{
	initEntity(e);
	
	e->isMissionTarget = 1;

	STRNCPY(e->spriteName, "Crate4", MAX_NAME_LENGTH);

	e->flags |= EF_EXPLODES;

	e->health = e->healthMax = 10;
	
	e->sprite[FACING_LEFT] = e->sprite[FACING_RIGHT] = e->sprite[FACING_DIE] = getSpriteIndex(e->spriteName);
	
	e->applyDamage = applyDamage;
	e->action = action;
}

static void applyDamage(int amount)
{
	if (self->health > 0)
	{
		self->health -= amount;
	}
}

static void action(void)
{
	int mx, my;
	
	if (self->health <= 0)
	{
		self->health--;

		if (self->health % 3 == 0)
		{
			mx = (int) ((self->x + (self->w / 2)) / MAP_TILE_SIZE);
			my = (int) ((self->y + self->h) / MAP_TILE_SIZE);
			addScorchDecal(mx, my);

			addExplosion(self->x, self->y, 50, self);
			self->dx = rrnd(-10, 10);
			self->dy = rrnd(-10, 10);
		}

		if (self->health <= -50)
		{
			dropCarriedItem();

			updateObjective(self->name);

			if (strlen(self->targetNames) > 0)
			{
				activateEntities(self->targetNames, 1);
			}

			self->alive = ALIVE_DEAD;
		}
	}
}
