/*
Copyright (C) 2018-2019 Parallel Realities

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

#include "postMission.h"

static void logic(void);
static void draw(void);
static void updateMissionStatus(void);
static int getPostMissionStatus(void);
static void saveGameAndWorld(void);

static int status;
static float missionCompleteY;
static Atlas *background;
static Texture *atlasTexture;
static float oNum;
static int canContinue;

void initPostMission(void)
{
	startSectionTransition();

	atlasTexture = getTexture("gfx/atlas/atlas.png");
	background = getImageFromAtlas("gfx/radar/background.png");

	updateMissionStatus();

	if (world.state == WS_GAME_COMPLETE)
	{
		saveGameAndWorld();

		destroyWorld();

		initEnding();
	}
	else if (world.state != WS_QUIT)
	{
		app.restrictTrophyAlert = 0;

		canContinue = 0;

		oNum = 0;

		missionCompleteY = UI_HEIGHT;

		playSound(SND_MISSION_COMPLETE, 0);

		app.delegate.logic = &logic;
		app.delegate.draw = &draw;

		saveGameAndWorld();

		endSectionTransition();
	}
	else
	{
		if (world.isReturnVisit)
		{
			saveGameAndWorld();
		}
		else
		{
			restoreGameState();

			saveGame(0);
		}

		destroyWorld();

		initHub();
	}
}

static void saveGameAndWorld(void)
{
	char *src, *dest;

	saveGame(1);

	if (game.plus == PLUS_NONE)
	{
		saveWorld();

		src = buildFormattedString("%s/%d/%s.json.tmp", app.saveDir, game.saveSlot, world.id);
		dest = buildFormattedString("%s/%d/%s.json", app.saveDir, game.saveSlot, world.id);

		if (!renameFile(src, dest))
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to save file '%s'", dest);
			exit(1);
		}

		free(src);
		free(dest);
	}

	src = buildFormattedString("%s/%d/game.json.tmp", app.saveDir, game.saveSlot);
	dest = buildFormattedString("%s/%d/game.json", app.saveDir, game.saveSlot, world.id);

	if (!renameFile(src, dest))
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to save file '%s'", dest);
		exit(1);
	}

	free(src);
	free(dest);
}

void retryMission(void)
{
	restoreGameState();

	saveGame(0);

	initWorld();
}

void returnToHub(void)
{
	restoreGameState();

	saveGame(0);

	destroyWorld();

	initHub();
}

static void updateMissionStatus(void)
{
	Tuple *t;

	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, world.id) == 0)
		{
			t->value.i = status = getPostMissionStatus();
			return;
		}
	}

	t = malloc(sizeof(Tuple));
	memset(t, 0, sizeof(Tuple));
	game.missionStatusTail->next = t;
	game.missionStatusTail = t;

	STRNCPY(t->key, world.id, MAX_NAME_LENGTH);
	t->value.i = status = getPostMissionStatus();
}

static void logic(void)
{
	int done;

	done = (status == MS_INCOMPLETE);

	missionCompleteY = limit(missionCompleteY - 10, 50, SCREEN_HEIGHT);

	if (missionCompleteY == 50)
	{
		oNum += 0.1;
	}

	if (canContinue && isAcceptControl())
	{
		done = 1;

		clearControls();
	}

	if (done)
	{
		destroyWorld();

		initHub();
	}
}

static void draw(void)
{
	Objective *o;
	SDL_Color c;
	char *status;
	int x, y, w, i;

	blitRectScaled(atlasTexture->texture, 0, 0, app.config.winWidth, app.config.winHeight, &background->rect, 0);

	SDL_SetRenderTarget(app.renderer, app.uiBuffer);

	drawText(UI_WIDTH / 2, missionCompleteY, 45, TA_CENTER, colors.white, app.strings[ST_MISSION_COMPLETE]);

	i = 0;

	if (missionCompleteY == 50)
	{
		w = 800;
		x = (UI_WIDTH - w) / 2;
		y = 150;

		for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
		{
			c = o->required ? colors.red : colors.white;
			status = app.strings[ST_INCOMPLETE];

			if (o->currentValue >= o->targetValue)
			{
				c = colors.green;
				status = app.strings[ST_COMPLETE];
			}

			drawText(x + 20, y, 24, TA_LEFT, c, o->description);
			drawText(UI_WIDTH / 2 + 100, y, 24, TA_LEFT, c, "%d / %d", MIN(o->currentValue, o->targetValue), o->targetValue);
			drawText(x + w - 20, y, 24, TA_RIGHT, c, status);

			y += 55;

			if (oNum < ++i)
			{
				return;
			}
		}

		drawText(UI_WIDTH / 2, UI_HEIGHT - 80, 24, TA_CENTER, colors.white, app.strings[ST_PRESS_FIRE]);

		canContinue = 1;
	}

	SDL_SetRenderTarget(app.renderer, app.backBuffer);
}

static int getPostMissionStatus(void)
{
	Objective *o;
	Entity *e;
	int status;

	status = MS_COMPLETE;

	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (o->required && o->currentValue < o->targetValue)
		{
			return MS_INCOMPLETE;
		}

		if (o->currentValue < o->totalValue)
		{
			status = MS_PARTIAL;
		}
	}

	if (status == MS_COMPLETE)
	{
		for (e = world.entityHead.next ; e != NULL ; e = e->next)
		{
			if (e->type == ET_HEART || e->type == ET_CELL)
			{
				return MS_MISSING_HEART_CELL;
			}
		}
	}

	return status;
}
