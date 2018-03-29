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

#include "camera.h"

void clipCamera(void);

void cameraTrack(Entity *e)
{
	camera.x = (int) e->x + e->w / 2;
	camera.y = (int) e->y + e->h / 2;

	camera.x -= (SCREEN_WIDTH / 2);
	camera.y -= (SCREEN_HEIGHT / 2);

	clipCamera();
}

float cameraChase(Entity *e, int maxSpeed)
{
	float x, y, tx, ty, diffX, diffY, dist;
	
	x = camera.x;
	y = camera.y;

	tx = e->x - (SCREEN_WIDTH / 2);
	ty = e->y - (SCREEN_HEIGHT / 2);

	diffX = fabs(tx - x);
	diffY = fabs(ty - y);

	dist = MAX(diffX, diffY);

	diffX /= 20;
	diffY /= 20;

	diffX = MAX(0, MIN(maxSpeed, diffX));
	diffY = MAX(0, MIN(maxSpeed, diffY));

	if (x > tx)
	{
		x -= diffX;
	}

	if (x < tx)
	{
		x += diffX;
	}

	if (y > ty)
	{
		y -= diffY;
	}

	if (y < ty)
	{
		y += diffY;
	}

	camera.x = (int) x;
	camera.y = (int) y;

	clipCamera();

	return dist;
}

void clipCamera(void)
{
	camera.x = (int) limit(camera.x, world.map.bounds.x, world.map.bounds.w);
	camera.y = (int) limit(camera.y, world.map.bounds.y, world.map.bounds.h);
}

int isOnScreen(Entity *e)
{
	int x, y;

	x = (-camera.x + e->x);
	y = (-camera.y + e->y);

	return (x >= 0 && y >= 0 && x - e->w < SCREEN_WIDTH && y - e->h < SCREEN_HEIGHT);
}
