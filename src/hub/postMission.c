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

#include "postMission.h"

static void logic(void);
static void draw(void);
static void updateMissionStatus(void);
static int status;

void initPostMission(void)
{
	startSectionTransition();
	
	updateMissionStatus();
	
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	app.restrictTrophyAlert = 0;
	
	endSectionTransition();
}

static void updateMissionStatus(void)
{
	Tuple *t;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, world.id) == 0)
		{
			t->value.i = status = getMissionStatus();
			return;
		}
	}
	
	t = malloc(sizeof(Tuple));
	memset(t, 0, sizeof(Tuple));
	game.missionStatusTail->next = t;
	game.missionStatusTail = t;
	
	STRNCPY(t->key, world.id, MAX_NAME_LENGTH);
	t->value.i = status = getMissionStatus();
	
	if (status != MS_INCOMPLETE)
	{
		saveGame();	
		saveWorld();
	}
}

static void logic(void)
{
	destroyWorld();

	initHub();
}

static void draw(void)
{
}
