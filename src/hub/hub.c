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

static void unlockAllMissions(void);
static void unlockMission(char *id);
static int requiredMissionUnlocked(char *id);
static void loadMissions(void);
static HubMission *getMissionAt(int x, int y);
static void drawMissions(void);
static void drawInfoBar(void);
static void drawMissionInfo(void);
static void logic(void);
static void draw(void);
static void startMission(void);
static void cancel(void);
static void options(void);
static void stats(void);
static void trophies(void);
static void quit(void);
static void returnFromTrophyStats(void);
static void doCursor(void);
static void doMissionSelect(void);
static void doMissionInfo(void);
static void drawHudWidgets(void);
static void awardMissionTrophies(void);
static void returnFromOptions(void);
void destroyHub(void);

static HubMission hubMissionHead;
static HubMission *hubMissionTail;
static HubMission *selectedMission;
static Atlas *worldMap;
static Atlas *alert;
static Atlas *clouds;
static Sprite *cursorSpr;
static Sprite *keySprites[MAX_KEY_TYPES];
static Texture *atlasTexture;
static int unlockedMissions;
static PointF cursor;
static float blipSize;
static float blipValue;
static int showing;
static PointF cloudPos;

void initHub(void)
{
	int unlockTeeka, i;
	HubMission *mission, *teeka;
	Tuple *t;
	
	startSectionTransition();
	
	memset(&hubMissionHead, 0, sizeof(HubMission));
	hubMissionTail = &hubMissionHead;
	
	memset(&keySprites, 0, sizeof(Sprite*) * MAX_KEY_TYPES);
	
	selectedMission = NULL;
	
	loadMusic("music/61321__mansardian__news-background.ogg");
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	worldMap = getImageFromAtlas("gfx/hub/worldMap.jpg");
	alert = getImageFromAtlas("gfx/hub/alert.png");
	clouds = getImageFromAtlas("gfx/hub/clouds.png");
	cursorSpr = getSprite("Cursor");
	
	for (i = 0 ; i < MAX_KEY_TYPES ; i++)
	{
		if (game.keys[i].value.i > 0)
		{
			keySprites[i] = getSprite(game.keys[i].key);
		}
	}

	cursor.x = SCREEN_WIDTH / 2;
	cursor.y = SCREEN_HEIGHT / 2;

	getWidget("startMission", "mission")->action = startMission;
	getWidget("cancel", "mission")->action = cancel;
	getWidget("options", "hub")->action = options;
	getWidget("stats", "hub")->action = stats;
	getWidget("trophies", "hub")->action = trophies;
	getWidget("quit", "hub")->action = quit;
	
	getWidget("ok", "stats")->action = returnFromTrophyStats;
	getWidget("ok", "trophies")->action = returnFromTrophyStats;
	
	loadMissions();
	
	if (dev.cheatLevels)
	{
		unlockAllMissions();
	}
	
	game.totalMissions = 0;
	
	unlockedMissions = 0;
	
	game.stats[STAT_MISSIONS_COMPLETE] = 0;
	
	unlockTeeka = 1;
	
	blipValue = 0;
	
	showing = SHOW_NONE;
	
	cursor.x = SCREEN_WIDTH / 2;
	cursor.y = SCREEN_HEIGHT / 2;
	SDL_WarpMouseInWindow(app.window, cursor.x * app.scaleX, cursor.y * app.scaleY);

	game.isComplete = 0;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (t->value.i != MS_INCOMPLETE)
		{
			unlockedMissions++;
		}
		
		if (t->value.i == MS_COMPLETE)
		{
			game.stats[STAT_MISSIONS_COMPLETE]++;

			if (strcmp(t->key, "teeka") == 0)
			{
				game.isComplete = 1;

				unlockTeeka = 0;
			}
		}
	}
	
	teeka = NULL;
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (requiredMissionUnlocked(mission->requires) || dev.cheatLevels)
		{
			unlockMission(mission->id);
		}
		
		mission->status = getMissionStatus(mission->id);
		
		if (strcmp(mission->id, "teeka") == 0)
		{
			teeka = mission;
		}
		else if (mission->status != MS_COMPLETE)
		{
			unlockTeeka = 0;
		}
		
		game.totalMissions++;
	}
	
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (game.isComplete)
		{
			STRNCPY(mission->description, _("As the game is now complete, free play for this mission has been unlocked."), MAX_DESCRIPTION_LENGTH);
		}
		else if (mission->status == MS_MISSING_HEART_CELL)
		{
			STRNCPY(mission->description, _("All objectives for this misson have been completed. However, there is a Cell or a Heart left to find. See if you can locate it."), MAX_DESCRIPTION_LENGTH);
		}
	}
	
	/* keep our unlock count in sync */
	unlockedMissions = 0;
	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->status != MS_LOCKED)
		{
			unlockedMissions++;
		}
	}
	
	if (teeka != NULL)
	{
		if (unlockTeeka)
		{
			unlockMission("teeka");
			
			teeka->status = MS_INCOMPLETE;
		}
		else
		{
			teeka->status = MS_LOCKED;
		}
	}

	awardMissionTrophies();
	
	cloudPos.x = randF() - randF();
	cloudPos.y = randF() - randF();
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	app.restrictTrophyAlert = 0;
	
	playMusic(1);
	
	endSectionTransition();
}

static void logic(void)
{
	blipValue += 0.1;
	
	blipSize = 64 + (sin(blipValue) * 16);
	
	scrollBackground(cloudPos.x, cloudPos.y);
	
	animateSprites();
	
	switch (showing)
	{
		case SHOW_NONE:
			doCursor();
			if (selectedMission == NULL)
			{
				doMissionSelect();
			}
			else
			{
				doMissionInfo();
			}
			break;
			
		case SHOW_WIDGETS:
			doWidgets();
			if (app.keyboard[SDL_SCANCODE_ESCAPE])
			{
				playSound(SND_MENU_BACK, 0);
				showing = SHOW_NONE;
				app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
			}
			break;
			
		case SHOW_STATS:
			doStats();
			if (app.keyboard[SDL_SCANCODE_ESCAPE])
			{
				playSound(SND_MENU_BACK, 0);
				returnFromTrophyStats();
			}
			break;
			
		case SHOW_TROPHIES:
			doTrophies();
			if (app.keyboard[SDL_SCANCODE_ESCAPE])
			{
				playSound(SND_MENU_BACK, 0);
				returnFromTrophyStats();
			}
			break;
			
		default:
			break;
	}
}

static void doCursor(void)
{
	if (app.mouse.dx != 0 || app.mouse.dy != 0)
	{
		cursor.x = app.mouse.x * app.scaleX;
		cursor.y = app.mouse.y * app.scaleY;
	}

	if (isControl(CONTROL_UP) || app.keyboard[SDL_SCANCODE_UP])
	{
		cursor.y -= CURSOR_SPEED;
		SDL_WarpMouseInWindow(app.window, cursor.x / app.scaleX, cursor.y / app.scaleY);
	}

	if (isControl(CONTROL_DOWN) || app.keyboard[SDL_SCANCODE_DOWN])
	{
		cursor.y += CURSOR_SPEED;
		SDL_WarpMouseInWindow(app.window, cursor.x / app.scaleX, cursor.y / app.scaleY);
	}

	if (isControl(CONTROL_LEFT) || app.keyboard[SDL_SCANCODE_LEFT])
	{
		cursor.x -= CURSOR_SPEED;
		SDL_WarpMouseInWindow(app.window, cursor.x / app.scaleX, cursor.y / app.scaleY);
	}

	if (isControl(CONTROL_RIGHT) || app.keyboard[SDL_SCANCODE_RIGHT])
	{
		cursor.x += CURSOR_SPEED;
		SDL_WarpMouseInWindow(app.window, cursor.x / app.scaleX, cursor.y / app.scaleY);
	}
}

static void doMissionSelect(void)
{
	HubMission *m;
	
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		playSound(SND_MENU_BACK, 0);
		showWidgetGroup("hub");
		showing = SHOW_WIDGETS;
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
	}
	else if (isControl(CONTROL_FIRE) || app.mouse.button[SDL_BUTTON_LEFT])
	{
		m = getMissionAt(cursor.x, cursor.y);
		
		if (m != NULL)
		{
			selectedMission = m;
			app.mouse.button[SDL_BUTTON_LEFT] = 0;
			clearControl(CONTROL_FIRE);

			showWidgetGroup("mission");
		}
	}
}

static void doMissionInfo(void)
{
	Widget *w;
	
	w = selectWidgetAt(cursor.x, cursor.y);
	
	if ((w != NULL) && (isControl(CONTROL_FIRE) || app.mouse.button[SDL_BUTTON_LEFT]))
	{
		w->action();
		app.mouse.button[SDL_BUTTON_LEFT] = 0;
		clearControl(CONTROL_FIRE);
	}
		
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		cancel();
	}
}

static void draw(void)
{
	blitRectScaled(atlasTexture->texture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &worldMap->rect, 0);
	
	drawBackground(atlasTexture->texture, &clouds->rect);
	
	drawMissions();
	
	drawInfoBar();
	
	switch (showing)
	{
		case SHOW_NONE:
			if (selectedMission != NULL)
			{
				drawMissionInfo();
				drawWidgets();
			}
			blitRect(atlasTexture->texture, cursor.x, cursor.y, getCurrentFrame(cursorSpr), 1);
			break;
			
		case SHOW_WIDGETS:
			drawHudWidgets();
			break;
			
		case SHOW_STATS:
			drawStats();
			break;
			
		case SHOW_TROPHIES:
			drawTrophies();
			break;
	}
}

static void drawMissions(void)
{
	HubMission *mission;
	
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
				
			default:
				break;
		}
	}
	
	SDL_SetTextureColorMod(atlasTexture->texture, 255, 255, 255);
}

static void drawInfoBar(void)
{
	drawRect(0, 0, SCREEN_WIDTH, 32, 0, 0, 0, 192);
	
	drawText(10, 5, 18, TA_LEFT, colors.white, "Missions : %d / %d", game.stats[STAT_MISSIONS_COMPLETE], unlockedMissions);
	
	drawText(210, 5, 18, TA_LEFT, colors.white, "MIAs : %d / %d", game.stats[STAT_MIAS_RESCUED], game.totalMIAs);
	
	drawText(410, 5, 18, TA_LEFT, colors.white, "Targets : %d / %d", game.stats[STAT_TARGETS_DEFEATED], game.totalTargets);
	
	drawText(610, 5, 18, TA_LEFT, colors.white, "Keys : %d / %d", game.stats[STAT_KEYS_FOUND], game.totalKeys);
	
	drawText(810, 5, 18, TA_LEFT, colors.white, "Hearts : %d / %d", game.stats[STAT_HEARTS_FOUND], game.totalHearts);
	
	drawText(1010, 5, 18, TA_LEFT, colors.white, "Cells : %d / %d", game.stats[STAT_CELLS_FOUND], game.totalCells);
}

static void drawHudWidgets(void)
{
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 128);
	
	drawWidgetFrame();
	
	drawWidgets();
}

static void drawMissionInfo(void)
{
	int w, h, x, y, size, mid, i;
	
	w = 800;
	h = 550;
	x = (SCREEN_WIDTH - w) / 2;
	y = (SCREEN_HEIGHT - h) / 2;
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 128);
	
	drawRect(x, y, w, h, 0, 0, 0, 192);
	drawOutlineRect(x, y, w, h, 255, 255, 255, 255);
	
	drawText(SCREEN_WIDTH / 2, y + 25, 32, TA_CENTER, colors.white, selectedMission->name);
	
	limitTextWidth(w - 150);
	drawText(x + 15, y + 100, 22, TA_LEFT, colors.white, selectedMission->description);
	limitTextWidth(0);
	
	size = 65;
	mid = size / 2;
	
	y = (((SCREEN_HEIGHT - h) / 2) + h) - 225;
	
	drawText(SCREEN_WIDTH / 2, y, 24, TA_CENTER, colors.white, "Keys");
	
	y += 64;
	
	x = ((SCREEN_WIDTH - w) / 2) + 30;
	
	for (i = 0 ; i < MAX_KEY_TYPES ; i++)
	{
		drawRect(x, y, size, size, 0, 0, 0, 128);
		
		drawOutlineRect(x, y, size, size, 255, 255, 255, 255);
		
		if (game.keys[i].value.i > 0)
		{
			blitRect(atlasTexture->texture, x + mid, y + mid + 7, getCurrentFrame(keySprites[i]), 1);
			
			drawText(x + size - 5, y, 18, TA_RIGHT, colors.white, "%d", game.keys[i].value.i);
		}
		
		x += (size + 30);
	}
}

static void unlockMission(char *id)
{
	Tuple *t;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, id) == 0)
		{
			if (t->value.i == MS_LOCKED || game.isComplete)
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

static int requiredMissionUnlocked(char *id)
{
	Tuple *t;
	
	if (strlen(id) > 0)
	{
		for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
		{
			if (strcmp(t->key, id) == 0)
			{
				return (t->value.i == MS_PARTIAL || t->value.i == MS_COMPLETE);
			}
		}
	}
	else
	{
		return 1;
	}
	
	return 0;
}

static void unlockAllMissions(void)
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

static void startMission(void)
{
	STRNCPY(game.worldId, selectedMission->id, MAX_NAME_LENGTH);
	
	saveGame();
	
	stopMusic();
	
	destroyHub();
	
	initWorld();
}

static void cancel(void)
{
	hideAllWidgets();
	showing = SHOW_NONE;
	selectedMission = NULL;
	app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
}

static void options(void)
{
	initOptions(returnFromOptions);
}

static void stats(void)
{
	showing = SHOW_STATS;
	initStatsDisplay();
	showWidgetGroup("stats");
}

static void trophies(void)
{
	showing = SHOW_TROPHIES;
	showWidgetGroup("trophies");
}

static void quit(void)
{
	stopMusic();
	
	destroyHub();
	
	initTitle();
}

static void returnFromTrophyStats(void)
{
	showWidgetGroup("hub");
	showing = SHOW_WIDGETS;
	app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
}

static void returnFromOptions(void)
{
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	returnFromTrophyStats();
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
		STRNCPY(mission->requires, cJSON_GetObjectItem(node, "requires")->valuestring, MAX_NAME_LENGTH);
		mission->status = MS_LOCKED;
		
		mission->x = cJSON_GetObjectItem(node, "x")->valuedouble * ratioX;
		mission->y = cJSON_GetObjectItem(node, "y")->valuedouble * ratioY;
	}
	
	cJSON_Delete(root);
	
	free(text);
}

static void awardMissionTrophies(void)
{
	int beach, greenlands, underground, outpost, save;
	HubMission *mission;

	beach = greenlands = underground = outpost = 1;
	
	save = 0;

	for (mission = hubMissionHead.next ; mission != NULL ; mission = mission->next)
	{
		if (mission->status != MS_COMPLETE)
		{
			if (strstr(mission->id, "beach"))
			{
				beach = 0;
			}
			else if (strstr(mission->id, "greenlands"))
			{
				greenlands = 0;
			}
			else if (strstr(mission->id, "underground"))
			{
				underground = 0;
			}
			else if (strstr(mission->id, "outpost"))
			{
				outpost = 0;
			}
		}
	}

	if (beach)
	{
		awardTrophy("BEACH");
		save = 1;
	}

	if (greenlands)
	{
		awardTrophy("GREENLANDS");
		save = 1;
	}

	if (underground)
	{
		awardTrophy("UNDERGROUND");
		save = 1;
	}

	if (outpost)
	{
		awardTrophy("OUTPOST");
		save = 1;
	}

	/* ignore training mission */
	if (game.stats[STAT_MISSIONS_COMPLETE] == 2)
	{
		awardTrophy("CLEAN");
		save = 1;
	}
	
	/* ignore Teeka's */
	if (game.totalMissions - game.stats[STAT_MISSIONS_COMPLETE] == 1)
	{
		awardTrophy("FULLY_CLEAN");
		save = 1;
	}
	
	if (save)
	{
		saveGame();
	}
}

void destroyHub(void)
{
	HubMission *m;

	while (hubMissionHead.next)
	{
		m = hubMissionHead.next;

		hubMissionHead.next = m->next;

		free(m);
	}
	
	memset(&hubMissionHead, 0, sizeof(HubMission));
	hubMissionTail = &hubMissionHead;
}
