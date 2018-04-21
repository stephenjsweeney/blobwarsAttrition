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
static void populateSaveSlotWidgets(void);
static void doNewGame(void);
static void doLoadGame(void);
static void doContinueGame(void);
static void doOptions(void);
static void doCredits(void);
static void doQuit(void);
static void doSaveSlot(void);
static void doLoadCancel(void);
static void doOK(void);
static void doCancel(void);
void returnToTitle(void);

static Texture *atlasTexture;
static Atlas *title;
static int recentSaveSlot;
static int saveAction;
static Widget *startNewGame;
static Widget *load;
static Widget *continueGame;
static Widget *options;
static Widget *credits;
static Widget *quit;
static Widget *save[MAX_SAVE_SLOTS];
static Widget *loadCancel;
static Widget *ok;
static Widget *cancel;
static float titleAlpha;

void initTitle(void)
{
	startSectionTransition();
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	title = getImageFromAtlas("gfx/main/title.png");
	
	startNewGame = getWidget("new", "title");
	startNewGame->action = &doNewGame;

	load = getWidget("load", "title");
	load->action = &doLoadGame;

	continueGame = getWidget("continue", "title");
	continueGame->action = &doContinueGame;

	options = getWidget("options", "title");
	options->action = &doOptions;

	credits = getWidget("credits", "title");
	credits->action = &doCredits;

	quit = getWidget("exit", "title");
	quit->action = &doQuit;

	populateSaveSlotWidgets();

	loadCancel = getWidget("cancel", "load");
	loadCancel->action = doLoadCancel;

	ok = getWidget("ok", "delete");
	ok->action = doOK;

	cancel = getWidget("cancel", "delete");
	cancel->action = doCancel;
	
	titleAlpha = 0;
	
	recentSaveSlot = getRecentSave();
	
	showWidgetGroup("title");
	
	if (recentSaveSlot != -1)
	{
		setSelectedWidget("continue", "title");
	}
	else
	{
		load->disabled = 1;
		continueGame->disabled = 1;
	}
	
	saveAction = SA_NONE;
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	loadMusic("music/Watching - DJ Sjors.ogg");
	
	playMusic(0);
	
	endSectionTransition();
}

static void logic(void)
{
	doWidgets();
	
	if (titleAlpha < 255)
	{
		titleAlpha++;
	}
	
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		doLoadCancel();
		
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
	}
}

static void draw(void)
{
	SDL_SetTextureAlphaMod(atlasTexture->texture, titleAlpha);
	blitRect(atlasTexture->texture, SCREEN_WIDTH / 2, 175, &title->rect, 1);
	SDL_SetTextureAlphaMod(atlasTexture->texture, 255);
	
	drawText(10, SCREEN_HEIGHT - 30, 16, TA_LEFT, colors.white, "Copyright 2014, 2018 Parallel Realities");
	drawText(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 30, 16, TA_RIGHT, colors.white, "Version %.2f.%d", VERSION, REVISION);
	
	drawWidgets();
	
	if (saveAction == SA_NEW)
	{
		drawText(SCREEN_WIDTH / 2, 275, 24, TA_CENTER, colors.white, app.strings[ST_CHOOSE_SAVE]);
	}
	else if (saveAction == SA_LOAD)
	{
		drawText(SCREEN_WIDTH / 2, 275, 24, TA_CENTER, colors.white, app.strings[ST_LOAD]);
	}
	else if (saveAction == SA_DELETE)
	{
		drawText(SCREEN_WIDTH / 2, 350, 24, TA_CENTER, colors.white, app.strings[ST_OVERWRITE_1]);
		drawText(SCREEN_WIDTH / 2, 400, 22, TA_CENTER, colors.white, app.strings[ST_OVERWRITE_2]);
	}
}

static int getRecentSave(void)
{
	char *filename;
	int i, slot, curModTime, modTime;
	
	slot = -1;
	modTime = 0;
	
	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		filename = buildFormattedString("%s/%d/game.json", app.saveDir, i);
		
		if (fileExists(filename))
		{
			curModTime = getFileModTime(filename);
			
			if (curModTime > modTime)
			{
				modTime = curModTime;
				slot = i;
			}
		}
		
		free(filename);
	}
	
	return slot;
}

static void populateSaveSlotWidgets(void)
{
	int i;
	char name[MAX_NAME_LENGTH], *filename;

	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		sprintf(name, "save%d", i);

		save[i] = getWidget(name, "saveSlot");

		filename = buildFormattedString("%s/%d/game.json", app.saveDir, i);

		if (fileExists(filename))
		{
			strcpy(save[i]->label, getSaveWidgetLabel(filename));
			save[i]->value[0] = 1;
		}
		else
		{
			strcpy(save[i]->label, "(empty)");
			save[i]->value[0] = 0;
		}
		
		save[i]->value[1] = i;

		save[i]->action = &doSaveSlot;
		
		free(filename);
	}
}

static void doNewGame(void)
{
	int i;
	
	saveAction = SA_NEW;
	
	showWidgetGroup("saveSlot");
	
	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		save[i]->disabled = 0;
	}
	
	loadCancel->visible = 1;
	
	destroyGame();
}

static void doLoadGame(void)
{
	int i;
	
	saveAction = SA_LOAD;
	
	showWidgetGroup("saveSlot");
	
	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		save[i]->disabled = save[i]->value[0] == 0;
	}
	
	loadCancel->visible = 1;
}

static void doContinueGame(void)
{
	stopMusic();
	
	loadGame();
	
	initHub();
	
	game.saveSlot = continueGame->value[1];
}

static void doOptions(void)
{
	initOptions(returnToTitle);
}

static void doCredits(void)
{
	initCredits(0);
}

static void doQuit(void)
{
	stopMusic();
	
	exit(1);
}

static void doSaveSlot(void)
{
	Widget *w;
	
	w = getSelectedWidget();
	
	game.saveSlot = w->value[1];
	
	if (saveAction == SA_LOAD)
	{
		stopMusic();
		
		loadGame();
		
		initHub();
	}
	else if (saveAction == SA_NEW)
	{
		if (!w->value[0])
		{
			doOK();
		}
		else
		{
			saveAction = SA_DELETE;
			
			showWidgetGroup("delete");
			
			setSelectedWidget("cancel", "delete");
		}
	}
}

static void doLoadCancel(void)
{
	showWidgetGroup("title");
	
	saveAction = SA_NONE;
}

static void doOK(void)
{
	int saveSlot;
	
	saveSlot = game.saveSlot;
	
	deleteSaveSlot(saveSlot);
	
	stopMusic();
	
	newGame();
	
	initHub();
	
	game.saveSlot = saveSlot;
	
	saveGame();
}

static void doCancel(void)
{
	doNewGame();
}

void returnToTitle(void)
{
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	showWidgetGroup("title");
}
