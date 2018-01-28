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
	Structure *s;
	
	initEntity(e);
	
	s = (Structure*)e;
	
	s->isMissionTarget = 1;

	STRNCPY(s->spriteName, "Crate4", MAX_NAME_LENGTH);

	s->flags |= EF_EXPLODES;

	s->health = s->healthMax = 10;
	
	s->sprite[FACING_LEFT] = s->sprite[FACING_RIGHT] = s->sprite[FACING_DIE] = getSpriteIndex(s->spriteName);
	
	s->applyDamage = applyDamage;
	s->action = action;
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
	Structure *s;
	int mx, my;
	
	s = (Structure*)self;
	
	if (s->health <= 0)
	{
		s->health--;

		if (s->health % 3 == 0)
		{
			mx = (int) ((s->x + (s->w / 2)) / MAP_TILE_SIZE);
			my = (int) ((s->y + s->h) / MAP_TILE_SIZE);
			addScorchDecal(mx, my);

			addExplosion(s->x, s->y, 50, self);
			s->dx = rrnd(-10, 10);
			s->dy = rrnd(-10, 10);
		}

		if (s->health <= -50)
		{
			dropCarriedItem();

			updateObjective(s->name);

			if (strlen(s->targetNames) > 0)
			{
				activateEntities(s->targetNames, 1);
			}

			s->alive = ALIVE_DEAD;
		}
	}
}
