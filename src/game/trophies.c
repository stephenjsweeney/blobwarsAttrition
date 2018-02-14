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

#include "trophies.h"

static void loadTrophyData(void);
static void resetAlert(void);
static void nextAlert(void);

static Trophy trophyHead, *trophyTail;
static int numTrophies;
static SDL_Rect alertRect;
static int alertTimer;
static Trophy *alertTrophy;
static float sparkleAngle;
/*
static SDL_Texture *trophyIcons[TROPHY_MAX];
static SDL_Texture *sparkle;
static SDL_Texture *alertSphere;
*/
static int awarded;

void initTrophies(void)
{
	memset(&trophyHead, 0, sizeof(Trophy));
	trophyTail = &trophyHead;

	numTrophies = 0;

	awarded = 0;

	alertTimer = 0;
	
	sparkleAngle = 0;

	loadTrophyData();
}

void awardTrophy(char *id)
{
	Trophy *t;
	int numRemaining;

	numRemaining = 0;

	for (t = trophyHead.next ; t != NULL ; t = t->next)
	{
		if (t->awardDate == 0 && strcmp(t->id, id) == 0)
		{
			t->awardDate = time(NULL);
			t->notify = SDL_GetTicks();

			SDL_Delay(1);

			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Awarding trophy '%s'", t->id);
		}

		if (t->awardDate == 0)
		{
			numRemaining++;
		}
	}

	/* the Platinum will always be the last trophy to unlock */
	if (numRemaining == 1)
	{
		awardTrophy("PLATINUM");
	}
}

void awardTrophies(void)
{
	int val;
	Trophy *t;

	for (t = trophyHead.next ; t != NULL ; t = t->next)
	{
		if (t->awardDate == 0 && t->statValue != 0)
		{
			val = game.stats[t->stat];

			if (val >= t->statValue)
			{
				awardTrophy(t->id);
			}
		}
	}
}

void doTrophyAlerts(void)
{
	if (!alertTrophy)
	{
		nextAlert();
	}
	else if (alertTrophy)
	{
		alertRect.x = MIN(alertRect.x + 24, -1);

		if (alertRect.x > -150)
		{
			alertTimer--;
		}

		if (alertTimer <= 0)
		{
			alertTrophy->notify = 0;
			resetAlert();
		}
	}
	
	sparkleAngle = mod(sparkleAngle + 0.25, 360);
}

static void nextAlert(void)
{
	int w, h;
	Trophy *t;

	for (t = trophyHead.next ; t != NULL ; t = t->next)
	{
		if (t->notify)
		{
			if (!alertTrophy || t->notify < alertTrophy->notify)
			{
				alertTrophy = t;
			}
		}
	}
	
	if (alertTrophy)
	{
		playSound(SND_TROPHY, -1);
		
		textSize(alertTrophy->title, 30, &alertRect.w, &h);
		textSize(alertTrophy->description, 20, &w, &h);
		
		alertRect.w = MAX(alertRect.w, w);
		alertRect.w = MAX(400, alertRect.w);
		alertRect.w += 125;
		alertRect.x = -alertRect.w;
	}
}

static void resetAlert(void)
{
	alertTimer = FPS * 3;
	alertTrophy = NULL;
}

void drawTrophyAlert(void)
{
	/*int x, y;*/
	
	if (alertTrophy)
	{
		SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(app.renderer, &alertRect);

		SDL_SetRenderDrawColor(app.renderer, 64, 64, 64, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(app.renderer, &alertRect);

		drawText(alertRect.x + 15, alertRect.y + 5, 30, TA_LEFT, colors.white, alertTrophy->title);
		drawText(alertRect.x + 15, alertRect.y + 45, 20, TA_LEFT, colors.white, alertTrophy->description);

		/*
		x = alertRect.x alertRect.w - 72;
		y = alertRect.y 20;

		setSparkleColor(alertTrophy);
		blit(alertSphere, x 24, y 24, 1);
		blitRotated(sparkle, x 24, y 24, sparkleAngle);
		blitRotated(sparkle, x 24, y 24, -sparkleAngle);
		blitScaled(trophyIcons[alertTrophy->value], x, y, 48, 48, 0);
		*/
	}
}

static void loadTrophyData(void)
{
	cJSON *root, *node;
	char *text;
	Trophy *t;
	char *filename;

	filename = "data/misc/trophies.json";

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	text = readFile(filename);
	root = cJSON_Parse(text);

	for (node = root->child ; node != NULL ; node = node->next)
	{
		t = malloc(sizeof(Trophy));
		memset(t, 0, sizeof(Trophy));
		trophyTail->next = t;
		trophyTail = t;

		STRNCPY(t->id, cJSON_GetObjectItem(node, "id")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(t->title, _(cJSON_GetObjectItem(node, "title")->valuestring), MAX_DESCRIPTION_LENGTH);
		STRNCPY(t->description, _(cJSON_GetObjectItem(node, "description")->valuestring), MAX_DESCRIPTION_LENGTH);
		t->value = lookup(cJSON_GetObjectItem(node, "value")->valuestring);

		if (cJSON_GetObjectItem(node, "hidden"))
		{
			t->hidden = cJSON_GetObjectItem(node, "hidden")->valueint;
		}
		
		t->stat = -1;
		
		if (cJSON_GetObjectItem(node, "stat"))
		{
			t->stat = lookup(cJSON_GetObjectItem(node, "stat")->valuestring);
			t->statValue = cJSON_GetObjectItem(node, "statValue")->valueint;
		}
	}

	cJSON_Delete(root);
	free(text);
}
 
