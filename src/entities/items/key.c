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

Entity *initBronzeKey(Entity *e)
{
	Item *i;
	
	i = (Item*)createItem();
	
	i->type = ET_KEY;

	STRNCPY(i->name, "Bronze Key", MAX_NAME_LENGTH);
	STRNCPY(i->spriteName, "BronzeKey", MAX_NAME_LENGTH);
	
	return (Entity*)i;
}

Entity *initSilverKey(Entity *e)
{
	Item *i;
	
	i = (Item*)createItem();
	
	i->type = ET_KEY;

	STRNCPY(i->name, "Silver Key", MAX_NAME_LENGTH);
	STRNCPY(i->spriteName, "SilverKey", MAX_NAME_LENGTH);
	
	return (Entity*)i;
}

Entity *initGoldKey(Entity *e)
{
	Item *i;
	
	i = (Item*)createItem();
	
	i->type = ET_KEY;

	STRNCPY(i->name, "Gold Key", MAX_NAME_LENGTH);
	STRNCPY(i->spriteName, "GoldKey", MAX_NAME_LENGTH);
	
	return (Entity*)i;
}
