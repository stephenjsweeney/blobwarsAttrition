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

#include "key.h"

void initBronzeKey(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "Bronze Key", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "BronzeKey", MAX_NAME_LENGTH);
}

void initSilverKey(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "Silver Key", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "SilverKey", MAX_NAME_LENGTH);
}

void initGoldKey(Entity *e)
{
	initItem(e);

	STRNCPY(e->name, "Gold Key", MAX_NAME_LENGTH);
	STRNCPY(e->spriteName, "GoldKey", MAX_NAME_LENGTH);
}
