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

#include "lookup.h"

static Lookup head;
static Lookup *tail;

static void addLookup(const char *name, long value); 

void initLookups(void)
{
	memset(&head, 0, sizeof(Lookup));
	tail = &head;
	
	addLookup("FACING_LEFT", FACING_LEFT);
	addLookup("FACING_RIGHT", FACING_RIGHT);
	
	addLookup("LIFT_GOTO_FINISH", LIFT_GOTO_FINISH);
	addLookup("LIFT_GOTO_START", LIFT_GOTO_START);
	
	addLookup("DOOR_OPEN", DOOR_OPEN);
	addLookup("DOOR_CLOSED", DOOR_CLOSED);
	
	addLookup("WPN_PLASMA", WPN_PLASMA);
	addLookup("WPN_SPREAD", WPN_SPREAD);
	
	addLookup("CONTROL_LEFT", CONTROL_LEFT);
	addLookup("CONTROL_RIGHT", CONTROL_RIGHT);
	addLookup("CONTROL_UP", CONTROL_UP);
	addLookup("CONTROL_DOWN", CONTROL_DOWN);
	addLookup("CONTROL_FIRE", CONTROL_FIRE);
	addLookup("CONTROL_JUMP", CONTROL_JUMP);
	addLookup("CONTROL_JETPACK", CONTROL_JETPACK);
	addLookup("CONTROL_PAUSE", CONTROL_PAUSE);
	addLookup("CONTROL_MAP", CONTROL_MAP);
	
	addLookup("WT_BUTTON", WT_BUTTON);
	addLookup("WT_SPINNER", WT_SPINNER);
	addLookup("WT_SLIDER", WT_SLIDER);
	addLookup("WT_INPUT", WT_INPUT);
	
	addLookup("MS_LOCKED", MS_LOCKED);
	addLookup("MS_INCOMPLETE", MS_INCOMPLETE);
	addLookup("MS_PARTIAL", MS_PARTIAL);
	addLookup("MS_MISSING_HEART_CELL", MS_MISSING_HEART_CELL);
	addLookup("MS_COMPLETE", MS_COMPLETE);
	
	addLookup("TROPHY_BRONZE", TROPHY_BRONZE);
	addLookup("TROPHY_SILVER", TROPHY_SILVER);
	addLookup("TROPHY_GOLD", TROPHY_GOLD);
	addLookup("TROPHY_PLATINUM", TROPHY_PLATINUM);
	
	addLookup("STAT_MISSIONS_COMPLETE", STAT_MISSIONS_COMPLETE);
	addLookup("STAT_KEYS_FOUND", STAT_KEYS_FOUND);
	addLookup("STAT_CELLS_FOUND", STAT_CELLS_FOUND);
	addLookup("STAT_HEARTS_FOUND", STAT_HEARTS_FOUND);
	addLookup("STAT_TARGETS_DEFEATED", STAT_TARGETS_DEFEATED);
	addLookup("STAT_MIAS_RESCUED", STAT_MIAS_RESCUED);
	addLookup("STAT_DEATHS", STAT_DEATHS);
	addLookup("STAT_SHOTS_FIRED", STAT_SHOTS_FIRED);
	addLookup("STAT_SHOTS_HIT", STAT_SHOTS_HIT);
	addLookup("STAT_EYE_DROID_EXPLOSION_KILLS", STAT_EYE_DROID_EXPLOSION_KILLS);
	addLookup("STAT_FLY_TIME", STAT_FLY_TIME);
	addLookup("STAT_SWIM_TIME", STAT_SWIM_TIME);
	addLookup("STAT_CHERRIES_PICKED_UP", STAT_CHERRIES_PICKED_UP);
	addLookup("STAT_BATTERIES_PICKED_UP", STAT_BATTERIES_PICKED_UP);
	addLookup("STAT_WEAPONS_PICKED_UP", STAT_WEAPONS_PICKED_UP);
	addLookup("STAT_ENEMIES_KILLED", STAT_ENEMIES_KILLED);
	addLookup("STAT_MISSIONS_PLAYED", STAT_MISSIONS_PLAYED);
	addLookup("STAT_SHOT_ACCURACY", STAT_SHOT_ACCURACY);
	addLookup("STAT_PERCENT_COMPLETE", STAT_PERCENT_COMPLETE);
	addLookup("STAT_TIME_PLAYED", STAT_TIME_PLAYED);
}

static void addLookup(const char *name, long value)
{
	Lookup *lookup = malloc(sizeof(Lookup));
	memset(lookup, 0, sizeof(Lookup));

	STRNCPY(lookup->name, name, MAX_NAME_LENGTH);
	lookup->value = value;

	tail->next = lookup;
	tail = lookup;
}

long lookup(const char *name)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (strcmp(l->name, name) == 0)
		{
			return l->value;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value '%s'", name);
	exit(1);

	return 0;
}

char *getLookupName(const char *prefix, long num)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (l->value == num && strncmp(prefix, l->name, strlen(prefix)) == 0)
		{
			return l->name;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value %ld, prefix=%s", num, prefix);
	exit(1);

	return "";
}

const char *getFlagValues(const char *prefix, long flags)
{
	static char flagStr[MAX_DESCRIPTION_LENGTH];
	int requirePlus;
	Lookup *l;

	memset(flagStr, '\0', MAX_DESCRIPTION_LENGTH);

	requirePlus = 0;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (flags & l->value && strncmp(prefix, l->name, strlen(prefix)) == 0)
		{
			if (requirePlus)
			{
				strcat(flagStr, "+");
			}

			strcat(flagStr, l->name);

			requirePlus = 1;
		}
	}

	return flagStr;
}

void destroyLookups(void)
{
	Lookup *l;

	while (head.next)
	{
		l = head.next;
		head.next = l->next;
		free(l);
	}
}
