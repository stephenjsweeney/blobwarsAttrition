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

static HubMission hubMissionHead;
static HubMission *hubMissionTail;

void initHub(void)
{
	int unlockedMissions;
	HubMission *prevMission, *mission;
	Tuple *t;
	
	memset(&hubMissionHead, 0, sizeof(HubMission));
	
	hubMissionTail = &hubMissionHead;
	
	loadMissions();
	
	if (dev.cheatLevels)
	{
		unlockAllLevels();
	}
	
	unlockedMissions = 0;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (t->value.i != MS_INCOMPLETE)
		{
			unlockedMissions++;
		}
	}
	
	prevMission = &hubMissionHead;
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->unlockCount > unlockedMissions && !dev.cheatLevels)
		{
			if (mission == hubMissionTail)
			{
				hubMissionTail = prevMission;
			}
			
			prevMission->next = mission->next;
			free(mission);
			mission = prevMission;
		}
		else
		{
			mission->status = getMissionStatus(mission->id);

			if (mission->unlockCount == 0 && mission->status == MS_LOCKED)
			{
				mission->status = MS_INCOMPLETE;
				unlockMission(mission->id);
			}
		}
		
		prevMission = mission;
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
	
	prevMission = &hubMissionHead;
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->status == MS_LOCKED || mission->status == MS_COMPLETE)
		{
			if (mission == hubMissionTail)
			{
				hubMissionTail = prevMission;
			}
			
			prevMission->next = mission->next;
			free(mission);
			mission = prevMission;
		}
		
		prevMission = mission;
	}
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

static void loadMissions(void)
{
	cJSON *root, *node;
	char *text;
	HubMission *mission;
	
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
		mission->x = cJSON_GetObjectItem(node, "x")->valueint;
		mission->y = cJSON_GetObjectItem(node, "y")->valueint;
		mission->status = MS_LOCKED;
		mission->unlockCount = cJSON_GetObjectItem(node, "unlockCount")->valueint;
	}
	
	cJSON_Delete(root);
	
	free(text);
}

static void unlockNeighbouringMission(HubMission *sourceMission)
{
	HubMission *mission, *missions[32];
	int i;

	i = 0;
	memset(missions, 0, sizeof(HubMission*) * 32);
	
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

HubMission *getMissionAt(float x, float y)
{
	HubMission *rtn;
	HubMission *mission;
	float distance, dist;
	
	rtn = NULL;
	distance = 32;

	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		dist = getDistance(x, y, mission->x, mission->y);

		if (dist < distance)
		{
			rtn = mission;
			distance = dist;
		}
	}

	return rtn;
}

static int missionComparator(const void *a, const void *b)
{
	HubMission *m1 = *((HubMission**)a);
	HubMission *m2 = *((HubMission**)b);

	return m2->distance - m1->distance;
}
