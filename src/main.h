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

#include "common.h"
#include <time.h>

extern void awardTrophies(void);
extern void cleanup(void);
extern void createScreenshotFolder(void);
extern void doTrophyAlerts(void);
extern void drawTrophyAlert(void);
extern void handleInput(void);
extern void init18N(int argc, char *argv[]);
extern void initCredits(void);
extern void initEnding(void);
extern void initGameSystem(void);
extern void initLookups(void);
extern void initSDL(void);
extern void initTitle(void);
extern void initWorldTest(char *worldId);
extern void prepareScene(void);
extern void presentScene(void);
extern void saveScreenshot(char *name);

App app;
Camera camera;
Colors colors;
Dev dev;
Entity *self;
Game game;
World world;
