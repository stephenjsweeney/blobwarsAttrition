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

#include "triggers.h"

void doLocationTriggers(void)
{
	Trigger *t, *prev;
	
	prev = &world.triggerHead;
	
	for (t = world.triggerHead.next ; t != NULL ; t = t->next)
	{
		if (t->w > 0 && t->h > 0)
		{
			if (collision(t->x, t->y, t->w, t->h, world.bob->x, world.bob->y, world.bob->w, world.bob->h))
			{
				activateEntities(t->targetNames, 1);

				if (t->message != NULL)
				{
					setGameplayMessage(t->message, MSG_GAMEPLAY);
				}
				
				if (t == world.triggerTail)
				{
					world.triggerTail = prev;
				}

				prev->next = t->next;
				free(t);
				t = prev;
			}
		}
		
		prev = t;
	}
}

void fireTriggers(char *name)
{
	Trigger *t, *prev;
	
	prev = &world.triggerHead;
	
	for (t = world.triggerHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->name, name) == 0)
		{
			activateEntities(t->targetNames, 1);

			if (t->message != NULL)
			{
				setGameplayMessage(t->message, MSG_GAMEPLAY);
			}
			
			if (t == world.triggerTail)
			{
				world.triggerTail = prev;
			}

			prev->next = t->next;
			free(t);
			t = prev;
		}
		
		prev = t;
	}
}
