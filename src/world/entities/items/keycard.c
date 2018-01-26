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

#include "keycard.h"

static void (*itemTouch)(Entity *other);
static void touchWhiteKeycard(Entity *other);

void initRedKeycard(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "Red Keycard", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "RedKeycard", MAX_NAME_LENGTH);
}

void initBlueKeycard(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "Blue Keycard", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "BlueKeycard", MAX_NAME_LENGTH);
}

void initGreenKeycard(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "Green Keycard", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "GreenKeycard", MAX_NAME_LENGTH);
}

void initYellowKeycard(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "Yellow Keycard", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "YellowKeycard", MAX_NAME_LENGTH);
}

void initWhiteKeycard(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "White Keycard", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "WhiteKeycard", MAX_NAME_LENGTH);

	itemTouch = e->touch;

	e->touch = touchWhiteKeycard;
}

static void touchWhiteKeycard(Entity *other)
{
	itemTouch(other);

	if (other == world.bob)
	{
		updateObjective("White Keycard");

		if (self->alive == ALIVE_DEAD)
		{
			teekaExitMission();
		}
	}
}
