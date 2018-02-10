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

#include "worldSaver.h"

static void saveEnemyTypes(cJSON *root);
static void saveTriggers(cJSON *root);
static void saveBob(cJSON *root);
static void saveEntities(cJSON *root);
static void saveObjectives(cJSON *root);
static int canPersistEntity(void);

void saveWorld(void)
{
	cJSON *root;
	char filename[MAX_FILENAME_LENGTH], *out;

	sprintf(filename, "%s/%s.json", app.saveDir, world.id);

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Saving world to '%s' ...", filename);

	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "id", world.id);
	cJSON_AddStringToObject(root, "name", world.name);
	cJSON_AddNumberToObject(root, "minEnemySpawnTime", world.minEnemySpawnTime);
	cJSON_AddNumberToObject(root, "maxEnemySpawnTime", world.maxEnemySpawnTime);
	cJSON_AddStringToObject(root, "music", world.music);
	cJSON_AddStringToObject(root, "tileset", world.tileset);
	cJSON_AddStringToObject(root, "background", world.background);
	cJSON_AddNumberToObject(root, "entityCounter", world.entityCounter);

	saveEnemyTypes(root);
	
	saveTriggers(root);
	
	saveBob(root);
	
	saveEntities(root);
	
	saveObjectives(root);

	out = cJSON_Print(root);

	if (!writeFile(filename, out))
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to save world");
	}

	cJSON_Delete(root);
	free(out);
}

static void saveEnemyTypes(cJSON *root)
{
	char enemyTypes[MAX_DESCRIPTION_LENGTH];
	int i;

	strcpy(enemyTypes, "");

	for (i = 0 ; i < world.numEnemyTypes ; i++)
	{
		if (i > 0)
		{
			strcat(enemyTypes, "|");
		}

		strcat(enemyTypes, world.enemyTypes[i]);
	}

	cJSON_AddStringToObject(root, "enemyTypes", enemyTypes);
}

static void saveTriggers(cJSON *root)
{
	cJSON *triggersJSON, *triggerJSON;
	Trigger *t;

	triggersJSON = cJSON_CreateArray();

	for (t = world.triggerHead.next ; t != NULL ; t = t->next)
	{
		triggerJSON = cJSON_CreateObject();

		cJSON_AddStringToObject(triggerJSON, "name", t->name);
		cJSON_AddStringToObject(triggerJSON, "targetNames", t->targetNames);
		cJSON_AddStringToObject(triggerJSON, "message", t->message);
		cJSON_AddNumberToObject(triggerJSON, "x", t->x);
		cJSON_AddNumberToObject(triggerJSON, "y", t->y);
		cJSON_AddNumberToObject(triggerJSON, "w", t->w);
		cJSON_AddNumberToObject(triggerJSON, "h", t->h);

		cJSON_AddItemToArray(triggersJSON, triggerJSON);
	}

	cJSON_AddItemToObject(root, "triggers", triggersJSON);
}

static void saveBob(cJSON *root)
{
	cJSON *bobJSON;

	bobJSON = cJSON_CreateObject();

	world.bob->save(bobJSON);

	cJSON_AddItemToObject(root, "bob", bobJSON);
}

static void saveEntities(cJSON *root)
{
	cJSON *entitiesJSON, *entityJSON;

	entitiesJSON = cJSON_CreateArray();

	for (self = world.entityHead.next ; self != NULL ; self = self->next)
	{
		if (canPersistEntity())
		{
			entityJSON = cJSON_CreateObject();

			cJSON_AddStringToObject(entityJSON, "name", self->name);
			cJSON_AddNumberToObject(entityJSON, "x", (int)self->x);
			cJSON_AddNumberToObject(entityJSON, "y", (int)self->y);

			self->save(entityJSON);

			cJSON_AddItemToArray(entitiesJSON, entityJSON);
		}
	}

	cJSON_AddItemToObject(root, "entities", entitiesJSON);
}

static int canPersistEntity(void)
{
	switch (self->type)
	{
		case ET_NONE:
		case ET_BOB:
		case ET_DECORATION:
		case ET_CONSUMABLE:
			return 0;
			break;
			
		default:
			return self->health > 0 && self->alive == ALIVE_ALIVE;
			break;
	}
	
	return 0;
}

static void saveObjectives(cJSON *root)
{
	Objective *o;
	cJSON *objectivesJSON, *objectiveJSON;

	objectivesJSON = cJSON_CreateArray();

	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		objectiveJSON = cJSON_CreateObject();

		cJSON_AddStringToObject(objectiveJSON, "id", o->id);
		cJSON_AddStringToObject(objectiveJSON, "targetName", o->targetName);
		cJSON_AddStringToObject(objectiveJSON, "description", o->description);
		cJSON_AddNumberToObject(objectiveJSON, "totalValue", o->totalValue);
		cJSON_AddNumberToObject(objectiveJSON, "targetValue", o->targetValue);
		cJSON_AddNumberToObject(objectiveJSON, "currentValue", o->currentValue);
		cJSON_AddNumberToObject(objectiveJSON, "required", o->required);

		cJSON_AddItemToArray(objectivesJSON, objectiveJSON);
	}

	cJSON_AddItemToObject(root, "objectives", objectivesJSON);
}
