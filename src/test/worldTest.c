#include "worldTest.h"
 
void initWorldTest(char *worldId)
{
	loadGame();
	
	saveGame();
	
	createScreenshotFolder();
	
	if (worldId != NULL)
	{
		STRNCPY(game.worldId, worldId, MAX_NAME_LENGTH);
		initWorld();
	}
	else
	{
		initHub();
	}
}
