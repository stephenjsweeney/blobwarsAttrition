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

static char TIME_STRING[MAX_NAME_LENGTH];

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

int lineLineIntersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	int d, n1, n2, ua, ub;

	d = ((y4 - y3) * (x2 - x1)) - ((x4 - x3) * (y2 - y1));
	n1 = ((x4 - x3) * (y1 - y3)) - ((y4 - y3) * (x1 - x3));
	n2 = ((x2 - x1) * (y1 - y3)) - ((y2 - y1) * (x1 - x3));

	if (d == 0)
	{
		if ( n1 == 0 && n2 == 0)
		{
			return 0;
		}

		return 0;
	}
	
	ua = n1 / d;
	ub = n2 / d;

	return (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1);
}

int lineRectIntersection(int x1, int y1, int x2, int y2, SDL_Rect *r)
{
	return (
		lineLineIntersection(x1, y1, x2, y2, r->x, r->y, r->x + r->w, r->y) || /* top */
		lineLineIntersection(x1, y1, x2, y2, r->x, r->y + r->h, r->x + r->w, r->y + r->h) || /* bottom */
		lineLineIntersection(x1, y1, x2, y2, r->x, r->y, r->x, r->y + r->h) || /* left */
		lineLineIntersection(x1, y1, x2, y2, r->x + r->w, r->y, r->x + r->w, r->y + r->h) /* right */
	);
}

char *timeToString(int seconds, int showHours)
{
	int hours, minutes;

	/* max out at 99h 59m 59s */
	seconds = MIN(seconds, (100 * 60 * 60) - 1);

	hours = seconds / (FPS * FPS);
	minutes = (seconds / FPS) % FPS;
	seconds %= FPS;

	if (showHours)
	{
		sprintf(TIME_STRING, "%dh %02dm %02ds", hours, minutes, seconds);
	}
	else
	{
		sprintf(TIME_STRING, "%dm %02ds", minutes, seconds);
	}

	return TIME_STRING;
}

char *timeToDate(long millis)
{
	static char DATE[MAX_NAME_LENGTH];
	
	struct tm *timeinfo;
	
	timeinfo = localtime(&millis);
	
	strftime(DATE, MAX_NAME_LENGTH, "%d %b %Y, %I:%M%p", timeinfo);
	
	return DATE;
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
