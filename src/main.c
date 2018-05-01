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

#include "main.h"

static long capFrameRate(const long then);
static void handleCommandLine(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	long then, nextSecond, frames;

	memset(&app, 0, sizeof(App));
	
	atexit(cleanup);

	srand(time(NULL));
	
	initLookups();
	
	init18N(argc, argv);
	
	initSDL();
	
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN);
	
	initGameSystem();
	
	handleCommandLine(argc, argv);
	
	frames = 0;
	
	nextSecond = SDL_GetTicks() + 1000;
	
	while (1)
	{
		then = SDL_GetTicks();
		
		handleInput();
		
		app.delegate.logic();
		
		doTrophyAlerts();
		
		prepareScene();

		app.delegate.draw();
		
		drawTrophyAlert();
		
		presentScene();
		
		then = capFrameRate(then);
		
		frames++;
		
		if (SDL_GetTicks() >= nextSecond)
		{
			dev.fps = frames;
			
			frames = 0;
			
			game.stats[STAT_TIME_PLAYED]++;
			
			nextSecond = SDL_GetTicks() + 1000;

			awardTrophies();
			
			expireTexts(0);
			
			if (dev.takeScreenshots)
			{
				saveScreenshot(NULL);
			}
		}
	}

	return 0;
}

static long capFrameRate(const long then)
{
	long wait;
	
	wait = 16 - (SDL_GetTicks() - then);
		
	if (wait > 0)
	{
		SDL_Delay(wait);
	}
	
	return SDL_GetTicks();
}

static void handleCommandLine(int argc, char *argv[])
{
	int i, plus;
	char *worldId;

	worldId = NULL;
	plus = 0;
	
	for (i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-debug") == 0)
		{
			dev.debug = 1;
			SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
			createScreenshotFolder();
		}

		if (strcmp(argv[i], "-info") == 0)
		{
			SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
		}

		if (strcmp(argv[i], "-blind") == 0)
		{
			dev.cheatBlind = 1;
		}

		if (strcmp(argv[i], "-noenemies") == 0)
		{
			dev.cheatNoEnemies = 1;
		}

		if (strcmp(argv[i], "-keys") == 0)
		{
			dev.cheatKeys = 1;
		}

		if (strcmp(argv[i], "-power") == 0)
		{
			dev.cheatPower = 1;
		}

		if (strcmp(argv[i], "-health") == 0)
		{
			dev.cheatHealth = 1;
		}

		if (strcmp(argv[i], "-levels") == 0)
		{
			dev.cheatLevels = 1;
		}

		if (strcmp(argv[i], "-screenshots") == 0)
		{
			dev.takeScreenshots = 1;
			createScreenshotFolder();
		}

		if (strcmp(argv[i], "-world") == 0)
		{
			worldId = argv[++i];
		}
		
		if (strcmp(argv[i], "-ending") == 0)
		{
			initEnding();
			return;
		}
		
		if (strcmp(argv[i], "-credits") == 0)
		{
			initCredits();
			return;
		}
		
		if (strcmp(argv[i], "-plus") == 0)
		{
			plus = 1;
		}
	}
	
	if (worldId != NULL)
	{
		initWorldTest(worldId, plus);
	}
	else
	{
		initTitle();
	}
}
