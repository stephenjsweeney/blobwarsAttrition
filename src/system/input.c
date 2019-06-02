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

#include "input.h"

void initInput(void)
{
}

static void doKeyDown(SDL_KeyboardEvent *event)
{
	if (event->keysym.scancode < MAX_KEYBOARD_KEYS && event->repeat == 0)
	{
		app.keyboard[event->keysym.scancode] = 1;

		app.lastKeyPressed = event->keysym.scancode;
	}
}

static void doKeyUp(SDL_KeyboardEvent *event)
{
	if (event->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		app.keyboard[event->keysym.scancode] = 0;
	}
}

static void doMouseDown(SDL_MouseButtonEvent *event)
{
	if (event->button >= 0 && event->button < MAX_MOUSE_BUTTONS)
	{
		app.mouse.button[event->button] = 1;
	}
}

static void doMouseUp(SDL_MouseButtonEvent *event)
{
	if (event->button >= 0 && event->button < MAX_MOUSE_BUTTONS)
	{
		app.mouse.button[event->button] = 0;
	}
}

/*
 * Note: the following assumes that SDL_BUTTON_X1 and SDL_BUTTON_X2 are 4 and 5, respectively. They usually are.
 */
static void doMouseWheel(SDL_MouseWheelEvent *event)
{
	if (event->y == -1)
	{
		app.mouse.button[SDL_BUTTON_X1] = 1;
	}

	if (event->y == 1)
	{
		app.mouse.button[SDL_BUTTON_X2] = 1;
	}
}

static void doMouseMotion(SDL_MouseMotionEvent *event)
{
	app.mouse.dx = event->xrel;
	app.mouse.dy = event->yrel;
}

static void doButtonDown(SDL_JoyButtonEvent *event)
{
	if (event->state == SDL_PRESSED)
	{
		app.joypadButton[event->button] = 1;

		app.lastButtonPressed = event->button;
	}
}

static void doButtonUp(SDL_JoyButtonEvent *event)
{
	if (event->state == SDL_RELEASED)
	{
		app.joypadButton[event->button] = 0;
	}
}

static void doJoyAxis(SDL_JoyAxisEvent *event)
{
	if (event->axis < JOYPAD_AXIS_MAX)
	{
		app.joypadAxis[event->axis] = event->value;
	}
}

void clearInput(void)
{
	SDL_Event event;

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	memset(&app.mouse, 0, sizeof(Mouse));
	memset(&app.joypadButton, 0, sizeof(int) * SDL_CONTROLLER_BUTTON_MAX);

	while (SDL_PollEvent(&event))
	{
	}
}

void handleInput(void)
{
	SDL_Event event;

	app.mouse.dx = 0;
	app.mouse.dy = 0;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_MOUSEMOTION:
				doMouseMotion(&event.motion);
				break;

			case SDL_MOUSEWHEEL:
				doMouseWheel(&event.wheel);
				break;

			case SDL_MOUSEBUTTONDOWN:
				doMouseDown(&event.button);
				break;

			case SDL_MOUSEBUTTONUP:
				doMouseUp(&event.button);
				break;

			case SDL_KEYDOWN:
				doKeyDown(&event.key);
				break;

			case SDL_KEYUP:
				doKeyUp(&event.key);
				break;

			case SDL_JOYBUTTONDOWN:
				doButtonDown(&event.jbutton);
				break;

			case SDL_JOYBUTTONUP:
				doButtonUp(&event.jbutton);
				break;

			case SDL_JOYAXISMOTION:
				doJoyAxis(&event.jaxis);
				break;

			case SDL_QUIT:
				exit(0);
				break;
		}
	}

	SDL_GetMouseState(&app.mouse.x, &app.mouse.y);
}
