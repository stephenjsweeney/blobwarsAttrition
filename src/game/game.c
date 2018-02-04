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

static void loadMetaInfo(void);

void initGame(void)
{
	memset(&game, 0, sizeof(Game));
	
	game.missionStatusTail = &game.missionStatusHead;
	
	game.cells = 5;
	game.hearts = 10;

	game.timePlayed = 0;
	
	loadMetaInfo();
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

int addItem(Entity *item)
{
	int i;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		if (world.bob->items[i] == NULL)
		{
			world.bob->items[i] = item;
			item->flags |= EF_GONE;
			return 1;
		}
	}
	
	return 0;
}

int hasItem(char *name)
{
	int i;
	Item *item;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		item = (Item*)world.bob->items[i];
		
		if (item != NULL && strcmp(item->name, name) == 0)
		{
			return 1;
		}
	}
	
	return 0;
}

Entity *getItem(char *name)
{
	int i;
	Item *item;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		item = (Item*)world.bob->items[i];
		
		if (item != NULL && strcmp(item->name, name) == 0)
		{
			return world.bob->items[i];
		}
	}
	
	return NULL;
}

void removeItem(char *name)
{
	int i;
	Item *item;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		item = (Item*)world.bob->items[i];
		
		if (item != NULL && strcmp(item->name, name) == 0)
		{
			item->flags &= ~EF_GONE;
			item->alive = ALIVE_DEAD;
		}
	}
}

void dropCarriedItems(void)
{
	int i;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		
	}
}

static void loadMetaInfo(void)
{
	cJSON *root;
	char *text;
	int i;
	
	text = readFile("data/meta/meta.json");

	root = cJSON_Parse(text);
	
	game.totalKeys = cJSON_GetObjectItem(root, "totalKeys")->valueint;
	game.totalTargets = cJSON_GetObjectItem(root, "totalTargets")->valueint;
	game.totalMIAs = cJSON_GetObjectItem(root, "totalMIAs")->valueint;
	game.totalHearts = cJSON_GetObjectItem(root, "totalHearts")->valueint;
	game.totalCells = cJSON_GetObjectItem(root, "totalCells")->valueint;
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Meta [keys=%d, targets=%d, mias=%d, hearts=%d, cells=%d]", game.totalKeys, game.totalTargets, game.totalMIAs, game.totalHearts, game.totalCells);
	
	game.mias = malloc(sizeof(char*) * game.totalMIAs);
	for (i = 0 ; i < game.totalMIAs ; i++)
	{
		game.mias[i] = malloc(MAX_NAME_LENGTH);
	}
	
	game.targets = malloc(sizeof(char*) * game.totalTargets);
	for (i = 0 ; i < game.totalTargets ; i++)
	{
		game.targets[i] = malloc(MAX_NAME_LENGTH);
	}
	
	cJSON_Delete(root);
	
	free(text);
}

void destroyGame(void)
{
}
