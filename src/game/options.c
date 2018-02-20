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

static Widget *soundVolumeWidget;
static Widget *musicVolumeWidget;
static Widget *bloodGoreWidget;
static Widget *trophyScreenshotWidget;
static Widget *trophyAlertWidget;
static Widget *inventoryWidget;
static Widget *keyboardWidget;
static Widget *joypadWidget;
static Widget *backWidget;
static Texture *atlasTexture;
static Atlas *background;

void initOptions(void (*callback)(void))
{
	returnFromOptions = callback;

	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	background = getImageFromAtlas("gfx/main/options.png");

	soundVolumeWidget = getWidget("soundVolume", "options");
	soundVolumeWidget->action = soundVolume;
	soundVolumeWidget->value = app.config.soundVolume;
	
	musicVolumeWidget = getWidget("musicVolume", "options");
	musicVolumeWidget->action = musicVolume;
	musicVolumeWidget->value = app.config.musicVolume;
	
	bloodGoreWidget = getWidget("bloodGore", "options");
	bloodGoreWidget->action = bloodGore;
	bloodGoreWidget->value = app.config.blood;
	
	trophyScreenshotWidget = getWidget("trophyScreenshot", "options");
	trophyScreenshotWidget->action = trophyScreenshot;
	trophyScreenshotWidget->value = app.config.trophyScreenshot;
	
	trophyAlertWidget = getWidget("trophyAlert", "options");
	trophyAlertWidget->action = trophyAlert;
	trophyAlertWidget->value = app.config.trophyAlert;
	
	inventoryWidget = getWidget("inventory", "options");
	inventoryWidget->action = inventory;
	inventoryWidget->value = app.config.inventory;
	
	keyboardWidget = getWidget("keyboard", "options");
	keyboardWidget->action = keyboard;
	
	joypadWidget = getWidget("joypad", "options");
	joypadWidget->action = joypad;
	
	backWidget = getWidget("back", "options");
	backWidget->action = back;

	showWidgetGroup("options");

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	doWidgets();
}

static void draw(void)
{
	float h;
	
	h = (SCREEN_WIDTH / 800.0) * background->rect.h;
	
	drawText(SCREEN_WIDTH / 2, 50, 40, TA_CENTER, colors.white, "Options");
	
	blitRectScaled(atlasTexture->texture, 0, SCREEN_HEIGHT - h, SCREEN_WIDTH, h, &background->rect, 0);
	
	drawWidgets();
}

static void soundVolume(void)
{
	app.config.soundVolume = soundVolumeWidget->value;
	
	Mix_Volume(-1, app.config.soundVolume);
}

static void musicVolume(void)
{
	app.config.musicVolume = musicVolumeWidget->value;
	
	Mix_VolumeMusic(app.config.musicVolume);
}

static void bloodGore(void)
{
	app.config.blood = bloodGoreWidget->value;
}

static void trophyScreenshot(void)
{
	app.config.trophyScreenshot = trophyScreenshotWidget->value;
}

static void trophyAlert(void)
{
	app.config.trophyAlert = trophyAlertWidget->value;
}

static void inventory(void)
{
	app.config.inventory = inventoryWidget->value;
}

static void keyboard(void)
{

}

static void joypad(void)
{

}

static void back(void)
{
	saveConfig();
	
	returnFromOptions();
}
