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

#include "atlasTest.h"

static void logic(void);
static void draw(void);
static int timeout;
static void trackRandomEntity(void);

void initAtlasTest(void)
{
	initGame();
	initHub();
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	loadWorld("data/maps/beachApproach.json");
	
	initMap();
	
	timeout = FPS * 2;
	
	cameraTrack((Entity*)world.bob);
}

static void logic(void)
{
	if (--timeout <= 0)
	{
		trackRandomEntity();
	}
}

static void draw(void)
{
	drawMap();
}

static void trackRandomEntity(void)
{
	Entity *e;
	
	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (rand() % 4 == 0)
		{
			cameraTrack(e);
			timeout = FPS * 2;
			return;
		}
	}
}
