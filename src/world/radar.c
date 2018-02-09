#define RADAR_TILE_SIZE 32
#define OFFSET_X ((SCREEN_WIDTH - (RADAR_TILE_SIZE * RADAR_TILE_SIZE)) / 2)
#define OFFSET_Y ((SCREEN_HEIGHT - (RADAR_TILE_SIZE * RADAR_TILE_SIZE)) / 2)

static void logic(void);
static void draw(void);
static void drawMap(void);

static SDL_Rect viewRect;

void initRadar(void)
{
	app.delegate.logic = logic;
	app.delegate.logic = draw;

	viewRect.x = (world.bob->x / MAP_TILE_SIZE) - (RADAR_TILE_SIZE / 2);
	viewRect.y = (world.bob->y / MAP_TILE_SIZE) - (RADAR_TILE_SIZE / 2);
	viewRect.w = RADAR_TILE_SIZE;
	viewRect.h = RADAR_TILE_SIZE;
}

static void logic(void)
{

}

static void draw(void)
{
	drawMap();
}

static void drawMap(void)
{
	int x, y, mx, my;

	for (x = 0 ; x < viewRect.w ; x++)
	{
		for (y = 0 ; y < viewRect.h ; y++)
		{
			mx = viewRect.x + x;
			my = viewRect.y + y;

			if (withinMap(mx, my))
			{
				drawRect(OFFSET_X + (x * RADAR_TILE_SIZE), OFFSET_Y + (y * RADAR_TILE_SIZE), RADAR_TILE_SIZE - 1, RADAR_TILE_SIZE - 1, 0, 200, 0, 255);
			}
		}	
	}
}

static void drawEntities(void)
{
	Entity *e;
	Entity **candidates;
	int i, x, y;

	candidates = getAllEntsWithin(viewRect.x * MAP_TILE_SIZE, viewRect.y * MAP_TILE_SIZE, viewRect.w * MAP_TILE_SIZE, viewRect.h * MAP_TILE_SIZE, NULL);
	
	for (i = 0, e = candidates[i] ; e != NULL ; e = candidates[++i])
	{
		x = viewRect.x + (e->x / RADAR_TILE_SIZE);
		y = viewRect.y + (e->y / RADAR_TILE_SIZE);

		drawRect(OFFSET_X + (x * RADAR_TILE_SIZE), OFFSET_Y + (y * RADAR_TILE_SIZE), RADAR_TILE_SIZE - 1, RADAR_TILE_SIZE - 1, 200, 0, 0, 255);
	}
}
