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

#include "entities.h"

static void doMarker(Marker *m, int delta);
static void handleTeleport(void);
static int isObserving(void);
static void addRider(void);
static void checkPlatformContact(void);
static void moveEntity(void);
static void haltAtEdge(void);
static int canWalkOnEntity(float x, float y);
static void moveToOthers(float dx, float dy, PointF *position);
static void addTouched(Entity *e);
static int pushEntity(Entity *e, float dx, float dy);
static void moveToMap(float dx, float dy, PointF *position);
static int hasHitWorld(int mx, int my);
static void compareEnvironments(void);
static int getMarkerType(void);
static int drawComparator(const void *a, const void *b);

static Entity *riders[MAX_RIDERS];
static Entity *touched[MAX_TOUCHED];
static Texture *atlasTexture;
static Marker targetMarker[3];

void initEntities(void)
{
	int i;
	SDL_Rect *r;
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	for (i = 0 ; i < 3 ; i++)
	{
		memset(&targetMarker[i], 0, sizeof(Marker));
		targetMarker[i].sprite = getSprite("Marker");
	}
	
	for (self = world.entityHead.next ; self != NULL ; self = self->next)
	{
		/* 
		 * Most things retain their dimensions, so this isn't a big deal. If we set
		 * this each frame, it will muck up the bouncing, especially in the case of grenades.
		 */
		if (self->w == 0 || self->h == 0)
		{
			r = &self->sprite[self->facing]->frames[self->spriteFrame]->rect;
			
			self->w = r->w;
			self->h = r->h;
		}
		
		addToQuadtree(self, &world.quadtree);
	}
}

void doEntities(void)
{
	Entity *prev, *oldSelf, *e;
	int camMidX, camMidY, flicker, i;
	SDL_Rect *r;
	
	memset(riders, 0, sizeof(Entity*) * MAX_RIDERS);
	
	camMidX = camera.x + (SCREEN_WIDTH / 2);
	camMidY = camera.y + (SCREEN_HEIGHT / 2);
	
	doMarker(&targetMarker[0], 1);
	doMarker(&targetMarker[1], -1);
	doMarker(&targetMarker[2], 1);
	
	flicker = world.frameCounter % 3 > 0;
	
	prev = &world.entityHead;
	
	for (self = world.entityHead.next ; self != NULL ; self = self->next)
	{
		if (dev.cheatStatic && self != (Entity*)world.bob)
		{
			self->isVisible = 1;
			world.bob->flags |= (EF_WEIGHTLESS | EF_NO_CLIP);
			continue;
		}
		
		if (self->w == 0 || self->h == 0)
		{
			r = &self->sprite[self->facing]->frames[self->spriteFrame]->rect;
			
			self->w = r->w;
			self->h = r->h;
		}
		
		removeFromQuadtree(self, &world.quadtree);
		
		if (self->alive == ALIVE_DEAD)
		{
			prev->next = self->next;
			
			if (self == world.entityTail)
			{
				world.entityTail = prev;
			}
			
			free(self);
			
			self = prev;
			
			continue;
		}
		
		self->isVisible = 0;
		
		if (self->flags & EF_TELEPORTING)
		{
			handleTeleport();
			prev = self;
			continue;
		}
		
		if ((self->flags & EF_ALWAYS_PROCESS) > 0 || getDistance(camMidX, camMidY, self->x, self->y) < SCREEN_WIDTH || isObserving())
		{
			self->isVisible = 1;
		}
		else if (self->flags & EF_KILL_OFFSCREEN)
		{
			self->alive = ALIVE_DEAD;
		}

		self->riding = NULL;
		
		if (self->isVisible)
		{
			memset(touched, 0, sizeof(Entity*) * MAX_TOUCHED);

			if (--self->thinkTime <= 0)
			{
				self->thinkTime = 0;

				self->action();
			}

			if (self->flags & EF_GONE)
			{
				self->isVisible = 0;
				prev = self;
				continue;
			}

			self->tick();
			
			self->observationTime = limit(self->observationTime - 1, 0, FPS * 5);

			self->isOnGround = 0;
			
			if (self->dy >= 0 && (!(self->flags & EF_WEIGHTLESS)))
			{
				checkPlatformContact();
			}

			if (!self->isStatic)
			{
				moveEntity();
			}

			self->animate();
			
			for (i = 0 ; i < MAX_TOUCHED ; i++)
			{
				if (touched[i])
				{
					self->touch(touched[i]);
					
					if (touched[i]->isStatic)
					{
						oldSelf = self;
						
						self = touched[i];
						
						touched[i]->touch(oldSelf); /* for objects that never move */
						
						self = oldSelf;
					}
				}
			}
			
			if (!(self->flags & EF_NO_ENVIRONMENT))
			{
				compareEnvironments();

				/*
				 * Always sink if not in the air
				 */
				if (self->environment != ENV_AIR && (!(self->flags & EF_SWIMS)))
				{
					self->dy = 0.5f;
				}
			}

			if ((self->flags & EF_FLICKER) && flicker)
			{
				self->isVisible = 0;
			}
			
			if (self->alive == ALIVE_ALIVE)
			{
				if (self->health <= 0)
				{
					self->alive = ALIVE_DYING;
					self->spriteFrame = 0;
					self->die();
				}
			}
		}
		
		if (!(self->flags & (EF_TELEPORTING | EF_GONE)))
		{
			addToQuadtree(self, &world.quadtree);
		}
		
		prev = self;
	}
	
	for (i = 0 ; i < MAX_RIDERS ; i++)
	{
		e = riders[i];
		
		if (e != NULL)
		{			
			if (e->dy > 0)
			{
				pushEntity(e, e->riding->dx, 0);

				if (!pushEntity(e, 0, e->riding->dy))
				{
					e->riding->y -= e->riding->dy;
				}

				e->y = e->riding->y - e->h;
			}
		}
	}
}

void drawEntities(int plane)
{
	int x, y, draw, i, t;
	Entity **candidates;

	candidates = getAllEntsWithin(camera.x, camera.y, SCREEN_WIDTH, SCREEN_HEIGHT, NULL);

	/* counting entities to draw */
	for (i = 0, self = candidates[i] ; self != NULL ; self = candidates[++i]) {};

	qsort(candidates, i, sizeof(Entity*), drawComparator);
	
	for (i = 0, self = candidates[i] ; self != NULL ; self = candidates[++i])
	{
		draw = self->isVisible && self->plane == plane;
		
		if (draw)
		{
			x = (-camera.x + self->x);
			y = (-camera.y + self->y);
			
			blitRect(atlasTexture->texture, x, y, self->getCurrentSprite(), 0);
			
			x += (self->w / 2) - 9;
			
			if (self->type == ET_ENEMY && ((Unit*)self)->carriedItem != NULL)
			{
				blitRect(atlasTexture->texture, x, y - (targetMarker[0].y + 5), &targetMarker[0].sprite->frames[0]->rect, 0);
			}
			
			if (self->isMissionTarget)
			{
				t = getMarkerType();
				
				blitRect(atlasTexture->texture, x, y - (targetMarker[t].y + 5), &targetMarker[t].sprite->frames[t]->rect, 0);
			}
		}
	}
}

static int getMarkerType(void)
{
	switch (self->type)
	{
		case ET_ENEMY:
			return 1;
			
		case ET_MIA:
			return 2;
			
		default:
			return 0;
	}
}

static void checkPlatformContact(void)
{
	Entity *e;
	Entity **candidates;
	int i;
	SDL_Rect srcRect;
	
	srcRect.x = self->x;
	srcRect.y = self->y;
	srcRect.w = self->w;
	srcRect.h = self->h + 8;

	candidates = getAllEntsWithin(srcRect.x, srcRect.y, srcRect.w, srcRect.h, NULL);

	for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
	{
		if (e == self || e->type != ET_LIFT)
		{
			continue;
		}

		if (e->y > self->y + self->h - (8 + self->dy) && collision(self->x, self->y + 4, self->w, self->h, e->x, e->y, e->w, e->h))
		{
			self->riding = e;
			self->isOnGround = 1;
			self->dy = 0;
			addRider();

			/* required for bullets */
			self->touch(NULL);
		}
	}
}

static void moveEntity(void)
{
	PointF position;
	
	switch (self->environment)
	{
		case ENV_AIR:
			if (!(self->flags & EF_WEIGHTLESS))
			{
				self->dy += GRAVITY_POWER;
				self->dy = limit(self->dy, -25, 25);

				if (self->dy > 0 && self->dy < 1)
				{
					self->dy = 1;
				}
			}
			break;

		case ENV_WATER:
			self->flags &= ~EF_BOUNCES;
			if (!(self->flags & EF_SWIMS))
			{
				self->dy += GRAVITY_POWER;
				self->dy = limit(self->dy, -2, 2);
			}
			break;

		case ENV_SLIME:
		case ENV_LAVA:
			self->dy += GRAVITY_POWER;
			self->dx = limit(self->dx, -2, 2);
			self->dy = limit(self->dy, -2, 2);
			break;
	}

	if (self->flags & EF_HALT_AT_EDGE)
	{
		haltAtEdge();
	}

	/* Deal with x movement */
	position.x = self->x;
	position.y = self->y;
	position.x += self->dx;
	moveToOthers(self->dx, 0, &position);
	moveToMap(self->dx, 0, &position);

	/* Deal with Y movement */
	position.y += self->dy;
	moveToOthers(0, self->dy, &position);
	moveToMap(0, self->dy, &position);

	if (self->dy > 0 && self->riding != NULL)
	{
		position.y = self->riding->y;
		position.y -= self->h;
	}

	self->x = position.x;
	self->y = position.y;

	if (!(self->flags & (EF_KILL_OFFSCREEN | EF_NO_CLIP)))
	{
		self->x = limit(self->x, world.map.bounds.x, world.map.bounds.w + SCREEN_WIDTH - self->w);
		self->y = limit(self->y, world.map.bounds.y, world.map.bounds.h + SCREEN_HEIGHT - self->h);
	}
}

static void haltAtEdge(void)
{
	float x, y;
	int mx, my, i;
	
	if (!(self->flags & (EF_WEIGHTLESS | EF_SWIMS)))
	{
		if (self->environment == ENV_WATER)
		{
			return;
		}

		x = self->x + self->dx + (self->w / 2);
		y = self->y + self->h + 1;

		if (canWalkOnEntity(x, y))
		{
			return;
		}

		mx = x / MAP_TILE_SIZE;
		my = y / MAP_TILE_SIZE;

		for (i = 0; i < 3; i++)
		{
			if (isLiquid(mx, my + i))
			{
				self->walk();
				self->dx = 0;
				self->thinkTime = 0;
				return;
			}

			if (isWalkable(mx, my + i))
			{
				return;
			}
		}

		self->walk();
		self->dx = 0;
		self->thinkTime = 0;
		return;
	}
	else if ((self->flags & EF_SWIMS) && self->dy < 0)
	{
		x = self->x + self->dx + (self->w / 2);
		y = self->y + self->dy + (self->h / 2);

		mx = x / MAP_TILE_SIZE;
		my = y / MAP_TILE_SIZE;

		my--;

		if (world.map.data[mx][my] == MAP_TILE_AIR)
		{
			self->walk();
			self->dx = 0;
			self->dy = 0;
			self->thinkTime = 0;
			return;
		}
	}
	else if ((self->flags & EF_WEIGHTLESS) && self->dy > 0)
	{
		x = self->x + self->dx + self->w;
		y = self->y + self->dy + self->h;

		mx = x / MAP_TILE_SIZE;
		my = y / MAP_TILE_SIZE;

		my++;

		if (isLiquid(mx, my))
		{
			self->walk();
			self->dx = 0;
			self->dy = 0;
			self->thinkTime = 0;
			return;
		}
	}
}

static int canWalkOnEntity(float x, float y)
{
	int i;
	Entity *e;
	Entity **candidates;
	SDL_Rect srcRect;
	
	srcRect.x = x;
	srcRect.x = y;
	srcRect.w = 8;
	srcRect.h = MAP_TILE_SIZE * 4;

	candidates = getAllEntsWithin(srcRect.x, srcRect.y, srcRect.w, srcRect.h, NULL);

	for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
	{
		if (self != e && e->isSolid && collision(x, y, 1, MAP_TILE_SIZE * 2, e->x, e->y, e->w, e->y))
		{
			return 1;
		}
	}

	return 0;
}

static void moveToOthers(float dx, float dy, PointF *position)
{
	Entity *e;
	Entity **candidates;
	int clearTouched, hit, dirX, dirY, solidLoopHits, i;
	SDL_Rect srcRect;
	
	srcRect.x = (int) position->x;
	srcRect.y = (int) position->y;
	srcRect.w = self->w;
	srcRect.h = self->h;

	clearTouched = 0;
	hit = 0;
	dirX = (dx > 0) ? 1 : -1;
	dirY = (dy > 0) ? 1 : -1;
	solidLoopHits = 0;

	do
	{
		hit = 0;

		candidates = getAllEntsWithin(srcRect.x, srcRect.y, srcRect.w, srcRect.h, NULL);

		for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
		{
			if (e == self || e->owner == self || self->owner == e)
			{
				continue;
			}

			if (collision(srcRect.x, srcRect.y, srcRect.w, srcRect.h, e->x, e->y, e->w, e->h))
			{
				if (clearTouched)
				{
					memset(touched, 0, sizeof(Entity*) * MAX_TOUCHED);
					clearTouched = 0;
				}

				if (self->type == ET_BOB && e->type == ET_PUSHBLOCK && dx != 0)
				{
					if (!pushEntity(e, dx * 0.35, 0))
					{
						position->x = e->x;
						position->x -= (dirX == 1) ? self->w : -e->w;
						self->dx = self->bounce(self->dx);
					}
					else
					{
						self->animate();
					}
				}

				if (e->isSolid && self->type != ET_LIFT)
				{
					if (dx != 0)
					{
						position->x = e->x;
						position->x -= (dirX == 1) ? self->w : -e->w;
						self->dx = self->bounce(self->dx);
					}

					if (dy != 0)
					{
						if (e->y > self->y)
						{
							self->isOnGround = 1;
							self->riding = e;
						}

						position->y = e->y;
						position->y -= (dirY == 1) ? self->h : -e->h;
						self->dy = self->bounce(self->dy);
						self->dy = limit(self->dy, JUMP_POWER, -JUMP_POWER);
					}
				}

				if (self->isSolid && e->isSolid)
				{
					hit = 1;

					/* Infinite loop! Kill it! */
					if (dx == 0 && dy == 0 && solidLoopHits++ > 1)
					{
						self->alive = ALIVE_DEAD;
						printf("Warning: Killed stuck entity '%s', stuck in '%s'\n", self->name, e->name);
						return;
					}
				}

				addTouched(e);
			}
		}

		clearTouched = 1;

		srcRect.x = self->x;
		srcRect.y = self->y;
		srcRect.w = self->w;
		srcRect.h = self->h;
	}
	while (hit);
}

static int pushEntity(Entity *e, float dx, float dy)
{
	float expectedX, expectedY;
	PointF position;
	Entity *oldSelf;
	
	expectedX = e->x + dx;
	expectedY = e->y + dy;

	position.x = e->x;
	position.y = e->y;
	
	oldSelf = self;
	
	self = e;
	
	removeFromQuadtree(e, &world.quadtree);

	if (dx != 0)
	{
		position.x += dx;
		moveToOthers(dx, 0, &position);
		moveToMap(dx, 0, &position);
		e->x = position.x;
	}

	if (dy != 0)
	{
		position.y += dy;
		moveToOthers(0, dy, &position);
		moveToMap(0, dy, &position);
		e->y = position.y;
	}
	
	addToQuadtree(e, &world.quadtree);
	
	self = oldSelf;

	return (e->x == expectedX && e->y == expectedY);
}

static void moveToMap(float dx, float dy, PointF *position)
{
	int i, mx, my, width, height, adjX, adjY, hit;
	
	width = self->w;
	height = self->h;
	adjX = adjY = 0;
	
	if (self->flags & EF_NO_CLIP)
	{
		return;
	}

	if (dx != 0)
	{
		width = (dx > 0) ? self->w + 1 : -1;
		adjX = (dx > 0) ? self->w : -MAP_TILE_SIZE;
	}

	if (dy != 0)
	{
		height = (dy > 0) ? self->h + 1 : -1;
		adjY = (dy > 0) ? self->h : -MAP_TILE_SIZE;
	}

	hit = 0;

	mx = (position->x + width) / MAP_TILE_SIZE;
	my = (position->y + height) / MAP_TILE_SIZE;

	if (mx < 0 || my < 0)
	{
		return;
	}

	/* X Axis */

	if (dx != 0)
	{
		for (i = 0; i < self->h - 1; i += MAP_TILE_SIZE)
		{
			my = (position->y + i) / MAP_TILE_SIZE;

			hit = (hasHitWorld(mx, my) ? 1 : hit);
		}

		my = (position->y + self->h - 1) / MAP_TILE_SIZE;

		hit = (hasHitWorld(mx, my) ? 1 : hit);
	}

	/* Y Axis */

	if (dy != 0)
	{
		for (i = 0; i < self->w - 1; i += MAP_TILE_SIZE)
		{
			mx = (position->x + i) / MAP_TILE_SIZE;

			hit = (hasHitWorld(mx, my) ? 1 : hit);
		}

		mx = (position->x + self->w - 1) / MAP_TILE_SIZE;

		hit = (hasHitWorld(mx, my) ? 1 : hit);
	}

	if (hit)
	{
		if (dx != 0)
		{
			position->x = (mx * MAP_TILE_SIZE) - adjX;
			self->dx = self->bounce(self->dx);
		}

		if (dy != 0)
		{
			if (dy > 0)
			{
				self->isOnGround = 1;
			}
			
			position->y = (my * MAP_TILE_SIZE) - adjY;
			self->dy = self->bounce(self->dy);
			self->dy = limit(self->dy, JUMP_POWER, -JUMP_POWER);
		}

		self->touch(NULL);
	}
}

static int hasHitWorld(int mx, int my)
{
	if (mx < 0 || mx >= MAP_WIDTH || my < 0 || my >= MAP_HEIGHT)
	{
		return 0;
	}

	if (isSolid(mx, my))
	{
		return 1;
	}

	return 0;
}

static void compareEnvironments(void)
{
	int prevEnv, x, y;
	
	prevEnv = self->environment;

	self->environment = ENV_AIR;
	x = self->x / MAP_TILE_SIZE;
	y = self->y / MAP_TILE_SIZE;

	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
	{
		return;
	}

	switch (world.map.data[x][y])
	{
		case MAP_TILE_LAVA:
			self->environment = ENV_LAVA;
			break;

		case MAP_TILE_WATER:
			self->environment = ENV_WATER;
			break;

		case MAP_TILE_SLIME:
			self->environment = ENV_SLIME;
			break;
	}

	if (self->environment == prevEnv)
	{
		return;
	}

	switch (prevEnv)
	{
		case ENV_WATER:
			playSound(SND_WATER_OUT, CH_EFFECTS);
			if ((self->environment == ENV_AIR) && (self->dy < 0))
			{
				self->dy = JUMP_POWER;
			}
			break;

		case ENV_AIR:
			self->dx = 0;
			self->dy = 0.25f;
			if (self->environment == ENV_WATER)
			{
				playSound(SND_WATER_IN, CH_EFFECTS);
			}
			else
			{
				playSound(SND_SLIME, CH_EFFECTS);
			}
			break;
			
		default:
			break;
	}

	self->changeEnvironment();
}

static int isObserving(void)
{
	int i;
	
	for (i = 0 ; i < MAX_ENTS_TO_OBSERVE ; i++)
	{
		if (world.entitiesToObserve[i] == self)
		{
			return 1;
		}
	}
	
	return 0;
}

void activateEntities(char *nameList, int active)
{
	Entity *oldSelf;
	char *name, names[MAX_DESCRIPTION_LENGTH];
	
	STRNCPY(names, nameList, MAX_DESCRIPTION_LENGTH);
	
	oldSelf = self;

	name = strtok(names, "|");

	while (name)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Activate '%s'", name);
		
		for (self = world.entityHead.next ; self != NULL ; self = self->next)
		{
			if (strcmp(self->name, name) == 0)
			{
				self->activate(active);
			}
		}

		name = strtok(NULL, "|");
	}
	
	self = oldSelf;
}

void teleportEntity(Entity *e, float tx, float ty)
{
	e->tx = tx;
	e->ty = ty;

	e->flags |= EF_TELEPORTING;

	addTeleportStars(e);
}

static void handleTeleport(void)
{
	float diffX, diffY;
	
	diffX = abs(self->x - self->tx) / 20;
	diffY = abs(self->y - self->ty) / 20;

	addTeleportStar(self->x + rand() % self->w, self->y + rand() % self->h);

	diffX = MAX(3, MIN(30, diffX));
	diffY = MAX(3, MIN(30, diffY));

	if (self->x > self->tx)
	{
		self->x -= diffX;
	}

	if (self->x < self->tx)
	{
		self->x += diffX;
	}

	if (self->y > self->ty)
	{
		self->y -= diffY;
	}

	if (self->y < self->ty)
	{
		self->y += diffY;
	}

	if (collision(self->x, self->y, self->w, self->h, self->tx, self->ty, self->w, self->h))
	{
		self->flags &= ~EF_TELEPORTING;
		self->x = self->tx;
		self->y = self->ty;
		addTeleportStars(self);
		self->dx = self->dy = 0;
		self->environment = ENV_AIR;
		playSound(SND_TELEPORT, CH_EFFECTS);
	}
}

void dropCarriedItem(void)
{
	EntityExt *e;
	Item *i;
	
	e = (EntityExt*)self;
	
	if (e->carriedItem != NULL)
	{
		i = e->carriedItem;
		
		i->x = (e->x + e->w / 2) - i->w / 2;
		i->y = e->y;

		i->dx = 0;
		i->dy = -9;
		i->flags &= ~EF_GONE;
		i->thinkTime = FPS / 2;

		e->carriedItem = NULL;
	}
}

void teleport(Entity *e, float tx, float ty)
{
	e->tx = tx;
	e->ty = ty;

	e->flags |= EF_TELEPORTING;

	addTeleportStars(e);
	
	if (e == (Entity*)world.bob)
	{
		terminateJetpack();
		
		world.bob->flags &= ~(EF_WATER_BREATHING | EF_WEIGHTLESS);
	}
}

Entity *findEntity(char *name)
{
	Entity *e;
	
	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (strcmp(e->name, name) == 0)
		{
			return e;
		}
	}

	return NULL;
}

Entity *getRandomObjectiveEntity(void)
{
	Entity *rtn, *e;
	
	rtn = (Entity*)world.bob;

	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->isMissionTarget && rand() % 4 == 0)
		{
			return e;
		}
	}

	return rtn;
}

static void doMarker(Marker *m, int delta)
{
	int i;
	
	for (i = 0 ; i < 3 ; i++)
	{
		m->value -= (0.05 * delta);
		m->y = 15 + (float) sin(m->value) * 5;
	}
}

static void addRider(void)
{
	int i;
	
	for (i = 0 ; i < MAX_RIDERS ; i++)
	{
		if (!riders[i])
		{
			riders[i] = self;
			return;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Couldn't add rider: out of array space.");
}

static void addTouched(Entity *e)
{
	int i;
	
	for (i = 0 ; i < MAX_TOUCHED ; i++)
	{
		if (!touched[i])
		{
			touched[i] = e;
			return;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Couldn't add touched: out of array space.");
}

void swapSelf(Entity *e)
{
	static Entity *oldSelf = NULL;
	
	if (!oldSelf)
	{
		oldSelf = self;
		self = e;
	}
	else
	{
		self = oldSelf;
		oldSelf = NULL;
	}
}

static int drawComparator(const void *a, const void *b)
{
	Entity *e1 = *((Entity**)a);
	Entity *e2 = *((Entity**)b);

	return e2->type - e1->type;
}
