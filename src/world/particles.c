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

#include "particles.h"

static void animate(Particle *p);
static Particle *createParticle(void);

static int bloodSprite[3];
static int explosionSprite[2];
static int flameSprite;
static int smokeSprite;
static int teleportStarSprite;

void initParticles(void)
{
	bloodSprite[0] = getSpriteIndex("Blood1");
	bloodSprite[1] = getSpriteIndex("Blood2");
	bloodSprite[2] = getSpriteIndex("Blood3");

	explosionSprite[0] = getSpriteIndex("Explosion1");
	explosionSprite[1] = getSpriteIndex("Explosion2");

	flameSprite = getSpriteIndex("Flame");
	smokeSprite = getSpriteIndex("Smoke");

	teleportStarSprite = getSpriteIndex("TeleportStar");
}

void addBlood(float x, float y)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->size = 1;
	p->dy = 1;
	p->health = rrnd(5, 30);
	p->spriteIndex = bloodSprite[(int) (rand() % 3)];
}

void addSparkParticles(float x, float y)
{
	Particle *p;
	int i;
	float c;
	
	for (i = 0; i < 3; i++)
	{
		p = createParticle();
		p->x = x;
		p->y = y;
		p->dx = rand() % 300 - rand() % 300;
		p->dx /= 100;
		p->dy = rand() % 300 - rand() % 300;
		p->dy /= 100;
		p->health = rrnd(5, 30);
		
		c = 50 + (rand() % 50);
		c /= 100;
		
		p->r = p->g = p->b = c;
	}
}

void addSmokeParticles(float x, float y)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->size = 1;
	p->health = rrnd(5, 30);
	p->spriteIndex = smokeSprite;
	p->spriteTime = 5;
	p->spriteFrame = 0;
	p->destroyAfterAnim = 1;
}

void addFlameParticles(float x, float y)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->size = 1;
	p->health = rrnd(5, 30);
	p->spriteIndex = flameSprite;
	p->spriteTime = 5;
	p->spriteFrame = 0;
	p->destroyAfterAnim = 1;
}

void addExplosionParticles(float x, float y, float radius, int amount)
{
	int i;
	Particle *p;
	
	for (i = 0; i < amount; i++)
	{
		p = createParticle();
		p->type = PT_TEXTURED;
		p->x = x;
		p->y = y;
		p->dx = (randF() - randF()) * radius;
		p->dx /= 5;
		p->dy = (randF() - randF()) * radius;
		p->dy /= 5;
		p->health = rrnd(FPS / 4, FPS);
		p->spriteTime = 5;
		p->spriteFrame = 0;
		p->spriteIndex = explosionSprite[i % 2];
	}
}

void addTeleportStar(float x, float y)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->dx = ((randF() - randF()) * 4);
	p->dy = ((randF() - randF()) * 4);
	p->health = rrnd(FPS / 4, FPS);
	p->r = p->g = p->b = 1.0f;
	p->spriteIndex = teleportStarSprite;
	p->spriteFrame = (rand() % 12);
	p->plane = PLANE_FOREGROUND;
}

void addTeleportStars(Entity *e)
{
	int x, y, i;
	
	x = (int) (e->x + (e->w / 2));
	y = (int) (e->y + (e->h / 2));

	for (i = 0 ; i < 32 ; i++)
	{
		addTeleportStar(x, y);
	}
}

void addMIATeleportStars(float x, float y)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->dy = -(1 + (rand()) % 4);
	p->health = FPS * 3;
	p->r = p->g = p->b = 1.0f;
	p->spriteIndex = teleportStarSprite;
	p->spriteFrame = (rand() % 12);
	p->plane = PLANE_FOREGROUND;
}

void addTeleporterEffect(float x, float y)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->dx = 0;
	p->dy = -(randF() * 2);
	p->health = rrnd(FPS / 4, FPS);
	p->r = p->g = p->b = 1.0f;
	p->spriteIndex = teleportStarSprite;
	p->spriteFrame = (rand() % 12);
}

void doParticles(void)
{
	Particle *p, *prev;
	int camMidX, camMidY;
	
	camMidX = camera.x + (SCREEN_WIDTH / 2);
	camMidY = camera.y + (SCREEN_HEIGHT / 2);
	
	prev = &world.particleHead;

	for (p = world.particleHead.next ; p != NULL ; p = p->next)
	{
		animate(p);

		p->x += p->dx;
		p->y += p->dy;
		
		p->onScreen = 0;

		if (--p->health <= 0 || (p->destroyAfterAnim && p->spriteTime == -1))
		{
			if (p == world.particleTail)
			{
				world.particleTail = prev;
			}
			
			prev->next = p->next;
			free(p);
			p = prev;
		}
		else if (getDistance(camMidX, camMidY, p->x, p->y) < SCREEN_WIDTH)
		{
			p->onScreen = 1;
		}
		
		prev = p;
	}
}

static void animate(Particle *p)
{
	Sprite *s;
	
	if (p->spriteTime != -1)
	{
		p->spriteTime--;

		if (p->spriteTime <= 0)
		{
			s = getSprite(p->spriteIndex);
			p->spriteFrame = (int) wrap(++p->spriteFrame, 0, s->numFrames);
			p->spriteTime = s->times[p->spriteFrame];
		}
	}
}

static Particle *createParticle(void)
{
	Particle *p;
	
	p = malloc(sizeof(Particle));
	memset(p, 0, sizeof(Particle));
	world.particleTail->next = p;
	world.particleTail = p;
	
	return p;
}
