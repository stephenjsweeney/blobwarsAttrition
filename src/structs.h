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
typedef struct Objective Objective;
typedef struct Trigger Trigger;
typedef struct Marker Marker;
typedef struct Particle Particle;
typedef struct Sprite Sprite;
typedef struct Tuple Tuple;
typedef struct HubMission HubMission;
typedef struct Widget Widget;
typedef struct Atlas Atlas;
typedef struct Bucket Bucket;
typedef struct EntityDef EntityDef;
typedef struct Trophy Trophy;
typedef struct cJSON cJSON;

typedef struct Entity Entity;
typedef struct EntityExt EntityExt;
typedef struct Bob Bob;
typedef struct Boss Boss;
typedef struct Bullet Bullet;
typedef struct Decoration Decoration;
typedef struct Item Item;
typedef struct MIA MIA;
typedef struct Structure Structure;
typedef struct Trap Trap;
typedef struct Unit Unit;

typedef struct {
	int debug;
	int takeScreenshots;
	char *screenshotFolder;
	int showFPS;
	int cheatHealth;
	int cheatPower;
	int cheatOxygen;
	int cheatKeys;
	int cheatLevels;
	int cheatReload;
	int cheatBlind;
	int cheatNoEnemies;
	int cheatStatic;
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
	float x;
	float y;
} PointF;

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

struct EntityDef {
	char name[MAX_NAME_LENGTH];
	Entity *(*initFunc)(void);
	EntityDef *next;
};

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
	void (*handleClick)(int x, int y, int btn);
	void (*handleDrag)(int x, int y, int dx, int dy, int cx, int cy);
	void (*handleMouseUp)(int x, int y, int btn);
} Delegate;

struct Entity {
	unsigned long uniqueId;
	int type;
	char name[MAX_NAME_LENGTH];
	float x, y;
	int w, h;
	int tx, ty;
	float dx, dy;
	int health, healthMax;
	int alive;
	int active;
	int environment;
	int thinkTime;
	int facing;
	Sprite *sprite[3];
	int spriteTime;
	int spriteFrame;
	int plane;
	int isSolid;
	int isStatic;
	int isOnGround;
	int isVisible;
	int isMissionTarget;
	int observationTime;
	Entity *riding;
	Entity *owner;
	unsigned long flags;
	void (*init)(void);
	void (*action)(void);
	void (*touch)(Entity *other);
	void (*tick)(void);
	void (*reset)(void);
	void (*die)(void);
	void (*animate)(void);
	void (*walk)(void);
	float (*bounce)(float x);
	void (*teleport)(float tx, float ty);
	void (*activate)(int active);
	void (*applyDamage)(int amount);
	void (*changeEnvironment)(void);
	SDL_Rect *(*getCurrentSprite)(void);
	void (*load)(cJSON *root);
	void (*save)(cJSON *root);
	Entity *next;
};

struct EntityExt {
	struct Entity;
	char spriteName[MAX_NAME_LENGTH];
	Item *carriedItem;
};

struct Unit {
	struct EntityExt;
	char *unitType;
	int weaponType;
	int canCarryItem;
	int reload;
	int shotsToFire;
	int maxShotsToFire;
	int spawnedInTimer;
	int oxygen;
	int spawnedIn;
	int returnToStartTimer;
	int startX, startY;
	void (*preFire)(void);
	void (*attack)(void);
	int (*canFire)(Entity *target);
};

struct MIA {
	struct Unit;
	int shudderTimer;
	int starTimer;
	int teleportTimer;
};

struct Boss {
	struct Unit;
	int weakAgainst;
	int teleportTimer;
	int stunTimer;
};

struct Item {
	struct EntityExt;
	int startX, startY;
	int power;
	int value;
	int weaponType;
	int provided;
	int collected;
	int canBeCarried;
	int canBePickedUp;
};

struct Bob {
	struct Unit;
	int outTimer;
	int stunTimer;
	int immuneTimer;
	float power, powerMax;
	int jpEffectTimer;
	PointF checkpoints[MAX_CHECKPOINTS];
	Item *items[MAX_ITEMS];
};

struct Structure {
	struct EntityExt;
	int bobTouching;
	char requiredItem[MAX_NAME_LENGTH];
	char targetNames[MAX_DESCRIPTION_LENGTH];
	char message[MAX_DESCRIPTION_LENGTH];
	int state;
	int waitTime;
	int startX, startY;
	int closedX, closedY;
	int isLocked;
	int speed;
	int requiredPower;
	int isWeighted;
	int weightApplied;
	float sinVal;
};

struct Decoration {
	struct Entity;
	int effectType;
	int bleedTime;
};

struct Trap {
	struct EntityExt;
	int onTime;
	int offTime;
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

struct HubMission {
	char id[MAX_NAME_LENGTH];
	char name[MAX_NAME_LENGTH];
	char description[MAX_DESCRIPTION_LENGTH];
	int x;
	int y;
	int status;
	int unlockCount;
	float distance;
	HubMission *next;
};

struct Tuple {
	char key[MAX_NAME_LENGTH];
	union {
		char s[MAX_NAME_LENGTH];
		int i;
		float f;
	} value;
	Tuple *next;
};

struct Trophy {
	char id[MAX_NAME_LENGTH];
	char title[MAX_DESCRIPTION_LENGTH];
	char description[MAX_DESCRIPTION_LENGTH];
	char awardDateStr[MAX_NAME_LENGTH];
	int value;
	int hidden;
	int stat;
	int statValue;
	unsigned long awardDate;
	int notify;
	Trophy *next;
};

typedef struct {
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
	int fullscreen;
	int soundVolume;
	int musicVolume;
	int keyControls[CONTROL_MAX];
	int joypadControls[CONTROL_MAX];
	int inventory;
	int blood;
	int trophyScreenshot;
	int trophyAlert;
} Config;

typedef struct {
	char saveDir[MAX_FILENAME_LENGTH];
	int winWidth;
	int winHeight;
	float scaleX;
	float scaleY;
	Mouse mouse;
	SDL_Joystick *joypad;
	int keyboard[MAX_KEYBOARD_KEYS];
	int joypadButton[SDL_CONTROLLER_BUTTON_MAX];
	int joypadAxis[JOYPAD_AXIS_MAX];
	SDL_Texture *backBuffer;
	SDL_Renderer *renderer;
	SDL_Window *window;
	Delegate delegate;
	int awaitingWidgetInput;
	int lastKeyPressed;
	int lastButtonPressed;
	int restrictTrophyAlert;
	Config config;
} App;

typedef struct {
	int cells;
	int hearts;
	int totalMIAs;
	int totalTargets;
	int totalCells;
	int totalHearts;
	int totalKeys;
	unsigned int stats[STAT_MAX];
	char worldId[MAX_NAME_LENGTH];
	int isComplete;
	Tuple keys[MAX_KEY_TYPES];
	Tuple missionStatusHead, *missionStatusTail;
	Trophy trophyHead, *trophyTail;
} Game;

struct Marker {
	Sprite *sprite;
	float x, y;
	float value;
	int angle;
	int visible;
	int type;
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

struct Sprite {
	char name[MAX_NAME_LENGTH];
	int *times;
	char **filenames;
	Atlas **frames;
	int currentFrame;
	float currentTime;
	int w;
	int h;
	int numFrames;
	Sprite *next;
};

struct Particle {
	int type;
	int plane;
	float health;
	float x;
	float y;
	float dx;
	float dy;
	int size;
	int r;
	int g;
	int b;
	Sprite *sprite;
	float spriteTime;
	int spriteFrame;
	int destroyAfterAnim;
	int onScreen;
	Particle *next;
};

struct Bullet {
	struct Entity;
	int damage;
	int weaponType;
};

typedef struct {
	char id[MAX_NAME_LENGTH];
	char name[MAX_NAME_LENGTH];
	int numEnemyTypes;
	char **enemyTypes;
	char tileset[MAX_NAME_LENGTH];
	char music[MAX_FILENAME_LENGTH];
	char background[MAX_FILENAME_LENGTH];
	int state;
	int minEnemySpawnTime, maxEnemySpawnTime;
	unsigned long entityCounter;
	int entityChaseTimer;
	int allObjectivesComplete;
	int frameCounter;
	int currentStatus;
	int missionType;
	int isBossActive;
	int isReturnVisit;
	int missionCompleteTimer;
	int observationTimer;
	int gameOverTimer;
	int betweenTimer;
	int mapAnimTimer;
	int enemySpawnTimer;
	int helperItemTimer;
	int spawnInterval;
	int numToSpawn;
	Bob *bob;
	Boss *boss;
	Entity *entityToTrack;
	Entity *entitiesToObserve[MAX_ENTS_TO_OBSERVE];
	Map map;
	Quadtree quadtree;
	Entity entityHead, *entityTail;
	Particle particleHead, *particleTail;
	Objective objectiveHead, *objectiveTail;
	Trigger triggerHead, *triggerTail;
} World;

struct Widget {
	char name[MAX_NAME_LENGTH];
	char group[MAX_NAME_LENGTH];
	char label[MAX_NAME_LENGTH];
	int type;
	int x;
	int y;
	int w;
	int h;
	int value;
	int minValue;
	int maxValue;
	int visible;
	int numOptions;
	char **options;
	void (*action)(void);
};

struct Atlas {
	char filename[MAX_FILENAME_LENGTH];
	SDL_Rect rect;
	Atlas *next;
};

/* ===== i18n stuff ==== */

struct Bucket {
	char *key, *value;
	Bucket *next;
};

typedef struct {
	Bucket **bucket;
	int *bucketCount;
} HashTable;

typedef struct {
	int32_t magicNumber, version, stringCount;
	int32_t originalOffset, translationOffset;
} MOHeader;

typedef struct {
	int32_t length, offset;
} MOEntry;
