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
	time_t t;
	
	t = millis;
	
	timeinfo = localtime(&t);
	
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

char *buildFormattedString(const char *format, ...)
{
	int n;
	char *s;
	va_list args;	
	
    va_start(args, format);
    n = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);
	
	s = malloc(sizeof(char) * n);
	
	va_start(args, format);
	vsprintf(s, format, args);
	va_end(args);
	
	return s;
}

/* 
 * public domain strtok_r() by Charlie Gordon
 *
 *   from comp.lang.c  9/14/2007
 *
 *      http://groups.google.com/group/comp.lang.c/msg/2ab1ecbb86646684
 *
 *     (Declaration that it's public domain):
 *      http://groups.google.com/group/comp.lang.c/msg/7c7b39328fefab9c
 */
char *strtok_r(char *str, const char *delim, char **nextp)
{
	char *ret;

	if (str == NULL)
	{
		str = *nextp;
	}

	str += strspn(str, delim);

	if (*str == '\0')
	{
		return NULL;
	}

	ret = str;

	str += strcspn(str, delim);

	if (*str)
	{
		*str++ = '\0';
	}
	
	*nextp = str;
	
	return ret;
}
