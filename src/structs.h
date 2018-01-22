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

typedef struct Texture Texture;
typedef struct Lookup Lookup;
typedef struct Quadtree Quadtree;
typedef struct Entity Entity;
typedef struct Objective Objective;
typedef struct Trigger Trigger;
typedef struct Marker Marker;

typedef struct {
	int debug;
	int takeScreenshots;
	char *screenshotFolder;
	int noAIWeapons;
	int showFPS;
	int playerImmortal;
	int playerUnlimitedMissiles;
	int noEntityActions;
	int allImmortal;
	int fps;
} Dev;

struct Texture {
	char name[MAX_DESCRIPTION_LENGTH];
	long hash;
	long ttl;
	SDL_Texture *texture;
	Texture *next;
};

typedef struct {
	SDL_Color red;
	SDL_Color orange;
	SDL_Color yellow;
	SDL_Color green;
	SDL_Color blue;
	SDL_Color cyan;
	SDL_Color purple;
	SDL_Color white;
	SDL_Color black;
	SDL_Color lightGrey;
	SDL_Color darkGrey;
} Colors;

struct Lookup {
	char name[MAX_NAME_LENGTH];
	long value;
	Lookup *next;
};

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
	void (*handleClick)(int x, int y, int btn);
	void (*handleDrag)(int x, int y, int dx, int dy, int cx, int cy);
	void (*handleMouseUp)(int x, int y, int btn);
} Delegate;

struct Entity {
	char name[MAX_NAME_LENGTH];
	int type;
	float x;
	float y;
	int w;
	int h;
	int health;
	int alive;
	float dx;
	float dy;
	int reload;
	int isOnGround;
	int facing;
	int damage;
	int weaponType;
	int shotsToFire;
	int isSolid;
	int value;
	long flags;
	SDL_Rect bounds;
	int sprite[3];
	int spriteTime;
	int spriteFrame;
	Entity *owner;
	void (*walk)(void);
	void (*attack)(void);
	void (*touch)(Entity *other);
	Entity *next;
};

struct Objective {
	char id[MAX_NAME_LENGTH];
	char description[MAX_DESCRIPTION_LENGTH];
	char targetName[MAX_NAME_LENGTH];
	int currentValue;
	int targetValue;
	int totalValue;
	int required;
	Objective *next;
};

/* How you going, Dave? */
struct Trigger {
	char name[MAX_NAME_LENGTH];
	char message[MAX_DESCRIPTION_LENGTH];
	char targetNames[MAX_DESCRIPTION_LENGTH];
	int x;
	int y;
	int w;
	int h;
	Trigger *next;
};

typedef struct {
	SDL_Rect bounds;
	float shakeAmount;
	int x;
	int y;
} Camera;

typedef struct {
	int x;
	int y;
	int w;
	int h;
	int dx;
	int dy;
	int button[MAX_MOUSE_BUTTONS];
	int dragging;
} Mouse;

typedef struct {
	char saveDir[MAX_FILENAME_LENGTH];
	int winWidth;
	int winHeight;
	float scaleX;
	float scaleY;
	int fullscreen;
	int musicVolume;
	int soundVolume;
	int hideMouse;
	Mouse mouse;
	int keyboard[MAX_KEYBOARD_KEYS];
	SDL_Texture *backBuffer;
	SDL_Renderer *renderer;
	SDL_Window *window;
	Delegate delegate;
	int awaitingWidgetInput;
	int lastKeyPressed;
	int lastButtonPressed;
	int keyControls[CONTROL_MAX];
} App;

typedef struct {
	long timePlayed;
} Game;

struct Marker {
	int sprite;
	float value;
	float y;
	Marker *next;
};

typedef struct {
	int data[MAP_WIDTH][MAP_HEIGHT];
	int decal[MAP_WIDTH][MAP_HEIGHT];
	SDL_Rect bounds;
} Map;

struct Quadtree {
	int depth;
	int x, y, w, h;
	Entity **ents;
	int capacity;
	int numEnts;
	int addedTo;
	Quadtree *node[4];
};

typedef struct {
	Entity *bob;
	Map map;
	Entity entityHead, *entityTail;
	int allObjectivesComplete;
	int currentStatus;
	int isBossMission;
	Quadtree quadtree;
	Objective objectiveHead, *objectiveTail;
	Trigger triggerHead, *triggerTail;
} World;
