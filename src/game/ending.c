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

#include "ending.h"

static void logic(void);
static void draw(void);
static void loadEndingText(void);
static char endingText[NUM_ENDING_LINES][MAX_DESCRIPTION_LENGTH];

static Atlas *background[2];
static Texture *atlasTexture;
static int endingTextIndex;
static int endingImageIndex;
static int endingTimer;

void initEnding(void)
{
	startSectionTransition();

	background[0] = getImageFromAtlas("gfx/ending/ending01.jpg");
	background[1] = getImageFromAtlas("gfx/ending/ending02.jpg");

	atlasTexture = getTexture("gfx/atlas/atlas.png");

	loadEndingText();

	endingTextIndex = 0;
	endingImageIndex = 0;
	endingTimer = strlen(endingText[endingTextIndex]) * 4;

	app.delegate.logic = &logic;
	app.delegate.draw = &draw;

	endSectionTransition();
}

static void logic(void)
{
	if (--endingTimer <= -FPS / 2)
	{
		if (endingTextIndex < NUM_ENDING_LINES)
		{
			if (++endingTextIndex == 3)
			{
				endingImageIndex++;
				startSectionTransition();
				endSectionTransition();
			}

			if (endingTextIndex < NUM_ENDING_LINES)
			{
				endingTimer = strlen(endingText[endingTextIndex]) * 4;
			}
			else
			{
				fadeMusic(3000);

				endingTimer = FPS * 4;
			}
		}
		else
		{
			initCredits(1);
		}
	}
}

static void draw(void)
{
	float h;
	int th;

	h = (app.config.winWidth / 800.0) * background[endingImageIndex]->rect.h;

	blitRectScaled(atlasTexture->texture, 0, app.config.winHeight - h, app.config.winWidth, h, &background[endingImageIndex]->rect, 0);

	if (endingTimer > 0 && endingTextIndex < NUM_ENDING_LINES)
	{
		app.textWidth = (app.config.winWidth / 2);
		th = getWrappedTextHeight(endingText[endingTextIndex], 24) + 15;
		drawRect(0, app.config.winHeight - th - 10, app.config.winWidth, th + 10, 0, 0, 0, 128);
		drawText(app.config.winWidth / 2, app.config.winHeight - th, 24, TA_CENTER, colors.white, endingText[endingTextIndex]);
		app.textWidth = 0;
	}
}

static void loadEndingText(void)
{
	int i;
	char *text, *line;

	i = 0;

	memset(endingText, 0, sizeof(NUM_ENDING_LINES * MAX_DESCRIPTION_LENGTH));

	text = readFile("data/misc/ending.txt");

	line = strtok(text, "\n");

	while (line)
	{
		strncpy(endingText[i++], _(line), MAX_DESCRIPTION_LENGTH);

		line = strtok(NULL, "\n");
	}

	free(text);
}
