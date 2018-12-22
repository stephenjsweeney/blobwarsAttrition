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

#include "stats.h"

static int page;
static float maxPages;
static char *statDescription[STAT_MAX];
static Texture *atlasTexture;
static Atlas *left;
static Atlas *right;

void initStats(void)
{
	page = 0;
	
	maxPages = STAT_TIME_PLAYED;
	maxPages /= STATS_PER_PAGE;
	maxPages = ceil(maxPages);
	
	statDescription[STAT_MISSIONS_COMPLETE] = _("Missions complete");
	statDescription[STAT_KEYS_FOUND] = _("Keys found");
	statDescription[STAT_CELLS_FOUND] = _("Power cells found");
	statDescription[STAT_HEARTS_FOUND] = _("Hearts found");
	statDescription[STAT_TARGETS_DEFEATED] = _("Targets defeated");
	statDescription[STAT_MIAS_RESCUED] = _("MIAs rescued");
	statDescription[STAT_DEATHS] = _("Deaths");
	statDescription[STAT_SHOTS_FIRED] = _("Shots fired");
	statDescription[STAT_SHOTS_HIT] = _("Shots hit");
	statDescription[STAT_SHOT_ACCURACY] = _("Accuracy");
	statDescription[STAT_EYE_DROID_EXPLOSION_KILLS] = _("EyeDroid explosion kills");
	statDescription[STAT_GRENADE_COMBO] = _("Best grenade combo");
	statDescription[STAT_FLY_TIME] = _("Time spent flying");
	statDescription[STAT_SWIM_TIME] = _("Time spent swimming");
	statDescription[STAT_CHERRIES_PICKED_UP] = _("Cherries picked up");
	statDescription[STAT_BATTERIES_PICKED_UP] = _("Batteries picked up");
	statDescription[STAT_WEAPONS_PICKED_UP] = _("Weapons picked up");
	statDescription[STAT_ENEMIES_KILLED] = _("Enemies killed");
	statDescription[STAT_MISSIONS_PLAYED] = _("Missions played");
	statDescription[STAT_PERCENT_COMPLETE] = _("Percent complete");
	statDescription[STAT_TIME_PLAYED] = _("Time played");
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	left = getImageFromAtlas("gfx/ui/left.png");
	right = getImageFromAtlas("gfx/ui/right.png");
}

void initStatsDisplay(void)
{
	int gameDone, gameTotal;
	
	gameDone = game.stats[STAT_MISSIONS_COMPLETE] + game.stats[STAT_MIAS_RESCUED] + game.stats[STAT_TARGETS_DEFEATED] + game.stats[STAT_KEYS_FOUND] + game.stats[STAT_HEARTS_FOUND] + game.stats[STAT_CELLS_FOUND];
	
	gameTotal = game.totalMissions + game.totalMIAs + game.totalTargets + game.totalKeys + game.totalHearts + game.totalCells;
	
	game.stats[STAT_SHOT_ACCURACY] = getPercent(game.stats[STAT_SHOTS_HIT], game.stats[STAT_SHOTS_FIRED]);
	game.stats[STAT_PERCENT_COMPLETE] = getPercent(gameDone, gameTotal);
}

void doStats(void)
{
	if (isControl(CONTROL_LEFT) || app.keyboard[SDL_SCANCODE_LEFT])
	{
		playSound(SND_MENU_NAV, 0);
		page = limit(page - 1, 0, maxPages - 1);
		app.keyboard[SDL_SCANCODE_LEFT] = 0;
		clearControl(CONTROL_LEFT);
	}

	if (isControl(CONTROL_RIGHT) || app.keyboard[SDL_SCANCODE_RIGHT])
	{
		playSound(SND_MENU_NAV, 0);
		page = limit(page + 1, 0, maxPages - 1);
		app.keyboard[SDL_SCANCODE_RIGHT] = 0;
		clearControl(CONTROL_RIGHT);
	}
	
	doWidgets();
}

void drawStats(void)
{
	int i, y, startIndex;
	SDL_Rect r;
	
	drawRect(0, 0, app.config.winWidth, app.config.winHeight, 0, 0, 0, 128);
	
	SDL_SetRenderTarget(app.renderer, app.uiBuffer);
	
	r.w = 500;
	r.h = 500;
	r.x = (UI_WIDTH / 2) - r.w / 2;
	r.y = (UI_HEIGHT / 2) - r.h / 2;
	
	drawRect(r.x, r.y, r.w, r.h, 0, 0, 0, 192);
	
	drawOutlineRect(r.x, r.y, r.w, r.h, 200, 200, 200, 255);
	
	drawText(UI_WIDTH / 2, r.y + 5, 28, TA_CENTER, colors.white, "Stats");
	
	drawText(UI_WIDTH / 2, r.y + 45, 16, TA_CENTER, colors.lightGrey, "Page %d / %d", page + 1, (int)maxPages);
	
	if (page > 0)
	{
		blitRect(atlasTexture->texture, UI_WIDTH / 2 - 100, r.y + 25, &left->rect, 1);
	}
	
	if (page < maxPages - 1)
	{
		blitRect(atlasTexture->texture, UI_WIDTH / 2 + 100, r.y + 25, &right->rect, 1);
	}
	
	SDL_SetRenderDrawColor(app.renderer, 128, 128, 128, 255);
	SDL_RenderDrawLine(app.renderer, r.x, r.y + 80, r.x + r.w, r.y + 80);
	
	y = 210;
	
	startIndex = (page * STATS_PER_PAGE);
	
	for (i = startIndex ; i < startIndex + STATS_PER_PAGE ; i++)
	{
		if (i < STAT_TIME_PLAYED)
		{
			drawText(r.x + 20, y, 18, TA_LEFT, colors.white, statDescription[i]);
			
			switch (i)
			{
				case STAT_SHOT_ACCURACY:
				case STAT_PERCENT_COMPLETE:
					drawText(r.x + r.w - 20, y, 18, TA_RIGHT, colors.white, "%d%%", game.stats[i]);
					break;
					
				case STAT_SWIM_TIME:
				case STAT_FLY_TIME:
					drawText(r.x + r.w - 20, y, 18, TA_RIGHT, colors.white, "%s", timeToString(game.stats[i] / FPS, 0));
					break;
					
				default:
					drawText(r.x + r.w - 20, y, 18, TA_RIGHT, colors.white, "%d", game.stats[i]);
					break;
			}
			
			y += 40;
		}
	}
	
	drawText(r.x + 20, r.y + r.h - 95, 18, TA_LEFT, colors.white, statDescription[STAT_TIME_PLAYED]);
	drawText(r.x + r.w - 20, r.y + r.h - 95, 18, TA_RIGHT, colors.white, timeToString(game.stats[STAT_TIME_PLAYED], 1));
		
	drawWidgets();
	
	SDL_SetRenderTarget(app.renderer, app.backBuffer);
}
