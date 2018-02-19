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
static void bloodGore(void);
static void trophyScreenshot(void);
static void trophyAlert(void);
static void inventory(void);
static void keyboard(void);
static void joypad(void);
static void back(void);

static Widget soundVolumeWidget;
static Widget musicVolumeWidget;
static Widget bloodGoreWidget;
static Widget trophyScreenshotWidget;
static Widget trophyAlertWidget;
static Widget inventoryWidget;
static Widget keyboardWidget;
static Widget joypadWidget;
static Widget backWidget;

void initOptions((void)(*callback))
{
	returnFromOptions = callback;

	atlasTexture = getTexture("");

	soundVolumeWidget = getWidget("soundVolume", "options")
	soundVolumeWidget->action = soundVolume;
	soundVolumeWidget->value = game.config.soundVolume;
	
	musicVolumeWidget = getWidget("musicVolume", "options")
	musicVolumeWidget->action = musicVolume;
	musicVolumeWidget->value = game.config.musicVolume;
	
	bloodGoreWidget = getWidget("bloodGore", "options")
	bloodGoreWidget->action = bloodGore;
	bloodGoreWidget->value = game.config.blood;
	
	trophyScreenshotWidget = getWidget("trophyScreenshot", "options")
	trophyScreenshotWidget->action = trophyScreenshot;
	trophyScreenshotWidget->value = game.config.trophyScreenshot;
	
	trophyAlertWidget = getWidget("trophyAlert", "options")
	trophyAlertWidget->action = trophyAlert;
	trophyAlertWidget->value = game.config.trophyAlert;
	
	inventoryWidget = getWidget("inventory", "options")
	inventoryWidget->action = inventory;
	inventoryWidget->value = game.config.inventory;
	
	keyboardWidget = getWidget("keyboard", "options")
	keyboardWidget->action = keyboard;
	
	joypadWidget = getWidget("joypad", "options")
	joypadWidget->action = joypad;
	
	backWidget = getWidget("back", "options")
	backWidget->action = back;

	showWidgetGroup("options");

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{

}

static void draw(void)
{

}

static void soundVolume(void)
{

}

static void musicVolume(void)
{

}

static void bloodGore(void)
{

}

static void trophyScreenshot(void)
{

}

static void trophyAlert(void)
{

}

static void inventory(void)
{

}

static void keyboard(void)
{

}

static void joypad(void)
{

}

static void back(void)
{

}
