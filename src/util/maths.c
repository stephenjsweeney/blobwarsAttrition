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

#include "maths.h"

float mod(float n, float x)
{
	return fmod(fmod(n, x) + x, x);
}

int rrnd(int low, int high)
{
	return low + rand() % ((high - low) + 1);
}

double randF(void)
{
    return (double)rand() / (double)((unsigned)RAND_MAX + 1);
}

int getPercent(float current, float total)
{
	return (current / total) * 100;
}

float limit(float i, float a, float b)
{
	if (i < a)
	{
		return a;
	}
	
	if (i > b)
	{
		return b;
	}
	
	return i;
}

int getDistance(int x1, int y1, int x2, int y2)
{
	int x, y;
	
	x = x2 - x1;
	y = y2 - y1;
	
	return sqrt(x * x + y *y);
}

void getSlope(int x1, int y1, int x2, int y2, float *dx, float *dy)
{
	int steps = MAX(abs(x1 - x2), abs(y1 - y2));

	if (steps == 0)
	{
		*dx = *dy = 0;
		return;
	}

	*dx = (x1 - x2);
	*dx /= steps;

	*dy = (y1 - y2);
	*dy /= steps;
}

float wrap(float value, float low, float high)
{
	if (value < low)
	{
		return high;
	}

	if (value > high)
	{
		return low;
	}

	return value;
}

unsigned long hashcode(const char *str)
{
    unsigned long hash = 5381;
    int c;

	c = *str;

	while (c)
	{
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        c = *str++;
	}

	hash = ((hash << 5) + hash);
	
	return hash;
}
