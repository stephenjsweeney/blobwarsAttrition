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

#include "atlasTest.h"

static void logic(void);
static void draw(void);

static Texture *background;

void initAtlasTest(void)
{
	initGame();
	
	initHub();
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	loadWorld("data/maps/beachApproach.json");
	
	initWorld();
	
	initMap();
	
	initEntities();
	
	background = getTexture(world.background);
	
	cameraTrack(findEntity("Transmitter"));
}

static void logic(void)
{
	doWorld();
	
	doEntities();
	
	doParticles();
	
	if (app.keyboard[SDL_SCANCODE_UP])
	{
		camera.y -= CAMERA_SCROLL_SPEED;
	}
	
	if (app.keyboard[SDL_SCANCODE_DOWN])
	{
		camera.y += CAMERA_SCROLL_SPEED;
	}
	
	if (app.keyboard[SDL_SCANCODE_LEFT])
	{
		camera.x -= CAMERA_SCROLL_SPEED;
	}
	
	if (app.keyboard[SDL_SCANCODE_RIGHT])
	{
		camera.x += CAMERA_SCROLL_SPEED;
	}
	
	clipCamera();
}

static void draw(void)
{
	blitScaled(background->texture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	
	drawEntities(PLANE_BACKGROUND);
	
	drawMap();
	
	drawEntities(PLANE_FOREGROUND);
}
