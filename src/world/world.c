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
static void returnFromTrophyStats(void);
static void drawQuit(void);
static void drawGameOver(void);
void quitMission(void);
static void returnFromOptions(void);
void autoCompleteMission(void);
static void retry(void);
static void hub(void);
static void title(void);

static Texture *background;
static Texture *atlasTexture;
static Atlas *missionFailed;
static int observationIndex;
static int showing;
static PointF observePos;

void initWorld(void)
{
	startSectionTransition();
	
	loadWorld(game.worldId);
	
	world.currentStatus = getMissionStatus(game.worldId);
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "world.currentStatus = %d", world.currentStatus);
	
	background = getTexture(world.background);
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	missionFailed = getImageFromAtlas("gfx/main/missionFailed.png");
	
	loadMusic(world.music);
	
	initQuadtree(&world.quadtree);
	
	initObjectives();
	
	initParticles();
	
	initHud();
	
	initWeapons();
	
	initEffects();

	initItems();

	initMap();
	
	initEntities();
	
	addKeysFromStash();

	world.enemySpawnTimer = (FPS * rrnd(world.minEnemySpawnTime, world.maxEnemySpawnTime));

	world.state = WS_START;

	observationIndex = 0;
	
	getWidget("resume", "gamePaused")->action = resume;
	getWidget("options", "gamePaused")->action = options;
	getWidget("stats", "gamePaused")->action = stats;
	getWidget("trophies", "gamePaused")->action = trophies;
	getWidget("quit", "gamePaused")->action = quit;
	
	getWidget("ok", "stats")->action = returnFromTrophyStats;
	getWidget("ok", "trophies")->action = returnFromTrophyStats;
	getWidget("ok", "gameQuit")->action = quitMission;
	getWidget("cancel", "gameQuit")->action = returnFromTrophyStats;
	
	getWidget("retry", "gameOver")->action = retry;
	getWidget("hub", "gameOver")->action = hub;
	getWidget("title", "gameOver")->action = title;

	if (world.missionType == MT_BOSS)
	{
		startMission();
		hideAllWidgets();
		world.betweenTimer = 0;
	}
	else
	{
		world.bob->flags |= EF_GONE;
		
		playMusic(1);
	}
	
	game.stats[STAT_MISSIONS_PLAYED]++;
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	app.restrictTrophyAlert = 1;
	
	endSectionTransition();
	
	/*
	startMission();
	world.bob->x = 17 * MAP_TILE_SIZE;
	world.bob->y = 89 * MAP_TILE_SIZE;
	*/
}

static void logic(void)
{
	if (--world.betweenTimer <= 0)
	{
		world.betweenTimer = 0;
		
		world.saveDelay = limit(world.saveDelay - 1, 0, FPS);
		
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
			case WS_QUIT:
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
			drawMissionStatus(0);
			break;
			
		case WS_START:
			drawNormal();
			drawMissionStatus(1);
			break;
			
		case WS_GAME_OVER:
			drawNormal();
			drawGameOver();
			break;
			
		default:
			if (world.betweenTimer == 0)
			{
				drawNormal();
				drawHud();
			}
			break;
	}
	
	switch (showing)
	{
		case SHOW_WIDGETS:
			drawInGameWidgets();
			break;
			
		case SHOW_STATS:
			drawStats();
			break;
			
		case SHOW_TROPHIES:
			drawTrophies();
			break;
			
		case SHOW_QUIT:
			drawQuit();
			break;
	}
}

static void drawInGameWidgets(void)
{
	drawRect(0, 0, app.config.winWidth, app.config.winHeight, 0, 0, 0, 128);
	
	SDL_SetRenderTarget(app.renderer, app.uiBuffer);
	
	drawWidgetFrame();
	
	drawWidgets();
	
	SDL_SetRenderTarget(app.renderer, app.backBuffer);
}

static void drawNormal(void)
{
	blitScaled(background->texture, 0, 0, app.config.winWidth, app.config.winHeight, 0);
	
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
	
	if (showing == SHOW_NONE)
	{
		doBob();

		doCommon();

		doLocationTriggers();
		
		world.time++;

		if (world.allObjectivesComplete && world.state != WS_COMPLETE)
		{
			world.bob->flags |= EF_IMMUNE;
			if (world.bob->stunTimer > 0)
			{
				world.bob->stunTimer = 0;
			}

			if (strcmp(world.id, "teeka") == 0)
			{
				world.state = WS_GAME_COMPLETE;
			}
			else
			{
				world.state = WS_COMPLETE;
			}

			world.missionCompleteTimer = FPS * 3;

			if (world.state != WS_GAME_COMPLETE)
			{
				stopMusic();
			}
		}
		
		if (isControl(CONTROL_PAUSE))
		{
			world.state = WS_PAUSED;
			pauseSound(1);
			clearControl(CONTROL_PAUSE);
		}
		
		if (isControl(CONTROL_MAP))
		{
			pauseSound(1);
			initRadar();
			clearControl(CONTROL_MAP);
		}
		
		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
			showWidgetGroup("gamePaused");
			playSound(SND_MENU_BACK, 0);
			showing = SHOW_WIDGETS;
		}
		
		if (world.observationTimer > 0)
		{
			if (--world.observationTimer == FPS)
			{
				world.entityToTrack = world.entitiesToObserve[0];
				observePos.x = camera.x;
				observePos.y = camera.y;
				world.state = WS_OBSERVING;
				pauseSound(1);
			}
		}
	}
	else if (showing == SHOW_WIDGETS)
	{
		handleWidgets();
	}
	else if (showing == SHOW_STATS)
	{
		doStats();

		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			playSound(SND_MENU_BACK, 0);
			
			returnFromTrophyStats();
		}
	}
	else if (showing == SHOW_TROPHIES)
	{
		doTrophies();
		
		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			playSound(SND_MENU_BACK, 0);
			
			returnFromTrophyStats();
		}
	}
	else if (showing == SHOW_QUIT)
	{
		handleWidgets();
		
		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			returnFromTrophyStats();
		}
	}
}

static void handleWidgets(void)
{
	doWidgets();
	
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		playSound(SND_MENU_BACK, 0);
		
		resume();
	}
}

static void doWorldObserving(void)
{
	int tx, ty;
	float diffX, diffY;
	
	tx = world.entityToTrack->x - (app.config.winWidth / 2);
	ty = world.entityToTrack->y - (app.config.winHeight / 2);
	
	doEntitiesStatic();
	
	diffX = abs(camera.x - tx) / 20;
	diffY = abs(camera.y - ty) / 20;

	diffX = MAX(3, MIN(50, diffX));
	diffY = MAX(3, MIN(50, diffY));

	if (observePos.x > tx)
	{
		observePos.x -= diffX;
	}

	if (observePos.x < tx)
	{
		observePos.x += diffX;
	}

	if (observePos.y > ty)
	{
		observePos.y -= diffY;
	}

	if (observePos.y < ty)
	{
		observePos.y += diffY;
	}
	
	camera.x = observePos.x;
	camera.y = observePos.y;
	
	clipCamera();

	if (collision(observePos.x, observePos.y, MAP_TILE_SIZE, MAP_TILE_SIZE, tx, ty, MAP_TILE_SIZE, MAP_TILE_SIZE))
	{
		if (--world.observationTimer <= 0)
		{
			if (++observationIndex < MAX_ENTS_TO_OBSERVE && world.entitiesToObserve[observationIndex] != NULL)
			{
				world.entityToTrack = world.entitiesToObserve[observationIndex];
				world.observationTimer = FPS;
				observePos.x = camera.x;
				observePos.y = camera.y;
			}
			else
			{
				memset(world.entitiesToObserve, 0, sizeof(Entity*) * MAX_ENTS_TO_OBSERVE);
				world.entityToTrack = (Entity*)world.bob;
				world.state = WS_IN_PROGRESS;
				observationIndex = 0;
				pauseSound(0);
			}
		}
	}
}

static void doWorldPaused(void)
{
	animateSprites();
	
	if (isControl(CONTROL_PAUSE))
	{
		pauseSound(0);
		world.state = WS_IN_PROGRESS;
		clearControl(CONTROL_PAUSE);
	}
}

static void doWorldComplete(void)
{
	world.missionCompleteTimer--;

	if (world.missionCompleteTimer <= 0 && world.saveDelay <= 0)
	{
		dropCarriedItems();
		
		initPostMission();
	}
	else if (world.missionCompleteTimer == FPS * 1.5)
	{
		world.bob->flags |= EF_GONE;
		addTeleportStars((Entity*)world.bob);
		playSound(SND_TELEPORT, world.bob->uniqueId % MAX_SND_CHANNELS);
	}
	else
	{
		doBob();
	}

	doCommon();
}

static void doGameComplete(void)
{
	if (--world.missionCompleteTimer <= 0)
	{
		dropCarriedItems();
		
		initPostMission();
	}
	else
	{
		doBob();
	}

	doCommon();
}

static void doGameOver(void)
{
	if (world.gameOverTimer == -FPS)
	{
		stopMusic();
	}
	else if (world.gameOverTimer == -FPS * 2)
	{
		loadMusic("music/Sadness.ogg");
		playMusic(0);
		showWidgetGroup("gameOver");
	}
	
	world.gameOverTimer = MAX(-FPS * 5, world.gameOverTimer - 1);

	doCommon();
	
	if (world.gameOverTimer <= -FPS * 3)
	{
		doWidgets();
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

		if (x >= world.map.bounds.x && y >= world.map.bounds.y && x < world.map.bounds.w + app.config.winWidth - 64 && y < world.map.bounds.h + app.config.winHeight - 64)
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
				playBattleSound(SND_APPEAR, u->uniqueId % MAX_SND_CHANNELS, u->x, u->y);
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

void observeActivation(Entity *e)
{
	int i;
	
	if (!isOnScreen(e) && (!(e->flags & EF_NO_OBSERVE)))
	{
		e->flags |= EF_NO_OBSERVE;
		
		for (i = 0 ; i < MAX_ENTS_TO_OBSERVE ; i++)
		{
			if (world.entitiesToObserve[i] == NULL)
			{
				world.entitiesToObserve[i] = e;
				world.observationTimer = FPS * 1.5;
				return;
			}
			else if (getDistance(e->x, e->y, world.entitiesToObserve[i]->x, world.entitiesToObserve[i]->y) < app.config.winHeight - 50)
			{
				return;
			}
		}
		
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Can't observe entity - out of array space");
		exit(1);
	}
}

void drawGameOver(void)
{
	int fadeAmount;
	
	fadeAmount = 0;
	
	if (world.gameOverTimer <= -FPS)
	{
		fadeAmount = MIN((world.gameOverTimer + FPS) * -1, 128);
	}
	
	drawRect(0, 0, app.config.winWidth, app.config.winHeight, 0, 0, 0, fadeAmount);
	
	if (world.gameOverTimer <= -FPS * 2)
	{
		blitRect(atlasTexture->texture, app.config.winWidth / 2, 240, &missionFailed->rect, 1);
		
		if (world.gameOverTimer <= -FPS * 3)
		{
			drawWidgetFrame();
			
			drawWidgets();
		}
	}
}

void drawQuit(void)
{
	SDL_Rect r;
	
	drawRect(0, 0, app.config.winWidth, app.config.winHeight, 0, 0, 0, 128);
	
	SDL_SetRenderTarget(app.renderer, app.uiBuffer);
	
	r.w = 650;
	r.h = 325;
	r.x = (UI_WIDTH / 2) - r.w / 2;
	r.y = (UI_HEIGHT / 2) - r.h / 2;
	
	drawRect(r.x, r.y, r.w, r.h, 0, 0, 0, 192);
	
	drawOutlineRect(r.x, r.y, r.w, r.h, 200, 200, 200, 255);
	
	limitTextWidth(r.w - 100);
	drawText(UI_WIDTH / 2, r.y + 10, 26, TA_CENTER, colors.white, app.strings[ST_QUIT_HUB]);
	
	if (world.missionType == MT_TRAINING)
	{
		drawText(UI_WIDTH / 2, r.y + 65, 26, TA_CENTER, colors.white, app.strings[ST_QUIT_TUTORIAL]);
	}
	else if (game.isComplete)
	{
		drawText(UI_WIDTH / 2, r.y + 65, 26, TA_CENTER, colors.white, app.strings[ST_QUIT_FREE_PLAY]);
	}
	else if (world.isReturnVisit)
	{
		drawText(UI_WIDTH / 2, r.y + 65, 26, TA_CENTER, colors.white, app.strings[ST_QUIT_SAVE]);
	}
	else
	{
		drawText(UI_WIDTH / 2, r.y + 65, 26, TA_CENTER, colors.white, app.strings[ST_QUIT_LOSE]);
	}
	
	limitTextWidth(0);
	
	drawWidgets();
	
	SDL_SetRenderTarget(app.renderer, app.backBuffer);
}

void exitRadar(void)
{
	startSectionTransition();
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	endSectionTransition();
}

static void resume(void)
{
	app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
	hideAllWidgets();
	showing = SHOW_NONE;
}

static void options(void)
{
	initOptions(returnFromOptions);
}

static void stats(void)
{
	showing = SHOW_STATS;
	initStatsDisplay();
	showWidgetGroup("stats");
}

static void trophies(void)
{
	showing = SHOW_TROPHIES;
	showWidgetGroup("trophies");
}

static void quit(void)
{
	showing = SHOW_QUIT;
	showWidgetGroup("gameQuit");
}

static void retry(void)
{
	retryMission();
}

static void hub(void)
{
	returnToHub();
}

static void title(void)
{
	returnToTitle();
}

static void returnFromTrophyStats(void)
{
	showWidgetGroup("gamePaused");
	showing = SHOW_WIDGETS;
	app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
}

void quitMission(void)
{
	resume();
	stopMusic();
	world.state = WS_QUIT;
	world.missionCompleteTimer = (FPS * 1.5) + 1;
	
	if (world.missionType == MT_TRAINING)
	{
		autoCompleteMission();
	}
}

static void returnFromOptions(void)
{
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	returnFromTrophyStats();
}

void autoCompleteMission(void)
{
	Objective *o;
	Entity *e;
	
	for (o = world.objectiveHead.next ; o != NULL ; o = o->next)
	{
		o->currentValue = o->targetValue;
	}
	
	for (e = world.entityHead.next ; e != NULL ; e = e->next)
	{
		switch (e->type)
		{
			case ET_MIA:
				e->alive = ALIVE_DEAD;
				game.stats[STAT_MIAS_RESCUED]++;
				break;
				
			case ET_KEY:
				if (!(e->flags & EF_GONE))
				{
					e->alive = ALIVE_DEAD;
					game.stats[STAT_KEYS_FOUND]++;
				}
				break;
				
			default:
				break;
		}
	}
	
	world.state = WS_COMPLETE;
}

void destroyWorld(void)
{
	int i;

	for (i = 0 ; i < world.numEnemyTypes ; i++)
	{
		free(world.enemyTypes[i]);
	}

	free(world.enemyTypes);

	destroyTriggers();

	destroyObjectives();

	destroyEntities();

	destroyParticles();

	destroyQuadtree();
}
