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
static void addKeyToStash(Item *item);
static int sortItems(const void *a, const void *b);

void initGame(void)
{
	memset(&game, 0, sizeof(Game));
	
	game.missionStatusTail = &game.missionStatusHead;
	game.trophyTail = &game.trophyHead;
	
	game.cells = 5;
	game.hearts = 10;

	game.stats[STAT_TIME_PLAYED] = 0;
	
	loadMetaInfo();
}

int getNumItemsCarried(void)
{
	int rtn, i;

	rtn = 0;

	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		if (world.bob->items[i] != NULL)
		{
			rtn++;
		}
	}

	return rtn;
}

int addItem(Item *item, int num)
{
	int i;
	
	if (getNumItemsCarried() < MAX_ITEMS)
	{
		for (i = 0 ; i < MAX_ITEMS ; i++)
		{
			if (item->type == ET_KEY && world.bob->items[i] != NULL && world.bob->items[i]->type == ET_KEY && strcmp(item->name, world.bob->items[i]->name) == 0)
			{
				world.bob->items[i]->value++;
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Inc. %s value (%d)(+1) - Killing", item->name, world.bob->items[i]->value);
				item->alive = ALIVE_DEAD;
				return 1;
			}
			else if (world.bob->items[i] == NULL)
			{
				world.bob->items[i] = item;
				item->canBePickedUp = 0;
				item->flags |= EF_GONE;
				if (item->type == ET_KEY)
				{
					item->value = num;
				}
				
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Added %s (value=%d)", item->name, world.bob->items[i]->value);
				
				return 1;
			}
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
		item = world.bob->items[i];
		
		if (item != NULL && strcmp(item->name, name) == 0)
		{
			return 1;
		}
	}
	
	return 0;
}

Item *getItem(char *name)
{
	int i;
	Item *item;
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		item = world.bob->items[i];
		
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
		item = world.bob->items[i];
		
		if (item != NULL && strcmp(item->name, name) == 0)
		{
			/* only null this, as whether to kill it is handled elsewhere */
			if (item->type != ET_KEY)
			{
				world.bob->items[i] = NULL;
				qsort(world.bob->items, MAX_ITEMS, sizeof(Entity*), sortItems);
				return;
			}
			else
			{
				if (--item->value == 0)
				{
					item->flags &= ~EF_GONE;
					item->alive = ALIVE_DEAD;
					world.bob->items[i] = NULL;
					qsort(world.bob->items, MAX_ITEMS, sizeof(Entity*), sortItems);
					return;
				}
			}
		}
	}
}

void dropCarriedItems(void)
{
	int i;
	Item *item;

	memset(game.keys, 0, sizeof(Tuple) * MAX_KEY_TYPES);
	
	for (i = 0 ; i < MAX_ITEMS ; i++)
	{
		item = world.bob->items[i];

		if (item != NULL)
		{
			if (item->type == ET_KEY)
			{
				addKeyToStash(item);

				item->alive = ALIVE_DEAD;
			}
			else
			{
				item->flags &= ~EF_GONE;
				item->x = world.bob->checkpoints[0].x;
				item->y = world.bob->checkpoints[0].y;
				item->collected = 1;
				item->canBeCarried = 1;
				item->canBePickedUp = 1;
				
				/* items can only be collected if they have a thinktime of 0 */
				item->thinkTime = FPS * 9999;
			}
			
			world.bob->items[i] = NULL;
		}
	}
}

static void addKeyToStash(Item *item)
{
	int i;
	Tuple *t;

	for (i = 0 ; i < MAX_KEY_TYPES ; i++)
	{
		t = &game.keys[i];

		if (t->value.i == 0)
		{
			STRNCPY(t->key, item->sprite[0]->name, MAX_NAME_LENGTH);
			t->value.i = item->value;

			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Added %s (x%d) to stash", t->key, t->value.i);
			
			return;
		}
	}
}

void addKeysFromStash(void)
{
	int i;
	Tuple *t;
	Item *item;

	for (i = 0 ; i < MAX_KEY_TYPES ; i++)
	{
		t = &game.keys[i];

		if (t->value.i > 0)
		{
			item = (Item*)createEntity(t->key);
			self = (Entity*)item;
			item->init();
			item->animate();

			addItem(item, t->value.i);

			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Added %s (x%d) to inventory", item->name, item->value);
		}
	}
}

int getMissionStatus(char *id)
{
	Tuple *t;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, id) == 0)
		{
			return t->value.i;
		}
	}
	
	return MS_LOCKED;
}

static void loadMetaInfo(void)
{
	cJSON *root;
	char *text;
	
	text = readFile("data/meta/meta.json");

	root = cJSON_Parse(text);
	
	game.totalKeys = cJSON_GetObjectItem(root, "totalKeys")->valueint;
	game.totalTargets = cJSON_GetObjectItem(root, "totalTargets")->valueint;
	game.totalMIAs = cJSON_GetObjectItem(root, "totalMIAs")->valueint;
	game.totalHearts = cJSON_GetObjectItem(root, "totalHearts")->valueint;
	game.totalCells = cJSON_GetObjectItem(root, "totalCells")->valueint;
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Meta [keys=%d, targets=%d, mias=%d, hearts=%d, cells=%d]", game.totalKeys, game.totalTargets, game.totalMIAs, game.totalHearts, game.totalCells);
	
	cJSON_Delete(root);
	
	free(text);
}

void loadGame(void)
{
	cJSON *root, *node, *statsJSON;
	char *text, filename[MAX_FILENAME_LENGTH], *statName;
	int i;
	Tuple *t;
	Trophy *trophy;

	sprintf(filename, "%s/game.json", app.saveDir);

	if (fileExists(filename))
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading game '%s' ...", filename);
		
		text = readFile(filename);

		root = cJSON_Parse(text);
		
		game.cells = cJSON_GetObjectItem(root, "cells")->valueint;
		game.hearts = cJSON_GetObjectItem(root, "hearts")->valueint;
		
		statsJSON = cJSON_GetObjectItem(root, "stats");
		
		for (i = 0 ; i < STAT_MAX ; i++)
		{
			statName = getLookupName("STAT_", i);

			if (cJSON_GetObjectItem(statsJSON, statName))
			{
				game.stats[i] = cJSON_GetObjectItem(statsJSON, statName)->valueint;
			}
		}

		i = 0;
		for (node = cJSON_GetObjectItem(root, "keys")->child ; node != NULL ; node = node->next)
		{
			STRNCPY(game.keys[i].key, cJSON_GetObjectItem(node, "type")->valuestring, MAX_NAME_LENGTH);
			game.keys[i].value.i = cJSON_GetObjectItem(node, "num")->valueint;

			i++;
		}

		for (node = cJSON_GetObjectItem(root, "missions")->child ; node != NULL ; node = node->next)
		{
			t = malloc(sizeof(Tuple));
			memset(t, 0, sizeof(Tuple));
			game.missionStatusTail->next = t;
			game.missionStatusTail = t;

			STRNCPY(t->key, cJSON_GetObjectItem(node, "id")->valuestring, MAX_NAME_LENGTH);
			t->value.i = lookup(cJSON_GetObjectItem(node, "status")->valuestring);
		}

		for (node = cJSON_GetObjectItem(root, "trophies")->child ; node != NULL ; node = node->next)
		{
			trophy = getTrophy(cJSON_GetObjectItem(node, "id")->valuestring);
			trophy->awardDate = cJSON_GetObjectItem(node, "awardDate")->valueint;
		}

		cJSON_Delete(root);
		
		free(text);
	}
}

void saveGame(void)
{
	cJSON *root, *statsJSON, *keysJSON, *keyJSON, *missionsJSON, *missionJSON, *trophiesJSON, *trophyJSON;
	char filename[MAX_FILENAME_LENGTH], *out;
	Tuple *t;
	Trophy *trophy;
	int i;

	sprintf(filename, "%s/game.json", app.saveDir);

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Saving game to '%s' ...", filename);

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "cells", game.cells);
	cJSON_AddNumberToObject(root, "hearts", game.hearts);

	statsJSON = cJSON_CreateObject();
	for (i = 0 ; i < STAT_MAX ; i++)
	{
		cJSON_AddNumberToObject(statsJSON, getLookupName("STAT_", i), game.stats[i]);
	}
	cJSON_AddItemToObject(root, "stats", statsJSON);

	keysJSON = cJSON_CreateArray();
	for (i = 0 ; i < MAX_KEY_TYPES ; i++)
	{
		keyJSON = cJSON_CreateObject();
		cJSON_AddStringToObject(keyJSON, "type", game.keys[i].key);
		cJSON_AddNumberToObject(keyJSON, "num", game.keys[i].value.i);

		cJSON_AddItemToArray(keysJSON, keyJSON);
	}
	cJSON_AddItemToObject(root, "keys", keysJSON);

	missionsJSON = cJSON_CreateArray();
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		missionJSON = cJSON_CreateObject();
		cJSON_AddStringToObject(missionJSON, "id", t->key);
		cJSON_AddStringToObject(missionJSON, "status", getLookupName("MS_", t->value.i));

		cJSON_AddItemToArray(missionsJSON, missionJSON);
	}
	cJSON_AddItemToObject(root, "missions", missionsJSON);

	trophiesJSON = cJSON_CreateArray();
	for (trophy = game.trophyHead.next ; trophy != NULL ; trophy = trophy->next)
	{
		trophyJSON = cJSON_CreateObject();
		cJSON_AddStringToObject(trophyJSON, "id", trophy->id);
		cJSON_AddNumberToObject(trophyJSON, "awardDate", trophy->awardDate);

		cJSON_AddItemToArray(trophiesJSON, trophyJSON);
	}
	cJSON_AddItemToObject(root, "trophies", trophiesJSON);

	out = cJSON_Print(root);

	if (!writeFile(filename, out))
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to save game");
	}

	cJSON_Delete(root);
	free(out);
}

/*
 * If the player quits during a mission, we don't want to save the keys, etc. they have picked up,
 * so instead reload the old save and cherry pick the data to keep.
 */
void restoreGameState(void)
{
	cJSON *root, *node, *statsJSON;
	char *text, filename[MAX_FILENAME_LENGTH];
	int i;

	sprintf(filename, "%s/game.json", app.saveDir);

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Restoring game from '%s' ...", filename);
	
	text = readFile(filename);

	root = cJSON_Parse(text);
	
	game.cells = cJSON_GetObjectItem(root, "cells")->valueint;
	game.hearts = cJSON_GetObjectItem(root, "hearts")->valueint;

	statsJSON = cJSON_GetObjectItem(root, "stats");

	game.stats[STAT_KEYS_FOUND] = cJSON_GetObjectItem(statsJSON, "STAT_KEYS_FOUND")->valueint;
	game.stats[STAT_CELLS_FOUND] = cJSON_GetObjectItem(statsJSON, "STAT_CELLS_FOUND")->valueint;
	game.stats[STAT_HEARTS_FOUND] = cJSON_GetObjectItem(statsJSON, "STAT_HEARTS_FOUND")->valueint;
	game.stats[STAT_TARGETS_DEFEATED] = cJSON_GetObjectItem(statsJSON, "STAT_TARGETS_DEFEATED")->valueint;
	game.stats[STAT_MIAS_RESCUED] = cJSON_GetObjectItem(statsJSON, "STAT_MIAS_RESCUED")->valueint;

	i = 0;
	memset(game.keys, 0, sizeof(Tuple) * MAX_KEY_TYPES);
	for (node = cJSON_GetObjectItem(root, "keys")->child ; node != NULL ; node = node->next)
	{
		STRNCPY(game.keys[i].key, cJSON_GetObjectItem(node, "type")->valuestring, MAX_NAME_LENGTH);
		game.keys[i].value.i = cJSON_GetObjectItem(node, "num")->valueint;

		i++;
	}

	cJSON_Delete(root);
	
	free(text);
}

static int sortItems(const void *a, const void *b)
{
	Entity *e1 = *((Entity**)a);
	Entity *e2 = *((Entity**)b);
	
	if (!e1)
    {
        return 1;
    }
    else if (!e2)
	{
		return -1;
	}
    else
	{
		return 0;
	}
}

void destroyGame(void)
{
}
