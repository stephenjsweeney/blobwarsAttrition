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

#include "title.h"

static void logic(void);
static void draw(void);
static int getRecentSave(void);

static Texture *atlasTexture;
static Atlas *title;
static int recentSaveSlot;
static Widget *newGame;
static Widget *loadGame;
static Widget *continueGame;
static Widget *options;
static Widget *credits;
static Widget *quit;

void initTitle(void)
{
	startSectionTransition();
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	title = getImageFromAtlas("gfx/main/title.png");
	
	newGame = getWidget("new", "title");
	loadGame = getWidget("load", "title");
	continueGame = getWidget("continue", "title");
	options = getWidget("options", "title");
	credits = getWidget("credits", "title");
	quit = getWidget("exit", "title");
	
	recentSaveSlot = getRecentSave();
	
	showWidgetGroup("title");
	
	if (recentSaveSlot != -1)
	{
		setSelectedWidget("continue", "title");
	}
	else
	{
		loadGame->disabled = 1;
		continueGame->disabled = 1;
	}
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	endSectionTransition();
}

static void logic(void)
{
	doWidgets();
}

static void draw(void)
{
	blitRect(atlasTexture->texture, SCREEN_WIDTH / 2, 175, &title->rect, 1);
	
	drawText(10, SCREEN_HEIGHT - 30, 18, TA_LEFT, colors.white, "Copyright 2014, 2018 Parallel Realities");
	drawText(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 30, 18, TA_RIGHT, colors.white, "Version %.2f.%d", VERSION, REVISION);
	
	drawWidgets();
}

static int getRecentSave(void)
{
	char filename[MAX_FILENAME_LENGTH];
	int i, slot, curModTime, modTime;
	
	slot = -1;
	modTime = 0;
	
	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		sprintf(filename, "%s/%d/game.json", app.saveDir, i);
		
		if (fileExists(filename))
		{
			curModTime = getFileModTime(filename);
			
			if (curModTime > modTime)
			{
				modTime = curModTime;
				slot = i;
			}
		}
	}
	
	return slot;
}
