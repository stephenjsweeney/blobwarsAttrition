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

static void setSparkleColor(Trophy *t);
static void loadTrophyData(void);
static void resetAlert(void);
static void nextAlert(void);

static int numTrophies;
static SDL_Rect alertRect;
static int alertTimer;
static Trophy *alertTrophy;
static float sparkleAngle;
static Atlas *trophyIcons[TROPHY_MAX];
static Atlas *sparkle;
static Atlas *alertSphere;
static Texture *atlasTexture;
static int awarded;

void initTrophies(void)
{
	atlasTexture = getTexture("gfx/atlas/atlas.png");

	trophyIcons[TROPHY_BRONZE] = getImageFromAtlas("gfx/trophies/bronze.png");
	trophyIcons[TROPHY_SILVER] = getImageFromAtlas("gfx/trophies/silver.png");
	trophyIcons[TROPHY_GOLD] = getImageFromAtlas("gfx/trophies/gold.png");
	trophyIcons[TROPHY_PLATINUM] = getImageFromAtlas("gfx/trophies/platinum.png");
	trophyIcons[TROPHY_UNEARNED] = getImageFromAtlas("gfx/trophies/unearned.png");
	sparkle = getImageFromAtlas("gfx/trophies/sparkle.png");
	alertSphere = getImageFromAtlas("gfx/trophies/alertSphere.png");
	
	alertRect.h = 90;
	alertRect.y = 10;

	numTrophies = 0;

	awarded = 0;
	
	sparkleAngle = 0;

	loadTrophyData();
	
	resetAlert();
}

void awardTrophy(char *id)
{
	Trophy *t;
	int numRemaining;

	numRemaining = 0;

	for (t = game.trophyHead.next ; t != NULL ; t = t->next)
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

Trophy *getTrophy(char *id)
{
	Trophy *t;

	for (t = game.trophyHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->id, id) == 0)
		{
			return t;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such trophy '%s'", id);
	exit(1);
}

void awardTrophies(void)
{
	int val;
	Trophy *t;

	for (t = game.trophyHead.next ; t != NULL ; t = t->next)
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
			saveScreenshot(alertTrophy->id);
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

	for (t = game.trophyHead.next ; t != NULL ; t = t->next)
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
	int x, y;
	
	if (alertTrophy)
	{
		drawRect(alertRect.x, alertRect.y, alertRect.w, alertRect.h, 0, 0, 0, 255);
		drawOutlineRect(alertRect.x, alertRect.y, alertRect.w, alertRect.h, 255, 255, 255, 255);

		drawText(alertRect.x + 15, alertRect.y + 5, 30, TA_LEFT, colors.white, alertTrophy->title);
		drawText(alertRect.x + 15, alertRect.y + 45, 20, TA_LEFT, colors.white, alertTrophy->description);

		x = alertRect.x + alertRect.w - 72;
		y = alertRect.y + 20;

		setSparkleColor(alertTrophy);
		blitRect(atlasTexture->texture, x + 24, y + 24, &alertSphere->rect, 1);
		blitRectRotated(atlasTexture->texture, x + 24, y + 24, &sparkle->rect, sparkleAngle);
		blitRectRotated(atlasTexture->texture, x + 24, y + 24, &sparkle->rect, -sparkleAngle);
		blitRectScaled(atlasTexture->texture, x, y, 48, 48, &trophyIcons[alertTrophy->value]->rect, 0);

		SDL_SetTextureColorMod(atlasTexture->texture, 255, 255, 255);
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
		game.trophyTail->next = t;
		game.trophyTail = t;

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
 
static void setSparkleColor(Trophy *t)
{
	switch (t->value)
	{
		case TROPHY_BRONZE:
			SDL_SetTextureColorMod(atlasTexture->texture, 255, 128, 0);
			break;
		
		case TROPHY_SILVER:
			SDL_SetTextureColorMod(atlasTexture->texture, 192, 192, 192);
			break;
		
		case TROPHY_GOLD:
			SDL_SetTextureColorMod(atlasTexture->texture, 255, 255, 0);
			break;
		
		case TROPHY_PLATINUM:
			SDL_SetTextureColorMod(atlasTexture->texture, 0, 128, 255);
			break;

		default:
			break;
	}
}
