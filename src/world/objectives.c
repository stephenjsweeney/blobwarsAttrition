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

static int isMissingHeartCell(char *targetName);
static int countTargetsInWorld(char *targetName);

static int missingHeartCell;
static int isReturnVisit;
static int isEliminateAllEnemies;

void initObjectives(void)
{
	int totalTargets;
	Objective *o;
	
	isReturnVisit = world.currentStatus == MS_PARTIAL || world.currentStatus == MS_MISSING_HEART_CELL;
	missingHeartCell = world.currentStatus == MS_MISSING_HEART_CELL;
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "isReturnVisit = %d", isReturnVisit);
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "missingHeartCell = %d", missingHeartCell);

	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		totalTargets = countTargetsInWorld(o->targetName);

		if (strcmp(o->targetName, "ENEMY") == 0)
		{
			isEliminateAllEnemies = 1;
		}

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

		if (o->required && isReturnVisit)
		{
			o->targetValue = o->totalValue;
			o->required = 0;
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
	
	if (missingHeartCell && !isMissingHeartCell(targetName))
	{
		return;
	}

	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (strcmp(o->targetName, targetName) == 0)
		{
			o->currentValue++;

			if (strcmp(o->targetName, "KEY") != 0)
			{
				if (o->currentValue == o->targetValue)
				{
					setGameplayMessage(MSG_OBJECTIVE, _("%s - Objective Complete!"), o->description);
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
			if (o->required || isReturnVisit)
			{
				world.allObjectivesComplete = 0;
			}
		}
	}

	if (world.allObjectivesComplete)
	{
		setGameplayMessage(MSG_OBJECTIVE, _("Mission Complete!"));
	}
}

static int isMissingHeartCell(char *targetName)
{
	Entity *e;
	
	if (targetName == NULL || strcmp(targetName, "HEART_CELL") == 0)
	{
		return 0;
	}

	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->alive == ALIVE_ALIVE)
		{
			if (e->type == ET_HEART_CELL)
			{
				return 0;
			}
		}
	}

	return 1;
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
