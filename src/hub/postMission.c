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

#include "postMission.h"

static void logic(void);
static void draw(void);
static void updateMissionStatus(void);

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
	
	if (status != MS_INCOMPLETE)
	{
		app.delegate.logic = logic;
		app.delegate.draw = draw;
		
		app.restrictTrophyAlert = 0;
		
		canContinue = 0;
		
		missionCompleteY = SCREEN_HEIGHT;
		
		playSound(SND_MISSION_COMPLETE, 0);
		
		endSectionTransition();
	}
}

static void updateMissionStatus(void)
{
	Tuple *t;
	
	for (t = game.missionStatusHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, world.id) == 0)
		{
			t->value.i = status = getMissionStatus();
			return;
		}
	}
	
	t = malloc(sizeof(Tuple));
	memset(t, 0, sizeof(Tuple));
	game.missionStatusTail->next = t;
	game.missionStatusTail = t;
	
	STRNCPY(t->key, world.id, MAX_NAME_LENGTH);
	t->value.i = status = getMissionStatus();
	
	if (status != MS_INCOMPLETE)
	{
		saveGame();
		saveWorld();
	}
	else
	{
		restoreGameState();
	}
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
	
	blitRectScaled(atlasTexture->texture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &background->rect, 0);
	
	drawText(SCREEN_WIDTH / 2, missionCompleteY, 45, TA_CENTER, colors.white, "Mission Complete!");
	
	i = 0;
	
	if (missionCompleteY == 50)
	{
		w = 800;
		x = (SCREEN_WIDTH - w) / 2;
		y = 150;
		
		for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
		{
			c = o->required ? colors.red : colors.white;
			status = _("Incomplete");
			
			if (o->currentValue >= o->targetValue)
			{
				c = colors.green;
				status = _("Complete");
			}
			
			drawText(x + 20, y, 24, TA_LEFT, c, o->description);
			drawText(SCREEN_WIDTH / 2 + 100, y, 24, TA_LEFT, c, "%d / %d", MIN(o->currentValue, o->targetValue), o->targetValue);
			drawText(x + w - 20, y, 24, TA_RIGHT, c, status);
			
			y += 55;
			
			if (oNum < ++i)
			{
				return;
			}
		}
		
		drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80, 24, TA_CENTER, colors.white, _("Press Fire to Continue"));
		
		canContinue = 1;
	}
}
