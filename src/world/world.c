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

#include "world.h"

static void logic(void);
static void draw(void);
static void doWorldStart(void);
static void doWorldInProgress(void);
static void doWorldObserving(void);
static void doWorldPaused(void);
static void doWorldComplete(void);
static void doGameComplete(void);
static void doGameOver(void);
static void doCommon(void);
static void addHelperItems(void);
static void spawnEnemies(void);
static int canAdd(Unit *u, int mx, int my);
void startMission(void);

static Texture *background;

void initWorld(void)
{
	background = getTexture(world.background);
	
	initObjectives();

	world.enemySpawnTimer = (FPS * rrnd(world.minEnemySpawnTime, world.maxEnemySpawnTime));

	world.state = WS_START;

	if (world.isBossMission)
	{
		startMission();
		hideAllWidgets();
		world.betweenTimer = 0;
	}

	if (!game.isResumingMission)
	{
		game.missionsPlayed++;
	}
	
	world.bob->flags |= EF_GONE;
	
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	startMission();
}

static void logic(void)
{
	if (--world.betweenTimer <= 0)
	{
		world.betweenTimer = 0;
		
		switch (world.state)
		{
			case WS_START:
				doWorldStart();
				break;
			case WS_IN_PROGRESS:
				doWorldInProgress();
				break;
			case WS_OBSERVING:
				doWorldObserving();
				break;
			case WS_PAUSED:
				doWorldPaused();
				break;
			case WS_COMPLETE:
				doWorldComplete();
				break;
			case WS_GAME_OVER:
				doGameOver();
				break;
			case WS_GAME_COMPLETE:
				doGameComplete();
				break;
			default:
				break;
		}
	}
	
	if (--world.mapAnimTimer < 0)
	{
		world.mapAnimTimer = 4;
	}
}

static void draw(void)
{
	clearScreen();
	
	if (world.betweenTimer == 0)
	{
		blitScaled(background->texture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		
		drawEntities(PLANE_BACKGROUND);
		
		drawMap();
		
		drawEntities(PLANE_FOREGROUND);
	}
}

void startMission(void)
{
	world.state = WS_IN_PROGRESS;
	world.betweenTimer = FPS / 2;
	resetAtCheckpoint();
	world.entityToTrack = (Entity*)world.bob;
	world.bob->flags &= ~EF_GONE;
}

static void doWorldStart(void)
{
	float dist;
	
	if (world.entityToTrack != NULL)
	{
		dist = cameraChase(world.entityToTrack, 5);
		
		if ((dist <= world.entityToTrack->w && dist <= world.entityToTrack->h) || world.entityChaseTimer <= 0)
		{
			world.entityToTrack = getRandomObjectiveEntity();
			
			world.entityChaseTimer = FPS * 5;
		}
	}
	else
	{
		world.entityToTrack = getRandomObjectiveEntity();

		cameraTrack(world.entityToTrack);
	}

	world.entityChaseTimer = MAX(world.entityChaseTimer - 1, 0);
	
	doCommon();
}

static void doWorldInProgress(void)
{
	cameraTrack(world.entityToTrack);
	
	game.config.control[CONTROL_LEFT] = app.keyboard[SDL_SCANCODE_A];
	game.config.control[CONTROL_RIGHT] = app.keyboard[SDL_SCANCODE_D];
	game.config.control[CONTROL_UP] = app.keyboard[SDL_SCANCODE_W];
	game.config.control[CONTROL_DOWN] = app.keyboard[SDL_SCANCODE_S];
	game.config.control[CONTROL_JUMP] = app.keyboard[SDL_SCANCODE_I];
	
	if (!world.showingInfoMessage)
	{
		doBob();

		doCommon();

		doLocationTriggers();

		if (world.allObjectivesComplete && world.state != WS_COMPLETE)
		{
			world.bob->flags |= EF_IMMUNE;

			if (strcmp(world.id, "teeka") == 0)
			{
				world.state = WS_GAME_COMPLETE;
			}
			else
			{
				world.state = WS_COMPLETE;
			}

			world.missionCompleteTimer = FPS * 3;

			stopMusic();
		}
	}

	if (world.observationTimer > 0)
	{
		world.observationTimer--;

		if (world.observationTimer == FPS * 1.5)
		{
			world.entityToTrack = world.entitiesToObserve[0];

			world.state = WS_OBSERVING;
		}
	}
}

static void doWorldObserving(void)
{
	int i;
	
	world.observationTimer--;

	if (world.observationTimer == 0)
	{
		for (i = 0 ; i < MAX_ENTS_TO_OBSERVE ; i++)
		{
			if (world.entitiesToObserve[i])
			{
				world.entitiesToObserve[i]->observationTime = SDL_GetTicks() + 5000;
			}
		}

		memset(world.entitiesToObserve, 0, sizeof(Entity*) * MAX_ENTS_TO_OBSERVE);
		world.entityToTrack = (Entity*)world.bob;
		world.state = WS_IN_PROGRESS;
	}
}

static void doWorldPaused(void)
{
	animateSprites();
}

static void doWorldComplete(void)
{
	world.missionCompleteTimer--;

	if (world.missionCompleteTimer <= 0)
	{
		
	}
	else if (world.missionCompleteTimer == FPS * 1.5)
	{
		dropCarriedItems();
		world.bob->flags |= EF_GONE;
		addTeleportStars((Entity*)world.bob);
		playSound(SND_TELEPORT, CH_BOB);
	}
	else
	{
		doBob();
	}

	doCommon();
}

static void doGameComplete(void)
{
	world.missionCompleteTimer--;

	if (world.missionCompleteTimer <= 0)
	{
		initEnding();
	}
	else
	{
		doBob();
	}

	doCommon();
}

static void doGameOver(void)
{
	world.gameOverTimer--;

	doCommon();

	if (world.gameOverTimer <= -(FPS * 5))
	{
		initTitle();
	}
}

static void doCommon(void)
{
	animateSprites();

	world.frameCounter++;
	world.frameCounter %= 4096;

	doHud();

	if (world.minEnemySpawnTime > 0 && !dev.cheatNoEnemies)
	{
		spawnEnemies();
	}

	if (world.isBossMission && --world.helperItemTimer <= 0)
	{
		addHelperItems();
	}

	doEntities();

	doParticles();
}

static void addHelperItems(void)
{
	int x, y, w, h;

	w = world.map.bounds.w - world.map.bounds.x;
	h = world.map.bounds.h - world.map.bounds.y;

	x = world.map.bounds.x + (rand() % w);
	y = world.map.bounds.y + 1;

	if (world.map.data[x / MAP_TILE_SIZE][y / MAP_TILE_SIZE] == 0)
	{
		dropRandomCherry(x, y);
	}

	x = world.map.bounds.x + (rand() % w);
	y = world.map.bounds.y + (rand() % h);

	if (world.map.data[x / MAP_TILE_SIZE][y / MAP_TILE_SIZE] == 0)
	{
		addRandomWeapon(x, y);
	}

	world.helperItemTimer = FPS * rrnd(3, 5);
}

static void spawnEnemies(void)
{
	char name[MAX_NAME_LENGTH];
	int r, x, y;
	Unit *u;

	if (world.numToSpawn == 0)
	{
		if (--world.enemySpawnTimer <= 0)
		{
			world.numToSpawn = 3 + (rand() % 3);
			world.spawnInterval = 0;
		}
		return;
	}

	if (--world.spawnInterval <= 0)
	{
		r = (rand() % world.numEnemyTypes);

		x = world.bob->x;
		x += ((randF() - randF()) * 5) * MAP_TILE_SIZE;

		y = world.bob->y;
		y += ((randF() - randF()) * 5) * MAP_TILE_SIZE;

		if (x >= world.map.bounds.x && y >= world.map.bounds.y && x < world.map.bounds.w + SCREEN_WIDTH - 64 && y < world.map.bounds.h + SCREEN_HEIGHT - 64)
		{
			sprintf(name, "%s%s", world.enemyTypes[r], (rand() % 2 ? "Blob" : "Droid"));

			u = (Unit*) createEntity(name);

			u->animate();

			x /= MAP_TILE_SIZE;
			y /= MAP_TILE_SIZE;

			if (canAdd(u, x, y))
			{
				u->x = x * MAP_TILE_SIZE;
				u->y = y * MAP_TILE_SIZE;

				u->spawnedIn = 1;
				u->canCarryItem = 0;
				addTeleportStars((Entity*)u);
				playSound(SND_APPEAR, CH_ANY);
			}
		}

		world.spawnInterval = rrnd(FPS / 4, FPS / 2);

		if (--world.numToSpawn <= 0)
		{
			world.enemySpawnTimer = (FPS * rrnd(world.minEnemySpawnTime, world.maxEnemySpawnTime));
		}
	}
}

static int canAdd(Unit *u, int mx, int my)
{
	int i;

	if (isSolid(mx, my))
	{
		return 0;
	}

	if (isLiquid(mx, my) && (!(u->flags & EF_WATER_BREATHING)))
	{
		return 0;
	}

	if (!(u->flags & EF_WEIGHTLESS))
	{
		for (i = 0 ; i < 10 ; i++)
		{
			if (isLiquid(mx, my + i))
			{
				return 0;
			}

			if (isWalkable(mx, my + i))
			{
				return 1;
			}
		}

		/* long drop */
		return 0;
	}

	return 1;
}

int getMissionStatus(void)
{
	Objective *o;
	Entity *e;
	int status;

	status = MS_COMPLETE;

	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		if (o->required && o->currentValue < o->targetValue)
		{
			return MS_INCOMPLETE;
		}

		if (o->currentValue < o->totalValue)
		{
			status = MS_PARTIAL;
		}
	}

	if (status == MS_COMPLETE)
	{
		for (e = world.entityHead.next ; e != NULL ; e = e->next)
		{
			if (e->type == ET_HEART_CELL)
			{
				return MS_MISSING_HEART_CELL;
			}
		}
	}

	return status;
}

void observeActivation(Entity *e)
{
	int i;

	if (e->observationTime < SDL_GetTicks() && !e->isOnScreen)
	{
		for (i = 0 ; i < MAX_ENTS_TO_OBSERVE ; i++)
		{
			if (world.entitiesToObserve[i] == NULL)
			{
				world.entitiesToObserve[i] = e;
				world.observationTimer = FPS * 2;
				return;
			}
		}
		
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Can't observe entity - out of array space");
		exit(1);
	}
}
