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

#include "options.h"

static void (*returnFromOptions)(void);
static void logic(void);
static void draw(void);
static void soundVolume(void);
static void musicVolume(void);
static void fullscreen(void);
static void windowSize(void);
static void bloodGore(void);
static void trophyAlert(void);
static void inventory(void);
static void controls(void);
static void back(void);
static void setGeneralOptions(void);
static void setWindowSizeOption(void);
static void setControlOptions(void);
static int section;

static Widget *soundVolumeWidget;
static Widget *musicVolumeWidget;
static Widget *windowSizeWidget;
static Widget *fullscreenWidget;
static Widget *bloodGoreWidget;
static Widget *trophyAlertWidget;
static Widget *inventoryWidget;
static Widget *controlsWidget;
static Texture *atlasTexture;
static Atlas *background;

void initOptions(void (*callback)(void))
{
	startSectionTransition();
	
	section = SECTION_MAIN;
	
	returnFromOptions = callback;

	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	background = getImageFromAtlas("gfx/main/options.png");
	
	setGeneralOptions();

	setControlOptions();

	showWidgetGroup("options");

	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	endSectionTransition();
}

static void setGeneralOptions(void)
{
	soundVolumeWidget = getWidget("soundVolume", "options");
	soundVolumeWidget->action = soundVolume;
	soundVolumeWidget->value[0] = app.config.soundVolume;
	
	musicVolumeWidget = getWidget("musicVolume", "options");
	musicVolumeWidget->action = musicVolume;
	musicVolumeWidget->value[0] = app.config.musicVolume;

	fullscreenWidget = getWidget("fullscreen", "options");
	fullscreenWidget->action = fullscreen;
	fullscreenWidget->value[0] = app.config.fullscreen;

	windowSizeWidget = getWidget("windowSize", "options");
	windowSizeWidget->action = windowSize;
	setWindowSizeOption();
	
	bloodGoreWidget = getWidget("bloodGore", "options");
	bloodGoreWidget->action = bloodGore;
	bloodGoreWidget->value[0] = app.config.blood;
	
	trophyAlertWidget = getWidget("trophyAlert", "options");
	trophyAlertWidget->action = trophyAlert;
	trophyAlertWidget->value[0] = app.config.trophyAlert;
	
	inventoryWidget = getWidget("inventory", "options");
	inventoryWidget->action = inventory;
	inventoryWidget->value[0] = app.config.inventory;
	
	controlsWidget = getWidget("controls", "options");
	controlsWidget->action = controls;
	
	getWidget("back", "options")->action = back;
	getWidget("back", "controls")->action = back;
}

static void setWindowSizeOption(void)
{
	int i;
	char winSize[16];

	sprintf(winSize, "%d x %d", app.config.winWidth, app.config.winHeight);

	for (i = 0 ; i < windowSizeWidget->numOptions ; i++)
	{
		if (strcmp(windowSizeWidget->options[i], winSize) == 0)
		{
			windowSizeWidget->value[0] = i;
			return;
		}
	}
}

static void setControlOptions(void)
{
	getWidget("left", "controls")->value[0] = app.config.keyControls[CONTROL_LEFT];
	getWidget("right", "controls")->value[0] = app.config.keyControls[CONTROL_RIGHT];
	getWidget("up", "controls")->value[0] = app.config.keyControls[CONTROL_UP];
	getWidget("down", "controls")->value[0] = app.config.keyControls[CONTROL_DOWN];
	getWidget("jump", "controls")->value[0] = app.config.keyControls[CONTROL_JUMP];
	getWidget("fire", "controls")->value[0] = app.config.keyControls[CONTROL_FIRE];
	getWidget("jetpack", "controls")->value[0] = app.config.keyControls[CONTROL_JETPACK];
	getWidget("pause", "controls")->value[0] = app.config.keyControls[CONTROL_PAUSE];
	getWidget("map", "controls")->value[0] = app.config.keyControls[CONTROL_MAP];
	
	getWidget("left", "controls")->value[1] = app.config.joypadControls[CONTROL_LEFT];
	getWidget("right", "controls")->value[1] = app.config.joypadControls[CONTROL_RIGHT];
	getWidget("up", "controls")->value[1] = app.config.joypadControls[CONTROL_UP];
	getWidget("down", "controls")->value[1] = app.config.joypadControls[CONTROL_DOWN];
	getWidget("jump", "controls")->value[1] = app.config.joypadControls[CONTROL_JUMP];
	getWidget("fire", "controls")->value[1] = app.config.joypadControls[CONTROL_FIRE];
	getWidget("jetpack", "controls")->value[1] = app.config.joypadControls[CONTROL_JETPACK];
	getWidget("pause", "controls")->value[1] = app.config.joypadControls[CONTROL_PAUSE];
	getWidget("map", "controls")->value[1] = app.config.joypadControls[CONTROL_MAP];
}

static void logic(void)
{
	doWidgets();
}

static void draw(void)
{
	float h;
	
	h = (SCREEN_WIDTH / 800.0) * background->rect.h;
	
	if (section == SECTION_MAIN)
	{
		drawText(SCREEN_WIDTH / 2, 50, 40, TA_CENTER, colors.white, app.strings[ST_OPTIONS]);
	}
	else
	{
		drawText(SCREEN_WIDTH / 2, 50, 40, TA_CENTER, colors.white, app.strings[ST_CONTROLS]);
	}
	
	blitRectScaled(atlasTexture->texture, 0, SCREEN_HEIGHT - h, SCREEN_WIDTH, h, &background->rect, 0);
	
	drawWidgets();
}

static void soundVolume(void)
{
	app.config.soundVolume = soundVolumeWidget->value[0];
	
	Mix_Volume(-1, app.config.soundVolume);
}

static void musicVolume(void)
{
	app.config.musicVolume = musicVolumeWidget->value[0];
	
	Mix_VolumeMusic(app.config.musicVolume);
}

static void fullscreen(void)
{
	app.config.fullscreen = fullscreenWidget->value[0];
}

static void windowSize(void)
{
	int i;

	i = windowSizeWidget->value[0];

	sscanf(windowSizeWidget->options[i], "%d x %d", &app.config.winWidth, &app.config.winHeight);
}

static void bloodGore(void)
{
	app.config.blood = bloodGoreWidget->value[0];
}

static void trophyAlert(void)
{
	app.config.trophyAlert = trophyAlertWidget->value[0];
}

static void inventory(void)
{
	app.config.inventory = inventoryWidget->value[0];
}

static void controls(void)
{
	section = SECTION_CONTROLS;
	
	playSound(SND_MENU_SELECT, 0);
	
	showWidgetGroup("controls");
}

static void updateControlConfig(void)
{
	app.config.keyControls[CONTROL_LEFT] = getWidget("left", "controls")->value[0];
	app.config.keyControls[CONTROL_RIGHT] = getWidget("right", "controls")->value[0];
	app.config.keyControls[CONTROL_UP] = getWidget("up", "controls")->value[0];
	app.config.keyControls[CONTROL_DOWN] = getWidget("down", "controls")->value[0];
	app.config.keyControls[CONTROL_JUMP] = getWidget("jump", "controls")->value[0];
	app.config.keyControls[CONTROL_FIRE] = getWidget("fire", "controls")->value[0];
	app.config.keyControls[CONTROL_JETPACK] = getWidget("jetpack", "controls")->value[0];
	app.config.keyControls[CONTROL_MAP] = getWidget("map", "controls")->value[0];
	app.config.keyControls[CONTROL_PAUSE] = getWidget("pause", "controls")->value[0];
	
	app.config.joypadControls[CONTROL_LEFT] = getWidget("left", "controls")->value[1];
	app.config.joypadControls[CONTROL_RIGHT] = getWidget("right", "controls")->value[1];
	app.config.joypadControls[CONTROL_UP] = getWidget("up", "controls")->value[1];
	app.config.joypadControls[CONTROL_DOWN] = getWidget("down", "controls")->value[1];
	app.config.joypadControls[CONTROL_JUMP] = getWidget("jump", "controls")->value[1];
	app.config.joypadControls[CONTROL_FIRE] = getWidget("fire", "controls")->value[1];
	app.config.joypadControls[CONTROL_JETPACK] = getWidget("jetpack", "controls")->value[1];
	app.config.joypadControls[CONTROL_MAP] = getWidget("map", "controls")->value[1];
	app.config.joypadControls[CONTROL_PAUSE] = getWidget("pause", "controls")->value[1];
}

static void back(void)
{
	switch (section)
	{
		case SECTION_MAIN:
			playSound(SND_MENU_BACK, 0);
			startSectionTransition();
			saveConfig();
			returnFromOptions();
			endSectionTransition();
			break;
			
		case SECTION_CONTROLS:
			updateControlConfig();
			playSound(SND_MENU_BACK, 0);
			section = SECTION_MAIN;
			showWidgetGroup("options");
			break;
	}
}
