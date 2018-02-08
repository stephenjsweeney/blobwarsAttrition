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

#include "entityFactory.h"

static void addEntityDef(char *name, Entity *(*initFunc)(void));
static Entity *initGenericEvilBlob(void);
static Entity *initGenericEyeDroid(void);

static EntityDef head;
static EntityDef *tail;

void initEntityFactory(void)
{
	memset(&head, 0, sizeof(EntityDef));
	tail = &head;
	
	addEntityDef("AquaBlob", initAquaBlob);
	addEntityDef("PistolBlob", initPistolBlob);
	addEntityDef("PistolEyeDroid", initPistolDroid);
	addEntityDef("GenericEvilBlob", initGenericEvilBlob);
	addEntityDef("GenericEyeDroid", initGenericEyeDroid);
	addEntityDef("MachineGunBlob", initMachineGunBlob);
	
	addEntityDef("Bob", initBob);
	addEntityDef("MIA", initMIA);
	
	addEntityDef("Item", initItem);
	addEntityDef("BronzeKey", initBronzeKey);
	addEntityDef("SilverKey", initSilverKey);
	addEntityDef("GoldKey", initGoldKey);
	addEntityDef("GreenKeycard", initGreenKeycard);
	
	addEntityDef("Cell", initCell);
	addEntityDef("Heart", initHeart);
	
	addEntityDef("Exit", initExit);
	addEntityDef("PowerPool", initPowerPool);
	addEntityDef("Teleporter", initTeleporter);
	addEntityDef("PressurePlate", initPressurePlate);
	addEntityDef("InfoPoint", initInfoPoint);
	addEntityDef("PowerPoint", initPowerPoint);
	addEntityDef("PushBlock", initPushBlock);
	addEntityDef("Lift", initLift);
	addEntityDef("Door", initDoor);
	addEntityDef("BronzeDoor", initBronzeDoor);
	addEntityDef("SilverDoor", initSilverDoor);
	addEntityDef("GoldDoor", initGoldDoor);
	addEntityDef("HorizontalDoor", initHorizontalDoor);
}

Entity *createEntity(char *name)
{
	EntityDef *def;
	
	for (def = head.next ; def != NULL ; def = def->next)
	{
		if (strcmp(def->name, name) == 0)
		{
			return def->initFunc();
		}
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such entity definition '%s'", name);
	exit(1);
	
	return NULL;
}

static void addEntityDef(char *name, Entity *(*initFunc)(void))
{
	EntityDef *def;
	
	def = malloc(sizeof(EntityDef));
	memset(def, 0, sizeof(EntityDef));
	tail->next = def;
	tail = def;
	
	STRNCPY(def->name, name, MAX_NAME_LENGTH);
	def->initFunc = initFunc;
}

static Entity *initGenericEvilBlob(void)
{
	int r;
	char name[MAX_NAME_LENGTH];
	strcpy(name, "");
	
	r = rand() % world.numEnemyTypes;
	
	sprintf(name, "%sBlob", world.enemyTypes[r]);
	
	return createEntity(name);
}

static Entity *initGenericEyeDroid(void)
{
	int r;
	char name[MAX_NAME_LENGTH];
	strcpy(name, "");
	
	r = rand() % world.numEnemyTypes;
	
	sprintf(name, "%sEyeDroid", world.enemyTypes[r]);
	
	return createEntity(name);
}
