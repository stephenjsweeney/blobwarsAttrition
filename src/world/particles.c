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

static Sprite *bloodSprite[3];
static Sprite *explosionSprite[2];
static Sprite *flameSprite;
static Sprite *smokeSprite;
static Sprite *teleportStarSprite;
static Texture *atlasTexture;

void initParticles(void)
{
	bloodSprite[0] = getSprite("Blood1");
	bloodSprite[1] = getSprite("Blood2");
	bloodSprite[2] = getSprite("Blood3");

	explosionSprite[0] = getSprite("Explosion1");
	explosionSprite[1] = getSprite("Explosion2");

	flameSprite = getSprite("Flame");
	smokeSprite = getSprite("Smoke");

	teleportStarSprite = getSprite("TeleportStar");
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
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

void drawParticles(int plane)
{
	Particle *p;
	int x, y;
	
	for (p = world.particleHead.next ; p != NULL ; p = p->next)
	{
		if (p->onScreen && p->plane == plane)
		{
			x = -camera.x + p->x;
			y = -camera.y + p->y;

			switch (p->type)
			{
				case PT_TEXTURED:
					blitRect(atlasTexture->texture, x, y, &p->sprite->frames[p->spriteFrame]->rect, 1);
					break;
				case PT_POINT:
					drawRect(x, y, 2, 2, p->r, p->g, p->b, 255);
					break;
				case PT_LINE:
					drawLine(x, y, (int) (x + p->dx), (int) (y + p->dy), p->r, p->g, p->b, 255);
					break;
			}
		}
	}
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
	p->sprite = bloodSprite[(int) (rand() % 3)];
}

void addSparkParticles(float x, float y)
{
	Particle *p;
	int i;
	
	for (i = 0; i < 3; i++)
	{
		p = createParticle();
		p->x = x;
		p->y = y;
		p->dx = (randF() - randF()) * 3;
		p->dy = (randF() - randF()) * 3;
		p->health = rrnd(5, 30);
		
		p->r = p->g = p->b = rrnd(128, 255);
	}
}

void addSmokeParticles(float x, float y, int rising)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->size = 1;
	p->health = rrnd(5, 30);
	p->sprite = smokeSprite;
	p->spriteTime = 5;
	p->spriteFrame = 0;
	p->destroyAfterAnim = 1;
}

void addFlameParticles(float x, float y, int rising)
{
	Particle *p;
	
	p = createParticle();
	p->type = PT_TEXTURED;
	p->x = x;
	p->y = y;
	p->size = 1;
	p->health = rrnd(5, 30);
	p->sprite = flameSprite;
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
		p->sprite = explosionSprite[i % 2];
	}
}

void addPopParticles(float x, float y)
{
	int i;
	Particle *p;
	
	for (i = 0; i < 12; i++)
	{
		p = createParticle();
		p->type = PT_TEXTURED;
		p->x = x;
		p->y = y;
		p->dx = (randF() - randF()) * 12;
		p->dx /= 5;
		p->dy = (randF() - randF()) * 12;
		p->dy /= 5;
		p->health = rrnd(FPS / 4, FPS);
		p->spriteTime = -1;
		p->spriteFrame = 0;
		p->sprite = smokeSprite;
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
	p->r = p->g = p->b = 255;
	p->sprite = teleportStarSprite;
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
	p->r = p->g = p->b = 255;
	p->sprite = teleportStarSprite;
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
	p->r = p->g = p->b = 255;
	p->sprite = teleportStarSprite;
	p->spriteFrame = (rand() % 12);
}

static void animate(Particle *p)
{
	Sprite *s;
	
	s = p->sprite;
	
	if (p->spriteTime != -1)
	{
		if (--p->spriteTime <= 0)
		{
			if (++p->spriteFrame >= s->numFrames)
			{
				p->spriteFrame = 0;
			}
			
			p->spriteTime = p->sprite->times[p->spriteFrame];
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
	
	p->spriteTime = -1;
	
	return p;
}

void destroyParticles(void)
{
	Particle *p;

	while (world.particleHead.next)
	{
		p = world.particleHead.next;

		world.particleHead.next = p->next;

		free(p);
	}
}
