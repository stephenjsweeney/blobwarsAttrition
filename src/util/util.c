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

#include "util.h"

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

char *timeToString(long millis, int showHours)
{
	static char TIME[MAX_NAME_LENGTH];

	int hours, minutes, seconds;

	seconds = millis / FPS;
	minutes = (seconds / FPS) % 60;
	hours = seconds / (FPS * FPS);
	seconds %= 60;

	if (showHours)
	{
		sprintf(TIME, "%dh %02dm %02ds", hours, minutes, seconds);
	}
	else
	{
		sprintf(TIME, "%dm %02ds", minutes, seconds);
	}

	return TIME;
}

void *resize(void *array, int oldSize, int newSize)
{
	void **newArray;
	int copySize;
	
	copySize = newSize > oldSize ? oldSize : newSize;
	
	newArray = malloc(newSize);
	memset(newArray, 0, newSize);
	memcpy(newArray, array, copySize);
	free(array);
	
	return newArray;
}
