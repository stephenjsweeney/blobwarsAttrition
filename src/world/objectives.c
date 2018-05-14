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

#include "objectives.h"

static int countTargetsInWorld(char *targetName);

void initObjectives(void)
{
	int totalTargets;
	Objective *o;
	
	world.isReturnVisit = world.currentStatus == MS_PARTIAL || world.currentStatus == MS_MISSING_HEART_CELL || game.isComplete;
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "world.isReturnVisit = %d", world.isReturnVisit);

	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		totalTargets = countTargetsInWorld(o->targetName);

		if (o->totalValue == 0)
		{
			o->totalValue = totalTargets;
		}

		if (o->targetValue == 0)
		{
			if (strcmp(o->targetName, "KEY") == 0)
			{
				o->targetValue = totalTargets;
			}
			else
			{
				o->targetValue = (int) (totalTargets * AUTO_THRESHOLD);
			}
		}

		if (world.isReturnVisit || game.plus & PLUS_ALL_OBJS)
		{
			o->targetValue = o->totalValue;
			o->required = game.plus & PLUS_ALL_OBJS;
		}
		
		if ((strcmp(o->targetName, "ENEMY") == 0 && o->targetValue == o->totalValue) || game.plus & PLUS_KILL_ALL)
		{
			world.isEliminateAllEnemies = 1;
		}
	}
}

static int countTargetsInWorld(char *targetName)
{
	int num, countMIAs, countEnemies, countKeys;
	Entity *e;
	
	num = 0;
	countMIAs = strcmp(targetName, "MIA") == 0;
	countEnemies = strcmp(targetName, "ENEMY") == 0;
	countKeys = strcmp(targetName, "KEY") == 0;

	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (strcmp(targetName, e->name) == 0)
		{
			num++;
		}

		if (countMIAs && e->type == ET_MIA)
		{
			num++;
		}

		if (countEnemies && e->type == ET_ENEMY)
		{
			num++;
		}

		if (countKeys && e->type == ET_KEY)
		{
			num++;
		}
	}

	return num;
}

void updateObjective(char *targetName)
{
	Objective *o;
	
	if (world.currentStatus != MS_MISSING_HEART_CELL)
	{
		for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
		{
			if (strcmp(o->targetName, targetName) == 0)
			{
				o->currentValue++;

				if (strcmp(o->targetName, "KEY") != 0)
				{
					if (o->currentValue == o->targetValue)
					{
						setGameplayMessage(MSG_OBJECTIVE, app.strings[ST_OBJECTIVE_COMPLETE], o->description);
					}
					else if (o->currentValue < o->targetValue)
					{
						if (strcmp(o->targetName, "ENEMY") != 0)
						{
							setGameplayMessage(MSG_PROGRESS, "%s - (%d / %d)", o->description, o->currentValue, o->targetValue);
						}
						else if (o->targetValue - o->currentValue < 10 || o->currentValue % 5 == 0)
						{
							setGameplayMessage(MSG_PROGRESS, "%s - (%d / %d)", o->description, o->currentValue, o->targetValue);
						}
					}
				}
			}
		}

		world.allObjectivesComplete = 1;

		for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
		{
			if (o->currentValue < o->targetValue)
			{
				if (o->required || world.isReturnVisit)
				{
					world.allObjectivesComplete = 0;
				}
			}
		}

		if (world.allObjectivesComplete)
		{
			setGameplayMessage(MSG_OBJECTIVE, app.strings[ST_MISSION_COMPLETE]);
		}
	}
}

void updateHeartCellObjective(void)
{
	Entity *e;
	
	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->alive == ALIVE_ALIVE)
		{
			if (e->type == ET_HEART || e->type == ET_CELL)
			{
				return;
			}
		}
	}

	if (world.currentStatus == MS_MISSING_HEART_CELL)
	{
		world.allObjectivesComplete = 1;

		setGameplayMessage(MSG_OBJECTIVE, app.strings[ST_MISSION_COMPLETE]);
	}
}

void destroyObjectives(void)
{
	Objective *o;

	while (world.objectiveHead.next)
	{
		o = world.objectiveHead.next;

		world.objectiveHead.next = o->next;

		free(o);
	}
}
