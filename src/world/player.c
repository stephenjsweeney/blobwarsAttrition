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

#include "player.h"

static void doCheatControls(void);

void doPlayer(void)
{
	game.config.control[CONTROL_LEFT] = app.keyboard[SDL_SCANCODE_A];
	game.config.control[CONTROL_RIGHT] = app.keyboard[SDL_SCANCODE_D];
	game.config.control[CONTROL_UP] = app.keyboard[SDL_SCANCODE_W];
	game.config.control[CONTROL_DOWN] = app.keyboard[SDL_SCANCODE_S];
	game.config.control[CONTROL_JUMP] = app.keyboard[SDL_SCANCODE_I];
	game.config.control[CONTROL_FIRE] = app.keyboard[SDL_SCANCODE_J];

	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		world.bob->activate(1);
		
		app.keyboard[SDL_SCANCODE_SPACE] = 0;
	}

	if (dev.debug)
	{
		doCheatControls();
	}
}

static void doCheatControls(void)
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
}
