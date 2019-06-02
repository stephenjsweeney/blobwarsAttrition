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

#include "effects.h"

static Sprite *fleshChunk[3];
static Sprite *debris[3];

void initEffects(void)
{
	fleshChunk[0] = getSprite("FleshChunk1");
	fleshChunk[1] = getSprite("FleshChunk2");
	fleshChunk[2] = getSprite("FleshChunk3");

	debris[0] = getSprite("Debris1");
	debris[1] = getSprite("Debris2");
	debris[2] = getSprite("Debris3");
}

void addSmallFleshChunk(float x, float y)
{
	Decoration *chunk;

	if (app.config.blood)
	{
		chunk = malloc(sizeof(Decoration));
		memset(chunk, 0, sizeof(Decoration));
		initFleshChunk(chunk);

		chunk->x = x;
		chunk->y = y;
		chunk->health = FPS / 4;
		chunk->sprite[0] = chunk->sprite[1] = chunk->sprite[2] = fleshChunk[0];

		if (app.config.blood == 2)
		{
			chunk->health = FPS * rrnd(2, 4);
		}
	}
}

void throwFleshChunks(float x, float y, int amount)
{
	int i;
	Decoration *chunk;

	amount *= app.config.blood;

	for (i = 0; i < amount; i++)
	{
		chunk = malloc(sizeof(Decoration));
		memset(chunk, 0, sizeof(Decoration));
		initFleshChunk(chunk);

		chunk->x = x;
		chunk->y = y;
		chunk->dx = rrnd(-20, 20);
		chunk->dx /= 10;
		chunk->dy = -rrnd(10, 15);
		chunk->health = FPS * rrnd(3, 12);
		chunk->sprite[0] = chunk->sprite[1] = chunk->sprite[2] = fleshChunk[i % 3];

		if (app.config.blood == 2)
		{
			chunk->health = FPS * rrnd(FPS, FPS * 5);
		}
	}
}

void throwDebris(float x, float y, int amount)
{
	int i;
	Decoration *piece;

	for (i = 0; i < amount; i++)
	{
		piece = malloc(sizeof(Decoration));
		memset(piece, 0, sizeof(Decoration));
		initDebris(piece);

		piece->x = x;
		piece->y = y;
		piece->dx = rrnd(-20, 20);
		piece->dx /= 10;
		piece->dy = -rrnd(10, 15);
		piece->health = FPS * rrnd(1, 10);
		piece->sprite[0] = piece->sprite[1] = piece->sprite[2] = debris[i % 3];

		if (app.config.blood == 2)
		{
			piece->health = FPS * rrnd(30, 60);
		}
	}
}
