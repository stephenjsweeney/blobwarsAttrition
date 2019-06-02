/*
Copyright (C) 2018-2019 Parallel Realities

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

#include "player.h"

static void doDebugControls(void);

void doPlayer(void)
{
	if (dev.debug)
	{
		doDebugControls();
	}
}

static void doDebugControls(void)
{
	if (app.keyboard[SDL_SCANCODE_1])
	{
		world.bob->weaponType = WPN_PISTOL;
	}

	if (app.keyboard[SDL_SCANCODE_2])
	{
		world.bob->weaponType = WPN_PLASMA;
	}

	if (app.keyboard[SDL_SCANCODE_3])
	{
		world.bob->weaponType = WPN_SPREAD;
	}

	if (app.keyboard[SDL_SCANCODE_4])
	{
		world.bob->weaponType = WPN_LASER;
	}

	if (app.keyboard[SDL_SCANCODE_5])
	{
		world.bob->weaponType = WPN_GRENADES;
	}

	if (app.keyboard[SDL_SCANCODE_0] && world.state != WS_COMPLETE)
	{
		quitMission();

		autoCompleteMission();

		app.keyboard[SDL_SCANCODE_0] = 0;
	}
}
