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

#include "explosions.h"

static SDL_Rect radiusRect;
static unsigned int killTimer = 0;
static unsigned int numKilled = 0;

void addExplosion(float x, float y, int radius, Entity *owner)
{
	Entity **candidates, *e;
	float power;
	int i;
	
	playBattleSound(SND_EXPLOSION, 0, x, y);

	/* assuming x and y were from the top left of the entity */
	x += radius / 2;
	y += radius / 2;

	addExplosionParticles(x, y, radius, radius / 4);

	radiusRect.x = (int) (x - radius);
	radiusRect.y = (int) (y - radius);
	radiusRect.w = radius * 2;
	radiusRect.h = radius * 2;
	
	if (killTimer < SDL_GetTicks())
	{
		if (numKilled >= 2)
		{
			setGameplayMessage(MSG_STANDARD, "%d hit grenade combo!", numKilled);
			
			game.stats[STAT_EYE_DROID_EXPLOSION_KILLS] = MAX(game.stats[STAT_EYE_DROID_EXPLOSION_KILLS], numKilled);
		}
		
		if (numKilled >= 12)
		{
			awardTrophy("GRENADE_COMBO");
		}
		
		numKilled = 0;
	}

	candidates = getAllEntsWithin(radiusRect.x, radiusRect.y, radiusRect.w, radiusRect.h, NULL);

	for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
	{
		if (e != owner && (e->type == ET_BOB || e->type == ET_ENEMY || e->type == ET_DESTRUCTABLE || e->type == ET_BOSS))
		{
			power = radius - getDistance(e->x + (e->w / 2), e->y + (e->h / 2), x, y);

			if (power > 0)
			{
				if (!(e->flags & EF_IMMUNE))
				{
					if (e->flags & EF_BOMB_SHIELD)
					{
						swapSelf(e);
						e->applyDamage(2);
						swapSelf(e);
					}
					else
					{
						swapSelf(e);
						e->applyDamage((int) power);
						swapSelf(e);
					}

					if (e->type == ET_BOB)
					{
						stunBob();
					}
					
					if (owner->type == ET_ENEMY && e->health <= 0 && e->type == ET_ENEMY && strstr(((Unit*)owner)->unitType, "EyeDroid"))
					{
						game.stats[STAT_EYE_DROID_EXPLOSION_KILLS]++;
					}
					
					if (e->health <= 0)
					{
						e->dx = rrnd(-radius / 8, radius / 8);
						e->dy = rrnd(-5, 0);
						
						if (owner->type == ET_BOB)
						{
							numKilled++;
							killTimer = SDL_GetTicks() + 1000;
						}
					}
				}
			}
		}
	}
}
