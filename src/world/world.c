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
static void drawNormal(void);
static void addHelperItems(void);
static void spawnEnemies(void);
static int canAdd(Unit *u, int mx, int my);
static void startMission(void);
static void drawInGameWidgets(void);
static void handleWidgets(void);
static void resume(void);
static void options(void);
static void stats(void);
static void trophies(void);
static void quit(void);

static Texture *background;
static int observationIndex;
static int showingWidgets;

void initWorld(void)
{
	SDL_ShowCursor(SDL_DISABLE);
	
	background = getTexture(world.background);
	
	loadMusic(world.music);
	
	initQuadtree(&world.quadtree);
	
	initObjectives();
	
	initParticles();
	
	initHud();
	
	initWeapons();
	
	initEffects();

	initItems();

	world.enemySpawnTimer = (FPS * rrnd(world.minEnemySpawnTime, world.maxEnemySpawnTime));

	world.state = WS_START;

	observationIndex = 0;
	
	getWidget("resume", "gamePaused")->action = resume;
	getWidget("options", "gamePaused")->action = options;
	getWidget("stats", "gamePaused")->action = stats;
	getWidget("trophies", "gamePaused")->action = trophies;
	getWidget("quit", "gamePaused")->action = quit;

	if (world.missionType == MT_BOSS)
	{
		startMission();
		hideAllWidgets();
		world.betweenTimer = 0;
	}
	else
	{
		playMusic(1);
	}

	if (!game.isResumingMission)
	{
		game.stats[STAT_MISSIONS_PLAYED]++;
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
	
	switch (world.state)
	{
		case WS_PAUSED:
			drawNormal();
			drawMissionStatus();
			break;
			
		case WS_START:
			drawNormal();
			drawMissionStatus();
			drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80, 24, TA_CENTER, colors.white, _("Press Fire to Continue"));
			break;
			
		default:
			if (world.betweenTimer == 0)
			{
				drawNormal();
				drawHud();
			}
			break;
	}
	
	if (showingWidgets)
	{
		drawInGameWidgets();
	}
}

static void drawInGameWidgets(void)
{
	int w, h;
	
	w = 300;
	h = 550;
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 64);
	
	drawRect((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2, w, h, 0, 0, 0, 192);
	drawOutlineRect((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2, w, h, 255, 255, 255, 255);
	
	drawWidgets();
}

static void drawNormal(void)
{
	blitScaled(background->texture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	
	drawEntities(PLANE_BACKGROUND);
	
	drawParticles(PLANE_BACKGROUND);
	
	drawMap();
	
	drawEntities(PLANE_FOREGROUND);
	
	drawParticles(PLANE_FOREGROUND);
}

static void startMission(void)
{
	Entity *self;
	SDL_Rect *r;
	
	self = (Entity*)world.bob;
	
	world.state = WS_IN_PROGRESS;
	world.betweenTimer = FPS / 2;
	
	r = &self->sprite[self->facing]->frames[self->spriteFrame]->rect;	
	self->w = r->w;
	self->h = r->h;
	
	resetAtCheckpoint();
	
	world.entityToTrack = self;
	self->flags &= ~EF_GONE;
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
	
	if (isAcceptControl())
	{
		clearControls();
		
		startMission();
	}
}

static void doWorldInProgress(void)
{
	cameraTrack(world.entityToTrack);

	doPlayer();
	
	if (!showingWidgets)
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
		
		if (isControl(CONTROL_PAUSE))
		{
			world.state = WS_PAUSED;
			clearControl(CONTROL_PAUSE);
		}
		
		if (isControl(CONTROL_MAP))
		{
			initRadar();
			clearControl(CONTROL_MAP);
		}
		
		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
			showWidgetGroup("gamePaused");
			playSound(SND_MENU_BACK, 0);
			showingWidgets = 1;
		}
		
		if (world.observationTimer > 0)
		{
			if (--world.observationTimer == FPS * 1.5)
			{
				world.entityToTrack = world.entitiesToObserve[0];

				world.state = WS_OBSERVING;
			}
		}
	}
	else
	{
		handleWidgets();
	}
}

static void handleWidgets(void)
{
	doWidgets();
	
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		resume();
	}
}

static void doWorldObserving(void)
{
	int i;
	
	cameraTrack(world.entityToTrack);
	
	if (--world.observationTimer == 0)
	{
		if (++observationIndex < MAX_ENTS_TO_OBSERVE && world.entitiesToObserve[observationIndex] != NULL)
		{
			world.entityToTrack = world.entitiesToObserve[observationIndex];

			world.observationTimer = FPS * 1.5;
		}
		else
		{
			for (i = 0 ; i < MAX_ENTS_TO_OBSERVE ; i++)
			{
				if (world.entitiesToObserve[i] != NULL)
				{
					world.entitiesToObserve[i]->observationTime = FPS * 5;
				}
			}
			
			memset(world.entitiesToObserve, 0, sizeof(Entity*) * MAX_ENTS_TO_OBSERVE);
			world.entityToTrack = (Entity*)world.bob;
			world.state = WS_IN_PROGRESS;

			observationIndex = 0;
		}
	}
}

static void doWorldPaused(void)
{
	animateSprites();
	
	if (isControl(CONTROL_PAUSE))
	{
		world.state = WS_IN_PROGRESS;
		clearControl(CONTROL_PAUSE);
	}
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
	world.frameCounter %= (FPS * 10);

	doHud();

	if (world.minEnemySpawnTime > 0 && !dev.cheatNoEnemies)
	{
		spawnEnemies();
	}

	if (world.missionType == MT_BOSS && --world.helperItemTimer <= 0)
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
			sprintf(name, "%s%s", world.enemyTypes[r], (rand() % 2 ? "Blob" : "EyeDroid"));

			u = (Unit*)createEntity(name);
			
			self = (Entity*)u;

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

	if (e->observationTime == 0 && !isOnScreen(e))
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

void exitRadar(void)
{
	startSectionTransition();
	
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	endSectionTransition();
}

static void resume(void)
{
	app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
	hideAllWidgets();
	showingWidgets = 0;
}

static void options(void)
{
}

static void stats(void)
{
}

static void trophies(void)
{
}

static void quit(void)
{
}
