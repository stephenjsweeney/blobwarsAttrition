/*
Copyright (C) 2015-2017 Parallel Realities

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

#include "controls.h"

void initControls(void)
{
	int i;

	app.config.keyControls[CONTROL_LEFT] = SDL_SCANCODE_A;
	app.config.keyControls[CONTROL_RIGHT] = SDL_SCANCODE_D;
	app.config.keyControls[CONTROL_UP] = SDL_SCANCODE_W;
	app.config.keyControls[CONTROL_DOWN] = SDL_SCANCODE_S;
	app.config.keyControls[CONTROL_JUMP] = SDL_SCANCODE_I;
	app.config.keyControls[CONTROL_FIRE] = SDL_SCANCODE_J;
	app.config.keyControls[CONTROL_JETPACK] = SDL_SCANCODE_L;
	app.config.keyControls[CONTROL_PAUSE] = SDL_SCANCODE_TAB;

	/* can't use memset here, as it doesn't work */
	for (i = 0 ; i < CONTROL_MAX ; i++)
	{
		app.config.joypadControls[i] = -1;
	}
	
	app.config.joypadControls[CONTROL_JUMP] = 1;
	app.config.joypadControls[CONTROL_FIRE] = 3;
	app.config.joypadControls[CONTROL_JETPACK] = 2;
}

int isControl(int type)
{
	int key, btn;
	
	key = app.config.keyControls[type];
	btn = app.config.joypadControls[type];
	
	if (type == CONTROL_LEFT && app.joypadAxis[JOYPAD_AXIS_X] <= -16384)
	{
		return 1;
	}
	
	if (type == CONTROL_RIGHT && app.joypadAxis[JOYPAD_AXIS_X] >= 16384)
	{
		return 1;
	}
	
	if (type == CONTROL_UP && app.joypadAxis[JOYPAD_AXIS_Y] <= -16384)
	{
		return 1;
	}
	
	if (type == CONTROL_DOWN && app.joypadAxis[JOYPAD_AXIS_Y] >= 16384)
	{
		return 1;
	}
	
	return ((key != 0 && app.keyboard[key]) || (btn != -1 && app.joypadButton[btn]));
}

int isAcceptControl(void)
{
	return (app.keyboard[SDL_SCANCODE_SPACE] ||app.keyboard[SDL_SCANCODE_RETURN] || isControl(CONTROL_FIRE));
}

void clearControl(int type)
{
	int key = app.config.keyControls[type];
	int btn = app.config.joypadControls[type];
	
	if (key != 0)
	{
		app.keyboard[key] = 0;
	}
	
	if (btn != 0)
	{
		app.joypadButton[btn] = 0;
	}
}

void clearControls(void)
{
	memset(&app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	memset(&app.joypadButton, 0, sizeof(int) * SDL_CONTROLLER_BUTTON_MAX);
}
