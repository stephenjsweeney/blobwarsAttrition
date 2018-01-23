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

#ifndef REVISION
#define REVISION 0
#endif

#ifndef DATA_DIR
#define DATA_DIR ""
#endif

#define _(string) getTranslatedString(string)

#define PI 3.14159265358979323846
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'
#define TO_RAIDANS(angleDegrees) (angleDegrees * PI / 180.0)
#define TO_DEGREES(angleRadians) (angleRadians * 180.0 / PI)

#define SAVE_FILENAME "game.save"
#define CONFIG_FILENAME "config.json"

#define SCREEN_WIDTH 	1280
#define SCREEN_HEIGHT 	720

#define MAX_KEYBOARD_KEYS	350
#define MAX_MOUSE_BUTTONS	6

#define FPS				60
#define LOGIC_RATE		(1000.0 / FPS)

#define MAX_FONTS					64
#define NUM_TEXT_BUCKETS			64
#define TEXT_TTL					(1000 * 20)

#define MAX_NAME_LENGTH				32
#define MAX_DESCRIPTION_LENGTH		512
#define MAX_LINE_LENGTH 			1024
#define MAX_FILENAME_LENGTH			1024

#define NUM_TEXTURE_BUCKETS		32

#define MAP_WIDTH					200
#define MAP_HEIGHT					200

#define MAP_TILE_SIZE				64
#define MAP_TILE_AIR				0
#define MAP_TILE_WATER				1
#define MAP_TILE_SLIME				2
#define MAP_TILE_LAVA				3
#define MAP_TILE_SOLID				4
#define MAP_TILE_NON_SOLID			200
#define MAP_TILE_ANIMATED_WATER		240
#define MAP_TILE_ANIMATED_SLIME		245
#define MAP_TILE_ANIMATED_LAVA		250
#define MAP_TILE_OUTSIDE			255
#define MAP_TILE_MAX				256

#define JUMP_POWER					1

#define ET_ENEMY					0
#define ET_KEY						1
#define ET_MIA						2
#define ET_BOB						3
#define ET_DESTRUCTABLE				4
#define ET_BOSS						5

#define EF_NONE						0
#define EF_HEART_CELL				(2 << 0)
#define EF_WEIGHTLESS				(2 << 1)
#define EF_BOMB_SHIELD				(2 << 2)
#define EF_IMMUNE					(2 << 3)

enum
{
	FACING_RIGHT,
	FACING_LEFT
};

enum
{
	WPN_PISTOL,
	WPN_PLASMA,
	WPN_SPREAD,
	WPN_LASER,
	WPN_GRENADES,
	WPN_ANY,
	WPN_AIMED_PISTOL,
	WPN_MACHINE_GUN,
	WPN_SHOTGUN,
	WPN_MISSILE
};

enum
{
	ALIVE_ALIVE
};

enum
{
	CONTROL_LEFT,
	CONTROL_RIGHT,
	CONTROL_UP,
	CONTROL_DOWN,
	CONTROL_FIRE,
	CONTROL_JUMP,
	CONTROL_JETPACK,
	CONTROL_SCREENSHOT,
	CONTROL_MAX
};

enum
{
	TA_LEFT,
	TA_RIGHT,
	TA_CENTER
};

enum
{
	MS_PARTIAL,
	MS_MISSING_HEART_CELL
};

enum
{
	PLANE_BACKGROUND,
	PLANE_FOREGROUND
};

enum
{
	PT_LINE,
	PT_POINT,
	PT_TEXTURED
};

enum
{
	MSG_STANDARD,
	MSG_GAMEPLAY,
	MSG_PROGRESS,
	MSG_OBJECTIVE
};

enum
{
	SND_SELECT,
	SND_BACK,
	SND_PISTOL,
	SND_MACHINE_GUN,
	SND_PLASMA,
	SND_THROW,
	SND_SPREAD,
	SND_SHOTGUN,
	SND_LASER,
	SND_MISSILE,
	SND_RICO_1,
	SND_RICO_2,
	SND_GRENADE_BOUNCE,
	SND_EXPLOSION,
	SND_FLESH_HIT,
	SND_METAL_HIT,
	SND_DEATH_1,
	SND_DEATH_2,
	SND_DEATH_3,
	SND_SPLAT,
	SND_DROID_DIE_1,
	SND_DROID_DIE_2,
	SND_PRESSURE_PLATE,
	SND_DOOR_START,
	SND_DOOR_FINISH,
	SND_CHERRY,
	SND_ITEM,
	SND_KEY,
	SND_WEAPON,
	SND_TELEPORT,
	SND_APPEAR,
	SND_MIA,
	SND_WATER_IN,
	SND_WATER_OUT,
	SND_SLIME,
	SND_DROWN,
	SND_DENIED,
	SND_CONFIRMED,
	SND_MISSION_COMPLETE,
	SND_HEART_CELL,
	SND_MAX
};

enum
{
	CH_PLAYER,
	CH_EXPLODE,
	CH_WEAPON,
	CH_MAX
};
