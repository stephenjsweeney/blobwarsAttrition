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

#include "game.h"

/*
public Map<String, MissionStatus> missionStatuses;

public List<String> mias;
public List<String> targets;
public Map<String, AtomicInteger> keys;
*/

void initGame(void)
{
	memset(&game, 0, sizeof(Game));
	
	game.cells = 5;
	game.hearts = 10;

	game.timePlayed = 0;
}

void addRescuedMIA(char *name)
{
	int i;
	
	for (i = 0 ; i < game.totalMIAs ; i++)
	{
		if (strcmp(game.mias[i], "") == 0)
		{
			STRNCPY(game.mias[i], name, MAX_NAME_LENGTH);
			return;
		}
	}
}

void addDefeatedTarget(char *name)
{
	int i;
	
	for (i = 0 ; i < game.totalTargets ; i++)
	{
		if (strcmp(game.targets[i], "") == 0)
		{
			STRNCPY(game.targets[i], name, MAX_NAME_LENGTH);
			return;
		}
	}
}

void addKey(char *name)
{
	
}

int hasItem(char *name)
{
	return 0;
}

Entity *getItem(char *name)
{
	return NULL;
}

void removeItem(char *name)
{
}

void destroyGame(void)
{
}
