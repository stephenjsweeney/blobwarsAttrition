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

void addExplosionEffect(int x, int y, float dx, float dy)
{
}

void addSmallFleshChunk(float x, float y)
{
	Decoration *chunk;
	
	chunk = malloc(sizeof(Decoration));
	memset(chunk, 0, sizeof(Decoration));
	world.entityTail->next = (Entity*)chunk;
	world.entityTail = (Entity*)chunk;
	
	chunk->x = x;
	chunk->y = y;
	chunk->dx = 0;
	chunk->dy = 0;
	chunk->health = FPS / 4;
	chunk->sprite[0] = chunk->sprite[1] = chunk->sprite[2] = fleshChunk[0];
}

void throwFleshChunks(float x, float y, int amount)
{
	int i;
	Decoration *chunk;
	
	for (i = 0; i < amount; i++)
	{
		chunk = malloc(sizeof(Decoration));
		memset(chunk, 0, sizeof(Decoration));
		world.entityTail->next = (Entity*)chunk;
		world.entityTail = (Entity*)chunk;
		
		chunk->x = x;
		chunk->y = y;
		chunk->dx = rrnd(-2, 2);
		chunk->dy = -rrnd(10, 15);
		chunk->health = FPS * rrnd(3, 12);
		chunk->sprite[0] = chunk->sprite[1] = chunk->sprite[2] = fleshChunk[i % 3];
	}
}
