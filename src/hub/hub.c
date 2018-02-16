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

#include "hub.h"

static void unlockAllLevels(void);
static void unlockMission(char *id);
static int getMissionStatus(char *id);
static void loadMissions(void);
static void unlockNeighbouringMission(HubMission *sourceMission);
static int missionComparator(const void *a, const void *b);
static HubMission *getMissionAt(int x, int y);
static void logic(void);
static void draw(void);

static HubMission hubMissionHead;
static HubMission *hubMissionTail;
static HubMission *selectedMission;
static Atlas *worldMap;
static Atlas *alert;
static Atlas *clouds;
static Texture *atlasTexture;
static int completedMissions;
static int numMissions;
static float blipSize;
static float blipValue;

void initHub(void)
{
	int unlockedMissions, unlockTeeka;
	HubMission *mission, *teeka;
	Tuple *t;
	
	memset(&hubMissionHead, 0, sizeof(HubMission));
	hubMissionTail = &hubMissionHead;
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	worldMap = getImageFromAtlas("gfx/hub/worldMap.jpg");
	alert = getImageFromAtlas("gfx/hub/alert.png");
	clouds = getImageFromAtlas("gfx/hub/clouds.png");
	
	loadMissions();
	
	if (dev.cheatLevels)
	{
		unlockAllLevels();
	}
	
	numMissions = 0;
	
	unlockedMissions = 0;
	
	completedMissions = 0;
	
	unlockTeeka = 1;
	
	blipValue = 0;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (t->value.i != MS_INCOMPLETE)
		{
			unlockedMissions++;
		}
		
		if (t->value.i == MS_COMPLETE)
		{
			completedMissions++;
		}
	}
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->unlockCount <= unlockedMissions || dev.cheatLevels)
		{
			unlockMission(mission->id);
		}
		
		mission->status = getMissionStatus(mission->id);
		
		if (strcmp(mission->id, "teeka") == 0)
		{
			teeka = mission;
		}
		else if (mission->status == MS_LOCKED)
		{
			unlockTeeka = 0;
		}
		
		numMissions++;
	}
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->status == MS_COMPLETE || mission->status == MS_MISSING_HEART_CELL || mission->status == MS_PARTIAL)
		{
			unlockNeighbouringMission(mission);
		}

		if (mission->status == MS_MISSING_HEART_CELL)
		{
			STRNCPY(mission->description, _("All objectives for this misson have been completed. However, there is a Cell or a Heart left to find. See if you can locate it."), MAX_DESCRIPTION_LENGTH);
		}
	}
	
	if (unlockTeeka)
	{
		teeka->status = MS_INCOMPLETE;
	}
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	app.hideMouse = 0;
}

static void logic(void)
{
	HubMission *m;
	
	blipValue += 0.1;
	
	blipSize = 64 + (sin(blipValue) * 16);
	
	m = NULL;
	
	if (app.mouse.button[SDL_BUTTON_LEFT])
	{
		m = getMissionAt(app.mouse.x, app.mouse.y);
		
		if (m != NULL)
		{
			selectedMission = m;
			app.mouse.button[SDL_BUTTON_LEFT] = 0;
		}
	}
}

static void draw(void)
{
	HubMission *mission;
	
	blitRectScaled(atlasTexture->texture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &worldMap->rect, 0);
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		switch (mission->status)
		{
			case MS_INCOMPLETE:
				SDL_SetTextureColorMod(atlasTexture->texture, 255, 0, 0);
				blitRectScaled(atlasTexture->texture, mission->x, mission->y, blipSize, blipSize, &alert->rect, 1);
				drawText(mission->x, mission->y - 32, 18, TA_CENTER, colors.white, mission->name);
				break;
				
			case MS_PARTIAL:
			case MS_MISSING_HEART_CELL:
				SDL_SetTextureColorMod(atlasTexture->texture, 255, 255, 0);
				blitRectScaled(atlasTexture->texture, mission->x, mission->y, blipSize, blipSize, &alert->rect, 1);
				drawText(mission->x, mission->y - 32, 18, TA_CENTER, colors.white, mission->name);
				break;
		}
	}
	
	SDL_SetTextureColorMod(atlasTexture->texture, 255, 255, 255);
	
	drawRect(0, 0, SCREEN_WIDTH, 32, 0, 0, 0, 192);
	
	drawText(10, 5, 18, TA_LEFT, colors.white, "Missions : %d / %d", completedMissions, numMissions);
	
	drawText(210, 5, 18, TA_LEFT, colors.white, "MIAs : %d / %d", 0, game.totalMIAs);
	
	drawText(410, 5, 18, TA_LEFT, colors.white, "Targets : %d / %d", 0, game.totalTargets);
	
	drawText(610, 5, 18, TA_LEFT, colors.white, "Keys : %d / %d", 0, game.totalKeys);
	
	drawText(810, 5, 18, TA_LEFT, colors.white, "Hearts : %d / %d", 0, game.totalHearts);
	
	drawText(1010, 5, 18, TA_LEFT, colors.white, "Cells : %d / %d", 0, game.totalCells);
}

static int getMissionStatus(char *id)
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

static void unlockMission(char *id)
{
	Tuple *t;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, id) == 0)
		{
			if (t->value.i == MS_LOCKED)
			{
				t->value.i = MS_INCOMPLETE;
				
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Unlocked mission %s", id);
			}
			
			return;
		}
	}
	
	t = malloc(sizeof(Tuple));
	memset(t, 0, sizeof(Tuple));
	game.missionStatusTail->next = t;
	game.missionStatusTail = t;
	
	STRNCPY(t->key, id, MAX_NAME_LENGTH);
	t->value.i = MS_INCOMPLETE;
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Unlocked mission %s", id);
}

static void unlockAllLevels(void)
{
	HubMission *mission;
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->status == MS_LOCKED || mission->status == MS_INCOMPLETE)
		{
			mission->status = MS_INCOMPLETE;
			unlockMission(mission->id);
		}
	}
}

static void unlockNeighbouringMission(HubMission *sourceMission)
{
	HubMission *mission, *missions[numMissions];
	int i;

	i = 0;
	memset(missions, 0, sizeof(HubMission*) * numMissions);
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		mission->distance = 99999;

		if (mission->status == MS_COMPLETE || mission->status == MS_MISSING_HEART_CELL || mission->status == MS_PARTIAL)
		{
			continue;
		}

		mission->distance = getDistance(mission->x, mission->y, sourceMission->x, sourceMission->y);
		
		missions[i++] = mission;
	}
	
	qsort(missions, i, sizeof(HubMission*), missionComparator);
	
	mission = missions[0];
	
	if (mission != NULL)
	{
		if (mission->status == MS_LOCKED || mission->status == MS_INCOMPLETE)
		{
			mission->status = MS_INCOMPLETE;
			unlockMission(mission->id);
		}

		mission = missions[1];
		
		if (mission != NULL)
		{
			if (mission->status == MS_LOCKED || mission->status == MS_INCOMPLETE)
			{
				mission->status = MS_INCOMPLETE;
				unlockMission(mission->id);
			}
		}
	}
}

HubMission *getMissionAt(int x, int y)
{
	HubMission *rtn;
	HubMission *mission;
	float distance, dist;
	
	rtn = NULL;
	distance = 32;

	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->status == MS_INCOMPLETE || mission->status == MS_MISSING_HEART_CELL || mission->status == MS_PARTIAL)
		{
			dist = getDistance(x, y, mission->x, mission->y);
			
			if (dist < distance)
			{
				rtn = mission;
				distance = dist;
			}
		}
	}

	return rtn;
}

static void loadMissions(void)
{
	cJSON *root, *node;
	char *text;
	HubMission *mission;
	double ratioX, ratioY;
	
	/* the original Attrition is based on 800x600, so multiply up */
	ratioX = SCREEN_WIDTH / 800.0;
	ratioY = SCREEN_HEIGHT / 600.0;
	
	text = readFile("data/hub/missions.json");

	root = cJSON_Parse(text);
	
	for (node = cJSON_GetObjectItem(root, "missions")->child ; node != NULL ; node = node->next)
	{
		mission = malloc(sizeof(HubMission));
		memset(mission, 0, sizeof(HubMission));
		hubMissionTail->next = mission;
		hubMissionTail = mission;
		
		STRNCPY(mission->id, cJSON_GetObjectItem(node, "id")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(mission->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(mission->description, cJSON_GetObjectItem(node, "description")->valuestring, MAX_DESCRIPTION_LENGTH);
		mission->status = MS_LOCKED;
		mission->unlockCount = cJSON_GetObjectItem(node, "unlockCount")->valueint;
		
		mission->x = cJSON_GetObjectItem(node, "x")->valuedouble * ratioX;
		mission->y = cJSON_GetObjectItem(node, "y")->valuedouble * ratioY;
	}
	
	cJSON_Delete(root);
	
	free(text);
}

static int missionComparator(const void *a, const void *b)
{
	HubMission *m1 = *((HubMission**)a);
	HubMission *m2 = *((HubMission**)b);

	return m2->distance - m1->distance;
}

void destroyHub(void)
{
	
}
