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

#include "strings.h"

void initStrings(void)
{
	app.strings[ST_MISSION_COMPLETE] = _("Mission Complete!");
	app.strings[ST_OBJECTIVES] = _("Objectives");
	app.strings[ST_INCOMPLETE] = _("Incomplete");
	app.strings[ST_COMPLETE] = _("Complete");
	app.strings[ST_PRESS_FIRE] = _("Press Fire to Continue");
	
	app.strings[ST_OPTIONS] = _("Options");
	app.strings[ST_CONTROLS] = _("Controls");
	app.strings[ST_TROPHIES] = _("Trophies");
	app.strings[ST_PAGE] = _("Page %d / %d");
	app.strings[ST_HIDDEN] = _("Hidden");
	
	app.strings[ST_WEAPON] = _("Weapon: %s");
	
	app.strings[ST_CELL] = _("Found a battery cell - Max power increased!");
	app.strings[ST_HEART] = _("Found a heart - Max health increased!");
	
	app.strings[ST_TELEPORTER] = _("Teleporter activated ...");
	app.strings[ST_LASERS] = _("Lasers disabled ...");
	app.strings[ST_POWER_POINT] = _("Not enough power (%d units required)");
	app.strings[ST_LIFT] = _("Platform activated ...");
	app.strings[ST_EXIT] = _("Can't exit yet - required objectives not met");
	app.strings[ST_LOCKED] = _("Door is locked");
	app.strings[ST_OPENED] = _("Door opened ...");
	
	app.strings[ST_GOT_GRENADES] = _("Got some Grenades");
	app.strings[ST_PICKED_UP] = _("Picked up a %s");
	app.strings[ST_REMOVED] = _("%s removed");
	app.strings[ST_REQUIRED] = _("%s required");
	app.strings[ST_CANNOT_CARRY] = _("Can't carry any more keys");
	app.strings[ST_RESCUED] = _("Rescued %s");
	
	app.strings[ST_JETPACK_POWER] = _("Not enough power for jetpack");
	app.strings[ST_AQUALUNG_POWER] = _("Not enough power for aqualung");
	
	app.strings[ST_QUIT_HUB] = _("Quit and return to hub?");
	app.strings[ST_QUIT_TUTORIAL] = _("As this is a tutorial mission, you can skip it and move onto the main game.");
	app.strings[ST_QUIT_SAVE] = _("Your progress on this mission will be saved.");
	app.strings[ST_QUIT_LOSE] = _("Warning: if you quit now, you will lose all progress on this level.");
	
	app.strings[ST_MIAS] = _("MIAs");
	app.strings[ST_ITEMS] = _("Items");
	app.strings[ST_TARGETS] = _("Targets");
	
	app.strings[ST_OBJECTIVE_COMPLETE] = _("%s - Objective Complete!");
	
	app.strings[ST_CHERRY_BUNCH] = _("bunch of cherries");
	app.strings[ST_CHERRY_PAIR] = _("pair of cherries");
	app.strings[ST_CHERRY_SMALL] = _("small cherry");
	
	app.strings[ST_BATTERY_FULL] = _("full battery");
	app.strings[ST_BATTERY] = _("battery");
	app.strings[ST_USED_BATTERY] = _("used battery");
	app.strings[ST_WEAK_BATTERY] = _("weak battery");
	
	app.strings[ST_CHOOSE_SAVE] = _("Choose a save slot to use ...");
	app.strings[ST_LOAD] = _("Choose a save game to load ...");
	app.strings[ST_OVERWRITE_1] = _("Are you sure you want to overwrite this game?");
	app.strings[ST_OVERWRITE_2] = _("All progress will be lost!");
	
	app.strings[ST_FREEPLAY] = _("As the game is now complete, free play for this mission has been unlocked. You may replay it as often as you wish.");
	app.strings[ST_HEART_CELL] = _("All objectives for this misson have been completed. However, there is a Cell or a Heart left to find. See if you can locate it.");
	
	app.strings[ST_HUB_MISSIONS] = _("Missions: %d / %d");
	app.strings[ST_HUB_MIAS] = _("MIAs: %d / %d");
	app.strings[ST_HUB_TARGETS] = _("Targets: %d / %d");
	app.strings[ST_HUB_KEYS] = _("Keys: %d / %d");
	app.strings[ST_HUB_HEARTS] = _("Hearts: %d / %d");
	app.strings[ST_HUB_CELLS] = _("Cells: %d / %d");
}
